// Copyright (c) 2017 The Typany Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_I18N_LOCALE_INFO_H_
#define BASE_I18N_LOCALE_INFO_H_

#include <algorithm>
#include <string>
#include <vector>

#include "unicode/ustring.h"

namespace base {
namespace i18n {

// Return exemplar set for current locale set.
std::vector<uint16_t> GetLocaleExemplarSet(const std::string& locale_name);

}  // namespace i18n
}  // namespace base

#endif  // BASE_I18N_LOCALE_INFO_H_