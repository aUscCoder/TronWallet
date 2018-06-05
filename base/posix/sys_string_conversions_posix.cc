// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "../sys_string_conversions.h"

#include <cmath>

#include <string.h>
#include <stddef.h>
#include <wchar.h>

#include <iconv.h>

#include "../utf_string_conversions.h"
#include "../build_config.h"

namespace base {

namespace {

#define UTF8_SEQUENCE_MAXLEN 6

static int iconv_conv(iconv_t cd, char *out, int *outlen,
                      const char *in, int *inlen) {
  size_t icv_inlen, icv_outlen;
  const char *icv_in = (const char *)in;
  char *icv_out = (char *)out;
  int ret;

  if ((out == NULL) || (outlen == NULL) || (inlen == NULL) || (in == NULL)) {
    if (outlen != NULL)
      *outlen = 0;
    return -1;
  }

  icv_inlen = *inlen;
  icv_outlen = *outlen;
  ret = iconv(cd, (char**)&icv_in, &icv_inlen, &icv_out, &icv_outlen);
  *inlen -= icv_inlen;
  *outlen -= icv_outlen;
  if ((icv_inlen != 0) || (ret == -1)) {
#ifdef EILSEQ
    if (errno == EILSEQ) {
      return -2;
    } else
#endif
#ifdef E2BIG
    if (errno == E2BIG) {
      return -1;
    } else
#endif
#ifdef EINVAL
    if (errno == EINVAL) {
      return -3;
    } else
#endif
    {
      return -3;
    }
  }
  return 0;
}

}

std::string SysWideToGBK(const std::wstring& wide) {
  static thread_local iconv_t cvt_gbk_out = iconv_open("GB18030", "WCHAR_T");
  int src_byte_length = wide.size() * sizeof(wchar_t);
  int need_length = UTF8_SEQUENCE_MAXLEN * wide.size();
  std::string output;
  output.resize(need_length);
  output.reserve(need_length + 1);
  auto res = iconv_conv(cvt_gbk_out, &output[0], &need_length, (const char*)wide.c_str(), &src_byte_length);
  if (res == 0) {
    output.resize(need_length);
    return output;
  } else {
    return std::string();
  }
}

std::wstring SysGBKToWide(const std::string& gbk) {
  static thread_local iconv_t cvt_gbk_in = iconv_open("WCHAR_T", "GB18030");
  int need_length = gbk.size();
  std::wstring output;
  int src_len = gbk.length();
  output.resize(need_length);
  output.reserve(need_length + 1);
  need_length *= sizeof(wchar_t);
  auto res = iconv_conv(cvt_gbk_in, (char*)&output[0], &need_length, gbk.c_str(), (int*)&src_len);
  if (res == 0) {
    output.resize(std::ceil(need_length / sizeof(wchar_t)));
    return output;
  } else {
    return std::wstring();
  }
}

std::string SysWideToUTF8(const std::wstring& wide) {
  // In theory this should be using the system-provided conversion rather
  // than our ICU, but this will do for now.
  return WideToUTF8(wide);
}

std::wstring SysUTF8ToWide(const std::string& utf8) {
  // In theory this should be using the system-provided conversion rather
  // than our ICU, but this will do for now.
  std::wstring out;
  UTF8ToWide(utf8.data(), utf8.size(), &out);
  return out;
}

#if defined(SYSTEM_NATIVE_UTF8) || defined(OS_ANDROID)
// TODO(port): Consider reverting the OS_ANDROID when we have wcrtomb()
// support and a better understanding of what calls these routines.

std::string SysWideToNativeMB(const std::wstring& wide) {
  return WideToUTF8(wide);
}

std::wstring SysNativeMBToWide(const std::string& native_mb) {
  return SysUTF8ToWide(native_mb);
}

#else

std::string SysWideToNativeMB(const std::wstring& wide) {
  mbstate_t ps;

  // Calculate the number of multi-byte characters.  We walk through the string
  // without writing the output, counting the number of multi-byte characters.
  size_t num_out_chars = 0;
  memset(&ps, 0, sizeof(ps));
  for (size_t i = 0; i < wide.size(); ++i) {
    const wchar_t src = wide[i];
    // Use a temp buffer since calling wcrtomb with an output of NULL does not
    // calculate the output length.
    char buf[16];
    // Skip NULLs to avoid wcrtomb's special handling of them.
    size_t res = src ? wcrtomb(buf, src, &ps) : 0;
    switch (res) {
      // Handle any errors and return an empty string.
      case static_cast<size_t>(-1):
        return std::string();
        break;
      case 0:
        // We hit an embedded null byte, keep going.
        ++num_out_chars;
        break;
      default:
        num_out_chars += res;
        break;
    }
  }

  if (num_out_chars == 0)
    return std::string();

  std::string out;
  out.resize(num_out_chars);

  // We walk the input string again, with |i| tracking the index of the
  // wide input, and |j| tracking the multi-byte output.
  memset(&ps, 0, sizeof(ps));
  for (size_t i = 0, j = 0; i < wide.size(); ++i) {
    const wchar_t src = wide[i];
    // We don't want wcrtomb to do its funkiness for embedded NULLs.
    size_t res = src ? wcrtomb(&out[j], src, &ps) : 0;
    switch (res) {
      // Handle any errors and return an empty string.
      case static_cast<size_t>(-1):
        return std::string();
        break;
      case 0:
        // We hit an embedded null byte, keep going.
        ++j;  // Output is already zeroed.
        break;
      default:
        j += res;
        break;
    }
  }

  return out;
}

std::wstring SysNativeMBToWide(const std::string& native_mb) {
  mbstate_t ps;

  // Calculate the number of wide characters.  We walk through the string
  // without writing the output, counting the number of wide characters.
  size_t num_out_chars = 0;
  memset(&ps, 0, sizeof(ps));
  for (size_t i = 0; i < native_mb.size(); ) {
    const char* src = native_mb.data() + i;
    size_t res = mbrtowc(NULL, src, native_mb.size() - i, &ps);
    switch (res) {
      // Handle any errors and return an empty string.
      case static_cast<size_t>(-2):
      case static_cast<size_t>(-1):
        return std::wstring();
        break;
      case 0:
        // We hit an embedded null byte, keep going.
        i += 1;  // Fall through.
      default:
        i += res;
        ++num_out_chars;
        break;
    }
  }

  if (num_out_chars == 0)
    return std::wstring();

  std::wstring out;
  out.resize(num_out_chars);

  memset(&ps, 0, sizeof(ps));  // Clear the shift state.
  // We walk the input string again, with |i| tracking the index of the
  // multi-byte input, and |j| tracking the wide output.
  for (size_t i = 0, j = 0; i < native_mb.size(); ++j) {
    const char* src = native_mb.data() + i;
    wchar_t* dst = &out[j];
    size_t res = mbrtowc(dst, src, native_mb.size() - i, &ps);
    switch (res) {
      // Handle any errors and return an empty string.
      case static_cast<size_t>(-2):
      case static_cast<size_t>(-1):
        return std::wstring();
        break;
      case 0:
        i += 1;  // Skip null byte.
        break;
      default:
        i += res;
        break;
    }
  }

  return out;
}

#endif  // OS_CHROMEOS

}  // namespace base
