// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utf_string_conversions.h"

#include <stdint.h>

#include "macros.h"
#include "utf_string_conversion_utils.h"
#include "build_config.h"
#include "logging.h"

namespace base {

namespace {

// Generalized Unicode converter -----------------------------------------------

// Converts the given source Unicode character type to the given destination
// Unicode character type as a STL string. The given input buffer and size
// determine the source, and the given output STL string will be replaced by
// the result.
template<typename SRC_CHAR, typename DEST_STRING>
bool ConvertUnicode(const SRC_CHAR* src,
                    size_t src_len,
                    DEST_STRING* output) {
  // ICU requires 32-bit numbers.
  bool success = true;
  int32_t src_len32 = static_cast<int32_t>(src_len);
  for (int32_t i = 0; i < src_len32; i++) {
    uint32_t code_point;
    if (ReadUnicodeCharacter(src, src_len32, &i, &code_point)) {
      WriteUnicodeCharacter(code_point, output);
    } else {
      WriteUnicodeCharacter(0xFFFD, output);
      success = false;
    }
  }

  return success;
}

}  // namespace

// UTF-8 <-> Wide --------------------------------------------------------------

bool WideToUTF8(const wchar_t* src, size_t src_len, std::string* output) {
  if (IsStringASCII(std::wstring(src, src_len))) {
    output->assign(src, src + src_len);
    return true;
  } else {
    PrepareForUTF8Output(src, src_len, output);
    return ConvertUnicode(src, src_len, output);
  }
}

std::string WideToUTF8(const std::wstring& wide) {
  if (IsStringASCII(wide)) {
    return std::string(wide.data(), wide.data() + wide.length());
  }

  std::string ret;
  PrepareForUTF8Output(wide.data(), wide.length(), &ret);
  ConvertUnicode(wide.data(), wide.length(), &ret);
  return ret;
}

bool UTF8ToWide(const char* src, size_t src_len, std::wstring* output) {
  if (IsStringASCII(std::string(src, src_len))) {
    output->assign(src, src + src_len);
    return true;
  } else {
    PrepareForUTF16Or32Output(src, src_len, output);
    return ConvertUnicode(src, src_len, output);
  }
}

std::wstring UTF8ToWide(const std::string& utf8) {
  if (IsStringASCII(utf8)) {
    return std::wstring(utf8.begin(), utf8.end());
  }

  std::wstring ret;
  PrepareForUTF16Or32Output(utf8.data(), utf8.length(), &ret);
  ConvertUnicode(utf8.data(), utf8.length(), &ret);
  return ret;
}

// UTF-16 <-> Wide -------------------------------------------------------------

#if defined(WCHAR_T_IS_UTF16)

// When wide == UTF-16, then conversions are a NOP.
bool WideToUTF16(const wchar_t* src, size_t src_len, std::u16string* output) {
  output->assign((char16_t*)src, src_len);
  return true;
}

std::u16string WideToUTF16(const std::wstring& wide) {
  return std::u16string((char16_t*)wide.c_str(), wide.size());
}

bool UTF16ToWide(const char16_t* src, size_t src_len, std::wstring* output) {
  output->assign((wchar_t*)src, src_len);
  return true;
}

std::wstring UTF16ToWide(const std::u16string& utf16) {
  return std::wstring((wchar_t*)utf16.c_str(), utf16.size());
}

#elif defined(WCHAR_T_IS_UTF32)

bool WideToUTF16(const wchar_t* src, size_t src_len, std::u16string* output) {
  output->clear();
  // Assume that normally we won't have any non-BMP characters so the counts
  // will be the same.
  output->reserve(src_len);
  return ConvertUnicode(src, src_len, output);
}

std::u16string WideToUTF16(const std::wstring& wide) {
  std::u16string ret;
  WideToUTF16(wide.data(), wide.length(), &ret);
  return ret;
}

bool UTF16ToWide(const char16_t* src, size_t src_len, std::wstring* output) {
  output->clear();
  // Assume that normally we won't have any non-BMP characters so the counts
  // will be the same.
  output->reserve(src_len);
  return ConvertUnicode(src, src_len, output);
}

std::wstring UTF16ToWide(const std::u16string& utf16) {
  std::wstring ret;
  UTF16ToWide(utf16.data(), utf16.length(), &ret);
  return ret;
}

#endif  // defined(WCHAR_T_IS_UTF32)

// UTF16 <-> UTF8 --------------------------------------------------------------

bool UTF8ToUTF16(const char* src, size_t src_len, std::u16string* output) {
  if (IsStringASCII(std::string(src, src_len))) {
    output->assign(src, src + src_len);
    return true;
  } else {
    PrepareForUTF16Or32Output(src, src_len, output);
    return ConvertUnicode(src, src_len, output);
  }
}

std::u16string UTF8ToUTF16(const std::string& utf8) {
  if (IsStringASCII(utf8)) {
    return std::u16string(utf8.begin(), utf8.end());
  }

  std::u16string ret;
  PrepareForUTF16Or32Output(utf8.data(), utf8.length(), &ret);
  // Ignore the success flag of this call, it will do the best it can for
  // invalid input, which is what we want here.
  ConvertUnicode(utf8.data(), utf8.length(), &ret);
  return ret;
}

bool UTF16ToUTF8(const char16_t* src, size_t src_len, std::string* output) {
  if (IsStringASCII(std::u16string(src, src_len))) {
    output->assign(src, src + src_len);
    return true;
  } else {
    PrepareForUTF8Output(src, src_len, output);
    return ConvertUnicode(src, src_len, output);
  }
}

std::string UTF16ToUTF8(const std::u16string& utf16) {
  if (IsStringASCII(utf16)) {
    return std::string(utf16.begin(), utf16.end());
  }

  std::string ret;
  // Ignore the success flag of this call, it will do the best it can for
  // invalid input, which is what we want here.
  UTF16ToUTF8(utf16.data(), utf16.length(), &ret);
  return ret;
}

std::u16string ASCIIToUTF16(const std::string& ascii) {
  DCHECK(IsStringASCII(ascii)) << ascii;
  return std::u16string(ascii.begin(), ascii.end());
}

std::string UTF16ToASCII(const std::u16string& utf16) {
  DCHECK(IsStringASCII(utf16)) << UTF16ToUTF8(utf16);
  return std::string(utf16.begin(), utf16.end());
}

std::u16string UTF16Swap(const std::u16string& utf16)
{
  std::u16string other;
  other.resize(utf16.size());
  for (size_t i = 0; i < utf16.size(); i++)
    other[i] = (utf16[i] >> 8) | (utf16[i] << 8);
  return other;
}

}  // namespace base
