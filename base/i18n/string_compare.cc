// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/i18n/string_compare.h"

#include "base/logging.h"
#include "base/utf_string_conversions.h"
#include "unicode/unistr.h"

namespace base {
namespace i18n {

// Compares the character data stored in two different string16 strings by
// specified Collator instance.
int CompareString16(const std::u16string& lhs,
                    const std::u16string& rhs,
                    bool case_insensitive) {
  UErrorCode error = U_ZERO_ERROR;
  if (case_insensitive) {
    int result = u_strCaseCompare(lhs.c_str(), static_cast<int>(lhs.length()),
                                  rhs.c_str(), static_cast<int>(rhs.length()), 0, &error);
    DCHECK(U_SUCCESS(error));
    return result;
  } else {
    return u_strCompare(lhs.c_str(), static_cast<int>(lhs.length()),
                        rhs.c_str(), static_cast<int>(rhs.length()), false);
  }
}

// Compares the character data stored in two different string16 strings by
// specified Collator instance.
UCollationResult CompareString16WithCollator(const icu::Collator& collator,
                                             const std::u16string& lhs,
                                             const std::u16string& rhs) {
  UErrorCode error = U_ZERO_ERROR;
  UCollationResult result = collator.compare(
      icu::UnicodeString(FALSE, lhs.c_str(), static_cast<int>(lhs.length())),
      icu::UnicodeString(FALSE, rhs.c_str(), static_cast<int>(rhs.length())),
      error);
  DCHECK(U_SUCCESS(error));
  return result;
}

}  // namespace i18n
}  // namespace base