// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "../sys_string_conversions.h"

#import <Foundation/Foundation.h>
#include <stddef.h>

#include <vector>

#include "scoped_cftyperef.h"

namespace base {

namespace {

// CFTypeRefToNSObjectAutorelease transfers ownership of a Core Foundation
// object (one derived from CFTypeRef) to the Foundation memory management
// system.  In a traditional managed-memory environment, cf_object is
// autoreleased and returned as an NSObject.  In a garbage-collected
// environment, cf_object is marked as eligible for garbage collection.
//
// This function should only be used to convert a concrete CFTypeRef type to
// its equivalent "toll-free bridged" NSObject subclass, for example,
// converting a CFStringRef to NSString.
//
// By calling this function, callers relinquish any ownership claim to
// cf_object.  In a managed-memory environment, the object's ownership will be
// managed by the innermost NSAutoreleasePool, so after this function returns,
// callers should not assume that cf_object is valid any longer than the
// returned NSObject.
//
// Returns an id, typed here for C++'s sake as a void*.
void* CFTypeRefToNSObjectAutorelease(CFTypeRef cf_object) {
  // When GC is on, NSMakeCollectable marks cf_object for GC and autorelease
  // is a no-op.
  //
  // In the traditional GC-less environment, NSMakeCollectable is a no-op,
  // and cf_object is autoreleased, balancing out the caller's ownership claim.
  //
  // NSMakeCollectable returns nil when used on a NULL object.
  return [NSMakeCollectable(cf_object) autorelease];
}

// Convert the supplied CFString into the specified encoding, and return it as
// an STL string of the template type.  Returns an empty string on failure.
//
// Do not assert in this function since it is used by the asssertion code!
template<typename StringType>
static StringType CFStringToSTLStringWithEncodingT(CFStringRef cfstring,
                                                   CFStringEncoding encoding) {
  CFIndex length = CFStringGetLength(cfstring);
  if (length == 0)
    return StringType();

  CFRange whole_string = CFRangeMake(0, length);
  CFIndex out_size;
  CFIndex converted = CFStringGetBytes(cfstring,
                                       whole_string,
                                       encoding,
                                       0,      // lossByte
                                       false,  // isExternalRepresentation
                                       NULL,   // buffer
                                       0,      // maxBufLen
                                       &out_size);
  if (converted == 0 || out_size == 0)
    return StringType();

  // out_size is the number of UInt8-sized units needed in the destination.
  // A buffer allocated as UInt8 units might not be properly aligned to
  // contain elements of StringType::value_type.  Use a container for the
  // proper value_type, and convert out_size by figuring the number of
  // value_type elements per UInt8.  Leave room for a NUL terminator.
  typename StringType::size_type elements =
      out_size * sizeof(UInt8) / sizeof(typename StringType::value_type) + 1;

  std::vector<typename StringType::value_type> out_buffer(elements);
  converted = CFStringGetBytes(cfstring,
                               whole_string,
                               encoding,
                               0,      // lossByte
                               false,  // isExternalRepresentation
                               reinterpret_cast<UInt8*>(&out_buffer[0]),
                               out_size,
                               NULL);  // usedBufLen
  if (converted == 0)
    return StringType();

  out_buffer[elements - 1] = '\0';
  return StringType(&out_buffer[0], elements - 1);
}

// Given an STL string |in| with an encoding specified by |in_encoding|,
// convert it to |out_encoding| and return it as an STL string of the
// |OutStringType| template type.  Returns an empty string on failure.
//
// Do not assert in this function since it is used by the asssertion code!
template<typename InStringType, typename OutStringType>
static OutStringType STLStringToSTLStringWithEncodingsT(
    const InStringType& in,
    CFStringEncoding in_encoding,
    CFStringEncoding out_encoding) {
  typename InStringType::size_type in_length = in.length();
  if (in_length == 0)
    return OutStringType();

  base::ScopedCFTypeRef<CFStringRef> cfstring(CFStringCreateWithBytesNoCopy(
      NULL,
      reinterpret_cast<const UInt8*>(in.data()),
      in_length * sizeof(typename InStringType::value_type),
      in_encoding,
      false,
      kCFAllocatorNull));
  if (!cfstring)
    return OutStringType();

  return CFStringToSTLStringWithEncodingT<OutStringType>(cfstring,
                                                         out_encoding);
}

// Given an STL string |in| with an encoding specified by |in_encoding|,
// return it as a CFStringRef.  Returns NULL on failure.
template<typename StringType>
static CFStringRef STLStringToCFStringWithEncodingsT(
    const StringType& in,
    CFStringEncoding in_encoding) {
  typename StringType::size_type in_length = in.length();
  if (in_length == 0)
    return CFSTR("");

  return CFStringCreateWithBytes(kCFAllocatorDefault,
                                 reinterpret_cast<const UInt8*>(in.data()),
                                 in_length *
                                   sizeof(typename StringType::value_type),
                                 in_encoding,
                                 false);
}

// Specify the byte ordering explicitly, otherwise CFString will be confused
// when strings don't carry BOMs, as they typically won't.
static const CFStringEncoding kNarrowStringEncoding = kCFStringEncodingUTF8;
#ifdef __BIG_ENDIAN__
static const CFStringEncoding kMediumStringEncoding = kCFStringEncodingUTF16BE;
static const CFStringEncoding kWideStringEncoding = kCFStringEncodingUTF32BE;
#elif defined(__LITTLE_ENDIAN__)
static const CFStringEncoding kMediumStringEncoding = kCFStringEncodingUTF16LE;
static const CFStringEncoding kWideStringEncoding = kCFStringEncodingUTF32LE;
#endif  // __LITTLE_ENDIAN__

}  // namespace

std::string SysWideToGBK(const std::wstring& wide) {
  return STLStringToSTLStringWithEncodingsT<std::wstring, std::string>(
      wide, kWideStringEncoding, kCFStringEncodingGB_18030_2000);
}

std::wstring SysGBKToWide(const std::string& gbk) {
  return STLStringToSTLStringWithEncodingsT<std::string, std::wstring>(
      gbk, kCFStringEncodingGB_18030_2000, kWideStringEncoding);
}

// Do not assert in this function since it is used by the asssertion code!
std::string SysWideToUTF8(const std::wstring& wide) {
  return STLStringToSTLStringWithEncodingsT<std::wstring, std::string>(
      wide, kWideStringEncoding, kNarrowStringEncoding);
}

// Do not assert in this function since it is used by the asssertion code!
std::wstring SysUTF8ToWide(const std::string& utf8) {
  return STLStringToSTLStringWithEncodingsT<std::string, std::wstring>(
      utf8, kNarrowStringEncoding, kWideStringEncoding);
}

std::string SysWideToNativeMB(const std::wstring& wide) {
  return SysWideToUTF8(wide);
}

std::wstring SysNativeMBToWide(const std::string& native_mb) {
  return SysUTF8ToWide(native_mb);
}

CFStringRef SysUTF8ToCFStringRef(const std::string& utf8) {
  return STLStringToCFStringWithEncodingsT(utf8, kNarrowStringEncoding);
}

CFStringRef SysUTF16ToCFStringRef(const std::u16string& utf16) {
  return STLStringToCFStringWithEncodingsT(utf16, kMediumStringEncoding);
}

NSString* SysUTF8ToNSString(const std::string& utf8) {
  return (NSString*)base::CFTypeRefToNSObjectAutorelease(
      SysUTF8ToCFStringRef(utf8));
}

NSString* SysUTF16ToNSString(const std::u16string& utf16) {
  return (NSString*)base::CFTypeRefToNSObjectAutorelease(
      SysUTF16ToCFStringRef(utf16));
}

std::string SysCFStringRefToUTF8(CFStringRef ref) {
  return CFStringToSTLStringWithEncodingT<std::string>(ref,
                                                       kNarrowStringEncoding);
}

std::u16string SysCFStringRefToUTF16(CFStringRef ref) {
  return CFStringToSTLStringWithEncodingT<std::u16string>(ref,
                                                    kMediumStringEncoding);
}

std::string SysNSStringToUTF8(NSString* nsstring) {
  if (!nsstring)
    return std::string();
  return SysCFStringRefToUTF8(reinterpret_cast<CFStringRef>(nsstring));
}

std::u16string SysNSStringToUTF16(NSString* nsstring) {
  if (!nsstring)
    return std::u16string();
  return SysCFStringRefToUTF16(reinterpret_cast<CFStringRef>(nsstring));
}

}  // namespace base
