// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "string_escape.h"

#include <stddef.h>
#include <stdint.h>

#include <limits>
#include <string>

#include "icu_utf.h"
#include "logging.h"
#include "stringprintf.h"
#include "utf_string_conversion_utils.h"

namespace base {

namespace {

// Format string for printing a \uXXXX escape sequence.
const char kU16EscapeFormat[] = "\\u%04X";

// The code point to output for an invalid input code unit.
const uint32_t kReplacementCodePoint = 0xFFFD;

// Used below in EscapeSpecialCodePoint().
static_assert('<' == 0x3C, "less than sign must be 0x3c");

// Try to escape the |code_point| if it is a known special character. If
// successful, returns true and appends the escape sequence to |dest|. This
// isn't required by the spec, but it's more readable by humans.
bool EscapeSpecialCodePoint(uint32_t code_point, std::string* dest) {
  // WARNING: if you add a new case here, you need to update the reader as well.
  // Note: \v is in the reader, but not here since the JSON spec doesn't
  // allow it.
  switch (code_point) {
    case '\b':
      dest->append("\\b");
      break;
    case '\f':
      dest->append("\\f");
      break;
    case '\n':
      dest->append("\\n");
      break;
    case '\r':
      dest->append("\\r");
      break;
    case '\t':
      dest->append("\\t");
      break;
    case '\\':
      dest->append("\\\\");
      break;
    case '"':
      dest->append("\\\"");
      break;
    // Escape < to prevent script execution; escaping > is not necessary and
    // not doing so save a few bytes.
    case '<':
      dest->append("\\u003C");
      break;
    // Escape the "Line Separator" and "Paragraph Separator" characters, since
    // they should be treated like a new line \r or \n.
    case 0x2028:
      dest->append("\\u2028");
      break;
    case 0x2029:
      dest->append("\\u2029");
      break;
    default:
      return false;
  }
  return true;
}

template <typename S>
bool EscapeJSONStringImpl(const S& str, bool put_in_quotes, std::string* dest) {
  bool did_replacement = false;

  if (put_in_quotes)
    dest->push_back('"');

  // Casting is necessary because ICU uses int32_t. Try and do so safely.
  CHECK_LE(str.length(),
           static_cast<size_t>(std::numeric_limits<int32_t>::max()));
  const int32_t length = static_cast<int32_t>(str.length());

  for (int32_t i = 0; i < length; ++i) {
    uint32_t code_point;
    if (!ReadUnicodeCharacter(str.data(), length, &i, &code_point)) {
      code_point = kReplacementCodePoint;
      did_replacement = true;
    }

    if (EscapeSpecialCodePoint(code_point, dest))
      continue;

    // Escape non-printing characters.
    if (code_point < 32) {
      base::StringAppendF(dest, kU16EscapeFormat, code_point);
    } else {
      if (code_point <= 0x7f) {
        // Fast path the common case of one byte.
        dest->push_back(static_cast<char>(code_point));
      } else {
        // CBU8_APPEND_UNSAFE can append up to 4 bytes.
        size_t char_offset = dest->length();
        dest->resize(char_offset + CBU8_MAX_LENGTH);

        CBU8_APPEND_UNSAFE(&(*dest)[0], char_offset, code_point);

        // CBU8_APPEND_UNSAFE will advance our pointer past the inserted character, so
        // it will represent the new length of the string.
        dest->resize(char_offset);
      }
    }
  }

  if (put_in_quotes)
    dest->push_back('"');

  return !did_replacement;
}

}  // namespace

bool EscapeJSONString(const std::string& str,
                      bool put_in_quotes,
                      std::string* dest) {
  return EscapeJSONStringImpl(str, put_in_quotes, dest);
}

bool EscapeJSONString(const std::wstring& str,
                      bool put_in_quotes,
                      std::string* dest) {
  return EscapeJSONStringImpl(str, put_in_quotes, dest);
}

std::string GetQuotedJSONString(const std::string& str) {
  std::string dest;
  bool ok = EscapeJSONStringImpl(str, true, &dest);
  DCHECK(ok);
  return dest;
}

std::string GetQuotedJSONString(const std::wstring& str) {
  std::string dest;
  bool ok = EscapeJSONStringImpl(str, true, &dest);
  DCHECK(ok);
  return dest;
}

std::string EscapeBytesAsInvalidJSONString(const std::string& str,
                                           bool put_in_quotes) {
  std::string dest;

  if (put_in_quotes)
    dest.push_back('"');

  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
    unsigned char c = *it;
    if (EscapeSpecialCodePoint(c, &dest))
      continue;

    if (c < 32 || c > 126) {
      base::StringAppendF(&dest, kU16EscapeFormat, c);
    } else {
      dest.push_back(*it);
    }
  }

  if (put_in_quotes)
    dest.push_back('"');

  return dest;
}

}  // namespace base
