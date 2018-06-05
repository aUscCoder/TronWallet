// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory_mapped_file.h"

#include <limits>
#include <utility>

#include "base/logging.h"
#include "base/build_config.h"

#if defined(OS_POSIX)
#include <unistd.h>
#endif // OS_POSIX

namespace base {

const MemoryMappedFile::Region MemoryMappedFile::Region::kWholeFile = {0, 0};

bool MemoryMappedFile::Region::operator==(
    const MemoryMappedFile::Region& other) const {
  return other.offset == offset && other.size == size;
}

bool MemoryMappedFile::Region::operator!=(
    const MemoryMappedFile::Region& other) const {
  return other.offset != offset || other.size != size;
}

MemoryMappedFile::~MemoryMappedFile() {
  CloseHandles();
}

#if !defined(OS_NACL)
bool MemoryMappedFile::Initialize(const PathString& file_path,
                                  const Region& region,
                                  Access access) {
  switch (access) {
    case READ_WRITE_EXTEND:
      // Can't open with "extend" because no maximum size is known.
      DCHECK(Region::kWholeFile != region);
      // Ensure that the extended size is within limits of File.
      if (region.size > std::numeric_limits<int64_t>::max() - region.offset) {
        DLOG(ERROR) << "Region bounds exceed maximum for base::File.";
        return false;
      }
      // Fall through.
    case READ_ONLY:
    case READ_WRITE:
      // Ensure that the region values are valid.
      if (region.offset < 0 || region.size < 0) {
        DLOG(ERROR) << "Region bounds are not valid.";
        return false;
      }
      break;
  }

  if (IsValid())
    return false;

  if (region != Region::kWholeFile) {
    DCHECK_GE(region.offset, 0);
    DCHECK_GT(region.size, 0);
  }

  file_path_ = file_path;
  if (!MapFileRegionToMemory(region, access)) {
    CloseHandles();
    return false;
  }

  return true;
}

bool MemoryMappedFile::IsValid() const {
  return data_ != NULL;
}

// static
void MemoryMappedFile::CalculateVMAlignedBoundaries(int64_t start,
                                                    int64_t size,
                                                    int64_t* aligned_start,
                                                    int64_t* aligned_size,
                                                    int32_t* offset) {
#if defined(OS_WIN)
  // Sadly, on Windows, the mmap alignment is not just equal to the page size.
  SYSTEM_INFO system_info = {};
  ::GetNativeSystemInfo(&system_info);
  const int64_t mask =
      static_cast<int64_t>(system_info.dwAllocationGranularity) - 1;
#elif defined(OS_POSIX)
  const int64_t mask = getpagesize();
#endif
  DCHECK_LT(mask, std::numeric_limits<int32_t>::max());
  *offset = static_cast<int32_t>(start & mask);
  *aligned_start = start & ~mask;
  *aligned_size = (size + *offset + mask) & ~mask;
}
#endif

}  // namespace base
