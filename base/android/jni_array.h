// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ANDROID_JNI_ARRAY_H_
#define BASE_ANDROID_JNI_ARRAY_H_

#include <jni.h>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

#include "base/android/scoped_java_ref.h"

namespace base {
namespace android {

// Returns a new Java byte array converted from the given bytes array.
ScopedJavaLocalRef<jbyteArray> ToJavaByteArray(JNIEnv* env,
                                               const uint8_t* bytes,
                                               size_t len);

ScopedJavaLocalRef<jbyteArray> ToJavaByteArray(
    JNIEnv* env,
    const std::vector<uint8_t>& bytes);

// Returns a new Java boolean array converted from the given bool array.
ScopedJavaLocalRef<jbooleanArray>
ToJavaBooleanArray(JNIEnv* env, const bool* bools, size_t len);

// Returns a new Java int array converted from the given int array.
ScopedJavaLocalRef<jintArray> ToJavaIntArray(
    JNIEnv* env, const int* ints, size_t len);

ScopedJavaLocalRef<jintArray> ToJavaIntArray(
    JNIEnv* env, const std::vector<int>& ints);

// Returns a new Java long array converted from the given int64_t array.
ScopedJavaLocalRef<jlongArray> ToJavaLongArray(JNIEnv* env,
                                                           const int64_t* longs,
                                                           size_t len);

ScopedJavaLocalRef<jlongArray> ToJavaLongArray(
    JNIEnv* env,
    const std::vector<int64_t>& longs);

// Returns a new Java float array converted from the given C++ float array.
ScopedJavaLocalRef<jfloatArray> ToJavaFloatArray(
    JNIEnv* env, const float* floats, size_t len);

ScopedJavaLocalRef<jfloatArray> ToJavaFloatArray(
    JNIEnv* env,
    const std::vector<float>& floats);

// Returns a array of Java byte array converted from |v|.
ScopedJavaLocalRef<jobjectArray> ToJavaArrayOfByteArray(
    JNIEnv* env, const std::vector<std::string>& v);

ScopedJavaLocalRef<jobjectArray> ToJavaArrayOfStrings(
    JNIEnv* env,  const std::vector<std::string>& v);

ScopedJavaLocalRef<jobjectArray> ToJavaArrayOfStrings(
    JNIEnv* env,  const std::vector<std::u16string>& v);

using JavaObjectWriterFunc = std::function<ScopedJavaLocalRef<jobject>(JNIEnv* env, size_t index)>;

ScopedJavaLocalRef<jobjectArray> ToJavaArrayOfObjects(
    JNIEnv* env, const jclass& object_clz, size_t size, JavaObjectWriterFunc writer);

// Converts a Java string array to a native array.
void AppendJavaStringArrayToStringVector(
    JNIEnv* env,
    jobjectArray array,
    std::vector<std::u16string>* out);

void AppendJavaStringArrayToStringVector(
    JNIEnv* env,
    jobjectArray array,
    std::vector<std::string>* out);

// Appends the Java bytes in |bytes_array| onto the end of |out|.
void AppendJavaByteArrayToByteVector(JNIEnv* env,
                                     jbyteArray byte_array,
                                     std::vector<uint8_t>* out);

// Replaces the content of |out| with the Java bytes in |bytes_array|.
void JavaByteArrayToByteVector(JNIEnv* env,
                               jbyteArray byte_array,
                               std::vector<uint8_t>* out);

// Replaces the content of |out| with the Java booleans in |boolean_array|.
void JavaBooleanArrayToBoolVector(JNIEnv* env,
                                  jbooleanArray boolean_array,
                                  std::vector<bool>* out);

// Replaces the content of |out| with the Java ints in |int_array|.
void JavaIntArrayToIntVector(
    JNIEnv* env,
    jintArray int_array,
    std::vector<int>* out);

// Replaces the content of |out| with the Java longs in |long_array|.
void JavaLongArrayToInt64Vector(JNIEnv* env,
                                jlongArray long_array,
                                std::vector<int64_t>* out);

// Replaces the content of |out| with the Java longs in |long_array|.
void JavaLongArrayToLongVector(
    JNIEnv* env,
    jlongArray long_array,
    std::vector<jlong>* out);

// Replaces the content of |out| with the Java floats in |float_array|.
void JavaFloatArrayToFloatVector(
    JNIEnv* env,
    jfloatArray float_array,
    std::vector<float>* out);

// Assuming |array| is an byte[][] (array of byte arrays), replaces the
// content of |out| with the corresponding vector of strings. No UTF-8
// conversion is performed.
void JavaArrayOfByteArrayToStringVector(
    JNIEnv* env,
    jobjectArray array,
    std::vector<std::string>* out);

// Assuming |array| is an int[][] (array of int arrays), replaces the
// contents of |out| with the corresponding vectors of ints.
void JavaArrayOfIntArrayToIntVector(
    JNIEnv* env,
    jobjectArray array,
    std::vector<std::vector<int>>* out);

using JavaObjectReaderFunc = std::function<void(JNIEnv* env, jobject item)>;

void JavaArrayOfObjectToObjectVector(
    JNIEnv* env,
    jobjectArray array,
    JavaObjectReaderFunc reader);

}  // namespace android
}  // namespace base

#endif  // BASE_ANDROID_JNI_ARRAY_H_
