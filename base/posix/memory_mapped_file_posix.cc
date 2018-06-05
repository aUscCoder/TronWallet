// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory_mapped_file.h"

#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <limits>

#include "base/logging.h"
#include "base/build_config.h"

#if defined(OS_ANDROID)
#include <android/api-level.h>
#endif

#include <errno.h>

#if defined(NDEBUG)

#define HANDLE_EINTR(x) ({ \
  decltype(x) eintr_wrapper_result; \
  do { \
    eintr_wrapper_result = (x); \
  } while (eintr_wrapper_result == -1 && errno == EINTR); \
  eintr_wrapper_result; \
})

#else

#define HANDLE_EINTR(x) ({ \
  int eintr_wrapper_counter = 0; \
  decltype(x) eintr_wrapper_result; \
  do { \
    eintr_wrapper_result = (x); \
  } while (eintr_wrapper_result == -1 && errno == EINTR && \
           eintr_wrapper_counter++ < 100); \
  eintr_wrapper_result; \
})

#endif  // NDEBUG

namespace base {

MemoryMappedFile::MemoryMappedFile() : data_(NULL), length_(0), file_(-1) {
}

#if !defined(OS_NACL)
bool MemoryMappedFile::MapFileRegionToMemory(
    const MemoryMappedFile::Region& region,
    Access access) {
  if (file_path_.empty())
    return false;

  // Try open file.
  int open_flags = 0;

  switch (access) {
    case READ_ONLY:
      open_flags = O_RDONLY;
      break;
    case READ_WRITE:
      open_flags = O_RDWR;
      break;
    case READ_WRITE_EXTEND:
      open_flags = O_RDWR;
      break;
  }

  static_assert(O_RDONLY == 0, "O_RDONLY must equal zero");

  int mode = S_IRUSR | S_IWUSR;
#if defined(OS_CHROMEOS)
  mode |= S_IRGRP | S_IROTH;
#endif

  file_ = HANDLE_EINTR(open(file_path_.c_str(), open_flags, mode));
  if (file_ < 0) {
    DLOG(ERROR) << "Couldn't open " << file_path_;
    return false;
  }

  off_t map_start = 0;
  size_t map_size = 0;
  int32_t data_offset = 0;

  if (region == MemoryMappedFile::Region::kWholeFile) {
    int64_t file_len = -1;
#if defined(OS_BSD) || defined(OS_MACOSX) || defined(OS_NACL)
    struct stat file_stat;
    if (!fstat(file_, &file_stat))
      file_len = file_stat.st_size;
#elif defined(OS_ANDROID)
#  if __ANDROID_API__ >= 21
    struct stat64 file_stat;
    if (!fstat64(file_, &file_stat))
      file_len = file_stat.st_size;
#  else
    struct stat file_stat;
    if (!fstat(file_, &file_stat))
      file_len = file_stat.st_size;
#  endif
#else
    struct stat64 file_stat;
    if (!fstat64(file_, &file_stat))
      file_len = file_stat.st_size;
#endif
    if (file_len < 0) {
      DPLOG(ERROR) << "fstat " << file_;
      return false;
    }
    map_size = static_cast<size_t>(file_len);
    length_ = map_size;
  } else {
    // The region can be arbitrarily aligned. mmap, instead, requires both the
    // start and size to be page-aligned. Hence, we map here the page-aligned
    // outer region [|aligned_start|, |aligned_start| + |size|] which contains
    // |region| and then add up the |data_offset| displacement.
    int64_t aligned_start = 0;
    int64_t aligned_size = 0;
    CalculateVMAlignedBoundaries(region.offset,
                                 region.size,
                                 &aligned_start,
                                 &aligned_size,
                                 &data_offset);

    // Ensure that the casts in the mmap call below are sane.
    if (aligned_start < 0 || aligned_size < 0 ||
        aligned_start > std::numeric_limits<off_t>::max() ||
        static_cast<uint64_t>(aligned_size) >
            std::numeric_limits<size_t>::max() ||
        static_cast<uint64_t>(region.size) >
            std::numeric_limits<size_t>::max()) {
      DLOG(ERROR) << "Region bounds are not valid for mmap";
      return false;
    }

    map_start = static_cast<off_t>(aligned_start);
    map_size = static_cast<size_t>(aligned_size);
    length_ = static_cast<size_t>(region.size);
  }

  int flags = 0;
  switch (access) {
    case READ_ONLY:
      flags |= PROT_READ;
      break;

    case READ_WRITE:
      flags |= PROT_READ | PROT_WRITE;
      break;

    case READ_WRITE_EXTEND:
      flags |= PROT_READ | PROT_WRITE;

      const int64_t new_file_len = region.offset + region.size;

      // POSIX won't auto-extend the file when it is written so it must first
      // be explicitly extended to the maximum size. Zeros will fill the new
      // space. It is assumed that the existing file is fully realized as
      // otherwise the entire file would have to be read and possibly written.
      int64_t original_file_len = -1;
#if defined(OS_BSD) || defined(OS_MACOSX) || defined(OS_NACL)
      struct stat file_stat;
      if (!fstat(file_, &file_stat))
        original_file_len = file_stat.st_size;
#elif defined(OS_ANDROID)
#  if __ANDROID_API__ >= 21
      struct stat64 file_stat;
      if (!fstat64(file_, &file_stat))
        original_file_len = file_stat.st_size;
#  else
      struct stat file_stat;
      if (!fstat(file_, &file_stat))
        original_file_len = file_stat.st_size;
#  endif
#else
      struct stat64 file_stat;
      if (!fstat64(file_, &file_stat))
        original_file_len = file_stat.st_size;
#endif
      if (original_file_len < 0) {
        DPLOG(ERROR) << "fstat " << file_;
        return false;
      }

      // Increase the actual length of the file, if necessary. This can fail if
      // the disk is full and the OS doesn't support sparse files.
      if (HANDLE_EINTR(ftruncate(file_, std::max(original_file_len, new_file_len)))) {
        DPLOG(ERROR) << "ftruncate " << file_;
        return false;
      }

      // Realize the extent of the file so that it can't fail (and crash) later
      // when trying to write to a memory page that can't be created. This can
      // fail if the disk is full and the file is sparse.
      //
      // Only Android API>=21 supports the fallocate call. Older versions need
      // to manually extend the file by writing zeros at block intervals.
      //
      // Mac OSX doesn't support this call but the primary filesystem doesn't
      // support sparse files so is unneeded.
      bool do_manual_extension = false;

#if defined(OS_ANDROID) && __ANDROID_API__ < 21
      do_manual_extension = true;
#elif !defined(OS_MACOSX)
      if (posix_fallocate(file_, region.offset,
                          region.size) != 0) {
        DPLOG(ERROR) << "posix_fallocate " << file_;
        // This can fail because the filesystem doesn't support it so don't
        // give up just yet. Try the manual method below.
        do_manual_extension = true;
      }
#endif

      // Manually realize the extended file by writing bytes to it at intervals.
      if (do_manual_extension) {
        int64_t block_size = 512;  // Start with something safe.
        struct stat statbuf;
        if (fstat(file_, &statbuf) == 0 &&
            statbuf.st_blksize > 0) {
          block_size = statbuf.st_blksize;
        }

        // Write starting at the next block boundary after the old file length.
        const int64_t extension_start =
            (original_file_len + block_size - 1) & ~(block_size - 1);
        for (int64_t i = extension_start; i < new_file_len; i += block_size) {
          char existing_byte;
          if (pread(file_, &existing_byte, 1, i) != 1)
            return false;  // Can't read? Not viable.
          if (existing_byte != 0)
            continue;  // Block has data so must already exist.
          if (pwrite(file_, &existing_byte, 1, i) != 1)
            return false;  // Can't write? Not viable.
        }
      }

      break;
  }

  data_ = static_cast<uint8_t*>(mmap(NULL, map_size, flags, MAP_SHARED,
                                     file_, map_start));
  if (data_ == MAP_FAILED) {
    DPLOG(ERROR) << "mmap " << file_;
    return false;
  }

  data_ += data_offset;
  return true;
}
#endif

void MemoryMappedFile::CloseHandles() {
  if (data_ != NULL)
    munmap(data_, length_);
  close(file_);

  data_ = NULL;
  length_ = 0;
}

}  // namespace base
