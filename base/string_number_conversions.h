// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRINGS_STRING_NUMBER_CONVERSIONS_H_
#define BASE_STRINGS_STRING_NUMBER_CONVERSIONS_H_

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// IMPORTANT MESSAGE FROM YOUR SPONSOR
//
// This file contains no "wstring" variants. New code should use std::u16string. If
// you need to make old code work, use the UTF8 version and convert. Please do
// not add wstring variants.
//
// Please do not add "convenience" functions for converting strings to integers
// that return the value and ignore success/failure. That encourages people to
// write code that doesn't properly handle the error conditions.
//
// DO NOT use these functions in any UI unless it's NOT localized on purpose.
// Instead, use base::MessageFormatter for a complex message with numbers
// (integer, float, double) embedded or base::Format{Number,Double,Percent} to
// just format a single number/percent. Note that some languages use native
// digits instead of ASCII digits while others use a group separator or decimal
// point different from ',' and '.'. Using these functions in the UI would lead
// numbers to be formatted in a non-native way.
// ----------------------------------------------------------------------------

namespace base {

// Number -> string conversions ------------------------------------------------

std::string IntToString(int value);
std::u16string IntToString16(int value);

std::string UintToString(unsigned value);
std::u16string UintToString16(unsigned value);

std::string Int64ToString(int64_t value);
std::u16string Int64ToString16(int64_t value);

std::string Uint64ToString(uint64_t value);
std::u16string Uint64ToString16(uint64_t value);

std::string SizeTToString(size_t value);
std::u16string SizeTToString16(size_t value);

// DoubleToString converts the double to a string format that ignores the
// locale. If you want to use locale specific formatting, use ICU.
std::string DoubleToString(double value);

// String -> number conversions ------------------------------------------------

// Perform a best-effort conversion of the input string to a numeric type,
// setting |*output| to the result of the conversion.  Returns true for
// "perfect" conversions; returns false in the following cases:
//  - Overflow. |*output| will be set to the maximum value supported
//    by the data type.
//  - Underflow. |*output| will be set to the minimum value supported
//    by the data type.
//  - Trailing characters in the string after parsing the number.  |*output|
//    will be set to the value of the number that was parsed.
//  - Leading whitespace in the string before parsing the number. |*output| will
//    be set to the value of the number that was parsed.
//  - No characters parseable as a number at the beginning of the string.
//    |*output| will be set to 0.
//  - Empty string.  |*output| will be set to 0.
// WARNING: Will write to |output| even when returning false.
//          Read the comments above carefully.
bool StringToInt(const std::string& input, int* output);
bool StringToInt(const std::u16string& input, int* output);

bool StringToUint(const std::string& input, unsigned* output);
bool StringToUint(const std::u16string& input, unsigned* output);

bool StringToInt64(const std::string& input, int64_t* output);
bool StringToInt64(const std::u16string& input, int64_t* output);

bool StringToUint64(const std::string& input, uint64_t* output);
bool StringToUint64(const std::u16string& input, uint64_t* output);

bool StringToSizeT(const std::string& input, size_t* output);
bool StringToSizeT(const std::u16string& input, size_t* output);

// For floating-point conversions, only conversions of input strings in decimal
// form are defined to work.  Behavior with strings representing floating-point
// numbers in hexadecimal, and strings representing non-finite values (such as
// NaN and inf) is undefined.  Otherwise, these behave the same as the integral
// variants.  This expects the input string to NOT be specific to the locale.
// If your input is locale specific, use ICU to read the number.
// WARNING: Will write to |output| even when returning false.
//          Read the comments here and above StringToInt() carefully.
bool StringToDouble(const std::string& input, double* output);

// Hex encoding ----------------------------------------------------------------

// Returns a hex string representation of a binary buffer. The returned hex
// string will be in upper case. This function does not check if |size| is
// within reasonable limits since it's written with trusted data in mind.  If
// you suspect that the data you want to format might be large, the absolute
// max size for |size| should be is
//   std::numeric_limits<size_t>::max() / 2
std::string HexEncode(const void* bytes, size_t size);

// Best effort conversion, see StringToInt above for restrictions.
// Will only successful parse hex values that will fit into |output|, i.e.
// -0x80000000 < |input| < 0x7FFFFFFF.
bool HexStringToInt(const std::string& input, int* output);

// Best effort conversion, see StringToInt above for restrictions.
// Will only successful parse hex values that will fit into |output|, i.e.
// 0x00000000 < |input| < 0xFFFFFFFF.
// The string is not required to start with 0x.
bool HexStringToUInt(const std::string& input, uint32_t* output);

// Best effort conversion, see StringToInt above for restrictions.
// Will only successful parse hex values that will fit into |output|, i.e.
// -0x8000000000000000 < |input| < 0x7FFFFFFFFFFFFFFF.
bool HexStringToInt64(const std::string& input, int64_t* output);

// Best effort conversion, see StringToInt above for restrictions.
// Will only successful parse hex values that will fit into |output|, i.e.
// 0x0000000000000000 < |input| < 0xFFFFFFFFFFFFFFFF.
// The string is not required to start with 0x.
bool HexStringToUInt64(const std::string& input, uint64_t* output);

// Similar to the previous functions, except that output is a vector of bytes.
// |*output| will contain as many bytes as were successfully parsed prior to the
// error.  There is no overflow, but input.size() must be evenly divisible by 2.
// Leading 0x or +/- are not allowed.
bool HexStringToBytes(const std::string& input,
                                  std::vector<uint8_t>* output);

}  // namespace base

#endif  // BASE_STRINGS_STRING_NUMBER_CONVERSIONS_H_