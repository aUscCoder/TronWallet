// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRINGS_STRINGPRINTF_H_
#define BASE_STRINGS_STRINGPRINTF_H_

#include <stdarg.h>   // va_list

#include <string>

#include "build_config.h"

namespace base {

// Tell the compiler a function is using a printf-style format string.
// |format_param| is the one-based index of the format string parameter;
// |dots_param| is the one-based index of the "..." parameter.
// For v*printf functions (which take a va_list), pass 0 for dots_param.
// (This is undocumented but matches what the system C headers do.)
#if defined(COMPILER_GCC)
#define PRINTF_FORMAT(format_param, dots_param) \
    __attribute__((format(printf, format_param, dots_param)))
#else
#define PRINTF_FORMAT(format_param, dots_param)
#endif

#if !defined(COMPILER_MSVC)
#define _Printf_format_string_
#endif 

// WPRINTF_FORMAT is the same, but for wide format strings.
// This doesn't appear to yet be implemented in any compiler.
// See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=38308 .
#define WPRINTF_FORMAT(format_param, dots_param)
// If available, it would look like:
//   __attribute__((format(wprintf, format_param, dots_param)))

// Return a C++ string given printf-like input.
std::string StringPrintf(_Printf_format_string_ const char* format,
                                     ...)
    PRINTF_FORMAT(1, 2);
#if defined(OS_WIN)
std::wstring StringPrintf(
    _Printf_format_string_ const wchar_t* format,
    ...) WPRINTF_FORMAT(1, 2);
#endif

// Return a C++ string given vprintf-like input.
std::string StringPrintV(const char* format, va_list ap)
    PRINTF_FORMAT(1, 0);

// Store result into a supplied string and return it.
const std::string& SStringPrintf(
    std::string* dst,
    _Printf_format_string_ const char* format,
    ...) PRINTF_FORMAT(2, 3);
#if defined(OS_WIN)
const std::wstring& SStringPrintf(
    std::wstring* dst,
    _Printf_format_string_ const wchar_t* format,
    ...) WPRINTF_FORMAT(2, 3);
#endif

// Append result to a supplied string.
void StringAppendF(std::string* dst,
                               _Printf_format_string_ const char* format,
                               ...) PRINTF_FORMAT(2, 3);
#if defined(OS_WIN)
void StringAppendF(std::wstring* dst,
                               _Printf_format_string_ const wchar_t* format,
                               ...) WPRINTF_FORMAT(2, 3);
#endif

// Lower-level routine that takes a va_list and appends to a specified
// string.  All other routines are just convenience wrappers around it.
void StringAppendV(std::string* dst, const char* format, va_list ap)
    PRINTF_FORMAT(2, 0);
#if defined(OS_WIN)
void StringAppendV(std::wstring* dst,
                               const wchar_t* format, va_list ap)
    WPRINTF_FORMAT(2, 0);
#endif

}  // namespace base

#endif  // BASE_STRINGS_STRINGPRINTF_H_
