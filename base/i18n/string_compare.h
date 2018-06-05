// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_I18N_STRING_COMPARE_H_
#define BASE_I18N_STRING_COMPARE_H_

#include <algorithm>
#include <string>
#include <vector>

#include "unicode/ustring.h"
#include "unicode/coll.h"

namespace base {
namespace i18n {

// Compares the two strings using the specified collator.
int CompareString16(const std::u16string& lhs,
                    const std::u16string& rhs,
                    bool case_insensitive = false);



// Compares the two strings using the specified collator.
UCollationResult
CompareString16WithCollator(const icu::Collator& collator,
                            const std::u16string& lhs,
                            const std::u16string& rhs);

}  // namespace i18n
}  // namespace base

#endif  // BASE_I18N_STRING_COMPARE_H_