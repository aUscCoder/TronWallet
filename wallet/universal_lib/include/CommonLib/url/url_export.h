// Author: Google
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef URL_URL_EXPORT_H_
#define URL_URL_EXPORT_H_
#pragma once

#include <cassert>
#include <string>

namespace base {
typedef wchar_t char16;
typedef std::wstring string16;
}

#define DCHECK assert
#define NOTREACHED() assert(0)

#define CHECK assert

#define OVERRIDE override

typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef __int8  int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

#endif  // URL_URL_EXPORT_H_
