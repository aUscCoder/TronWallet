// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef URL_URL_UTIL_INTERNAL_H_
#define URL_URL_UTIL_INTERNAL_H_

#include <string>

#include "url_parse.h"

namespace url_util {

extern const char kFileScheme[];
extern const char kFileSystemScheme[];
extern const char kMailtoScheme[];

// Given a string and a range inside the string, compares it to the given
// lower-case |compare_to| buffer.
bool CompareSchemeComponent(const char* spec,
                            const url_parse::Component& component,
                            const char* compare_to);
bool CompareSchemeComponent(const base::char16* spec,
                            const url_parse::Component& component,
                            const char* compare_to);

}  // namespace url_util

#endif  // URL_URL_UTIL_INTERNAL_H_
