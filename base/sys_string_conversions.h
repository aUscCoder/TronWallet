// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRINGS_SYS_STRING_CONVERSIONS_H_
#define BASE_STRINGS_SYS_STRING_CONVERSIONS_H_

// Provides system-dependent string type conversions for cases where it's
// necessary to not use ICU. Generally, you should not need this in Chrome,
// but it is used in some shared code. Dependencies should be minimal.

#include <stdint.h>

#include <string>

#include "build_config.h"
#include "utf_string_conversions.h"

#if defined(OS_MACOSX)
#include <CoreFoundation/CoreFoundation.h>
#ifdef __OBJC__
@class NSString;
#else
class NSString;
#endif
#endif  // OS_MACOSX

namespace base {

// Converts between wide and UTF-8 representations of a string. On error, the
// result is system-dependent.
std::string SysWideToUTF8(const std::wstring& wide);
std::wstring SysUTF8ToWide(const std::string& utf8);

// Converts between wide and the system multi-byte representations of a string.
// DANGER: This will lose information and can change (on Windows, this can
// change between reboots).
std::string SysWideToNativeMB(const std::wstring& wide);
std::wstring SysNativeMBToWide(const std::string& native_mb);

std::string SysWideToGBK(const std::wstring& wide);
std::wstring SysGBKToWide(const std::string& gbk);

inline std::string SysUTF16ToGBK(const std::u16string& utf16) {
  return SysWideToGBK(UTF16ToWide(utf16));
}

inline std::u16string SysGBKToUTF16(const std::string& gbk) {
  return WideToUTF16(SysGBKToWide(gbk));
}

inline std::string SysGBKToUTF8(const std::string& gbk) {
  return SysWideToUTF8(SysGBKToWide(gbk));
}

inline std::string SysUTF8ToGBK(const std::string& utf8) {
  return SysWideToGBK(SysUTF8ToWide(utf8));
}

inline std::u16string SysGBKToUTF16le(const std::string& gbk) {
  return UTF16Swap(SysGBKToUTF16(gbk));
}

inline std::string SysUTF16leToGBK(const std::u16string& utf16) {
  return SysUTF16ToGBK(UTF16Swap(utf16));
}

// Windows-specific ------------------------------------------------------------

#if defined(OS_WIN)

// Converts between 8-bit and wide strings, using the given code page. The
// code page identifier is one accepted by the Windows function
// MultiByteToWideChar().
std::wstring SysMultiByteToWide(const std::string& mb,
                                            uint32_t code_page);
std::string SysWideToMultiByte(const std::wstring& wide,
                                           uint32_t code_page);

#endif  // defined(OS_WIN)

// Mac-specific ----------------------------------------------------------------

#if defined(OS_MACOSX)

// Converts between STL strings and CFStringRefs/NSStrings.

// Creates a string, and returns it with a refcount of 1. You are responsible
// for releasing it. Returns NULL on failure.
CFStringRef SysUTF8ToCFStringRef(const std::string& utf8);
CFStringRef SysUTF16ToCFStringRef(const std::u16string& utf16);

// Same, but returns an autoreleased NSString.
NSString* SysUTF8ToNSString(const std::string& utf8);
NSString* SysUTF16ToNSString(const std::u16string& utf16);

// Converts a CFStringRef to an STL string. Returns an empty string on failure.
std::string SysCFStringRefToUTF8(CFStringRef ref);
std::u16string SysCFStringRefToUTF16(CFStringRef ref);

// Same, but accepts NSString input. Converts nil NSString* to the appropriate
// string type of length 0.
std::string SysNSStringToUTF8(NSString* ref);
std::u16string SysNSStringToUTF16(NSString* ref);

#endif  // defined(OS_MACOSX)

}  // namespace base

#define GBKSTR(x) base::SysUTF8ToGBK(u8##x)

#endif  // BASE_STRINGS_SYS_STRING_CONVERSIONS_H_
