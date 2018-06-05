// Copyright 2013 Sogou.Com Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "icu_utf.h"
#include "url_canon_internal.h"

namespace url_canon {

// Converts the Unicode input representing a hostname to ASCII using IDN rules.
// The output must be ASCII, but is represented as wide characters.
//
// On success, the output will be filled with the ASCII host name and it will
// return true. Unlike most other canonicalization functions, this assumes that
// the output is empty. The beginning of the host will be at offset 0, and
// the length of the output will be set to the length of the new host name.
//
// On error, this will return false. The output in this case is undefined.
// TODO(jungshik): use UTF-8/ASCII version of nameToASCII.
// Change the function signature and callers accordingly to avoid unnecessary
// conversions in our code. In addition, consider using icu::IDNA's UTF-8/ASCII
// version with StringByteSink. That way, we can avoid C wrappers and additional
// string conversion.
bool IDNToASCII(const base::char16* src, int src_len, CanonOutputW* output) {
  DCHECK(output->length() == 0);  // Output buffer is assumed empty.

  typedef int (WINAPI *IdnToAsciiProc)(DWORD dwFlags,
                                       LPCWSTR lpUnicodeCharStr,
                                       int cchUnicodeChar,
                                       LPWSTR lpASCIICharStr,
                                       int cchASCIIChar);
  static IdnToAsciiProc s_idnToAscii = NULL;
  if (s_idnToAscii == NULL) {
    HMODULE hModule = GetModuleHandle(L"kernel32");
    if (!hModule)
      hModule = ::LoadLibraryW(L"kernel32");
    if (hModule)
      s_idnToAscii = (IdnToAsciiProc)::GetProcAddress(hModule, "IdnToAscii");
    if (!s_idnToAscii) {
      hModule = LoadLibrary(L"Normaliz");
      if (hModule)
        s_idnToAscii = (IdnToAsciiProc)::GetProcAddress(hModule, "IdnToAscii");
    }
    if (!s_idnToAscii) {
      s_idnToAscii = (IdnToAsciiProc)- 1;
      return false;
    }
  }
  else if (s_idnToAscii == (IdnToAsciiProc)-1) {
    return false;
  }

  while (true) {
    int output_length = s_idnToAscii(0, src, src_len, NULL, 0);
    // Not enough room in our buffer, expand.
    if (output_length > output->capacity())
      output->Resize(output_length);

    // TODO(jungshik): Look at info.errors to handle them case-by-case basis
    // if necessary.
    output_length = s_idnToAscii(0, src, src_len, output->data(), output->capacity());
    if (output_length) {
      output->set_length(output_length);
      return true;
    }

    return false;  // Unknown error, give up.
  }
}

bool ReadUTFChar(const char* str, int* begin, int length,
                 unsigned* code_point_out) {
  int code_point;  // Avoids warning when U8_NEXT writes -1 to it.
  CBU8_NEXT(str, *begin, length, code_point);
  *code_point_out = static_cast<unsigned>(code_point);

  // The ICU macro above moves to the next char, we want to point to the last
  // char consumed.
  (*begin)--;

  // Validate the decoded value.
  if (CBU_IS_UNICODE_CHAR(code_point))
    return true;
  *code_point_out = kUnicodeReplacementCharacter;
  return false;
}

bool ReadUTFChar(const base::char16* str, int* begin, int length,
                 unsigned* code_point) {
  if (CBU16_IS_SURROGATE(str[*begin])) {
    if (!CBU16_IS_SURROGATE_LEAD(str[*begin]) || *begin + 1 >= length ||
        !CBU16_IS_TRAIL(str[*begin + 1])) {
      // Invalid surrogate pair.
      *code_point = kUnicodeReplacementCharacter;
      return false;
    } else {
      // Valid surrogate pair.
      *code_point = CBU16_GET_SUPPLEMENTARY(str[*begin], str[*begin + 1]);
      (*begin)++;
    }
  } else {
    // Not a surrogate, just one 16-bit word.
    *code_point = str[*begin];
  }

  if (CBU_IS_UNICODE_CHAR(*code_point))
    return true;

  // Invalid code point.
  *code_point = kUnicodeReplacementCharacter;
  return false;
}

}  // namespace url_canon
