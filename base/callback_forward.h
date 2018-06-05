// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CALLBACK_FORWARD_H_
#define BASE_CALLBACK_FORWARD_H_

#include <functional>

namespace base {
/*
template <typename Signature>
class OnceCallback;

template <typename Signature>
class RepeatingCallback;

template <typename Signature>
using Callback = RepeatingCallback<Signature>;

// Syntactic sugar to make Callback<void()> easier to declare since it
// will be used in a lot of APIs with delayed execution.
using OnceClosure = OnceCallback<void()>;
using RepeatingClosure = RepeatingCallback<void()>;
using Closure = Callback<void()>;
*/

// As we use cxx11's bind and function, we hijack the underlying callback classes.
template <typename Signature>
using OnceCallback = std::function<Signature>;

template <typename Signature>
using RepeatingCallback = std::function<Signature>;

template <typename Signature>
using Callback = std::function<Signature>;

using OnceClosure = std::function<void()>;
using RepeatingClosure = std::function<void()>;
using Closure = std::function<void()>;

}  // namespace base

#endif  // BASE_CALLBACK_FORWARD_H_