// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This defines helpful methods for dealing with Callbacks.  Because Callbacks
// are implemented using templates, with a class per callback signature, adding
// methods to Callback<> itself is unattractive (lots of extra code gets
// generated).  Instead, consider adding methods here.
//
// ResetAndReturn(&cb) is like cb.Reset() but allows executing a callback (via a
// move or copy) after the original callback is Reset().  This can be handy if
// Run() reads/writes the variable holding the Callback.

#ifndef BASE_CALLBACK_HELPERS_H_
#define BASE_CALLBACK_HELPERS_H_

#include <atomic>
#include <memory>
#include <utility>

//#include "base/bind.h"
#include "base/callback_forward.h"
#include "base/macros.h"
#include "base/logging.h"

namespace base {

template <typename CallbackType>
CallbackType ResetAndReturn(CallbackType* cb) {
  CallbackType ret(std::move(*cb));
  DCHECK(!*cb);
  return ret;
}

namespace internal {

template <typename... Args>
class AdaptCallbackForRepeatingHelper final {
 public:
  explicit AdaptCallbackForRepeatingHelper(std::function<void(Args...)> callback)
      : callback_(std::move(callback)) {
    DCHECK(callback_);
  }

  void operator()(Args... args) {
    if (has_run_.exchange(1, std::memory_order_relaxed))
      return;
    DCHECK(callback_);
    std::move(callback_)(std::forward<Args>(args)...);
  }

 private:
  volatile std::atomic<int32_t> has_run_{0};
  std::function<void(Args...)> callback_;

  DISALLOW_COPY_AND_ASSIGN(AdaptCallbackForRepeatingHelper);
};

}  // namespace internal

// Wraps the given OnceCallback into a RepeatingCallback that relays its
// invocation to the original OnceCallback on the first invocation. The
// following invocations are just ignored.
template <typename... Args>
std::function<void(Args...)> AdaptCallbackForRepeating(
    std::function<void(Args...)> callback) {
  using Helper = internal::AdaptCallbackForRepeatingHelper<Args...>;
  return std::bind(&Helper::Run, base::MakeUnique<Helper>(std::move(callback)));
}

// ScopedClosureRunner is akin to std::unique_ptr<> for Closures. It ensures
// that the Closure is executed no matter how the current scope exits.
class ScopedClosureRunner {
 public:
  ScopedClosureRunner();
  explicit ScopedClosureRunner(OnceClosure closure);
  ~ScopedClosureRunner();

  ScopedClosureRunner(ScopedClosureRunner&& other);

  // Releases the current closure if it's set and replaces it with the closure
  // from |other|.
  ScopedClosureRunner& operator=(ScopedClosureRunner&& other);

  // Calls the current closure and resets it, so it wont be called again.
  void RunAndReset();

  // Replaces closure with the new one releasing the old one without calling it.
  void ReplaceClosure(OnceClosure closure);

  // Releases the Closure without calling.
  OnceClosure Release();

 private:
  OnceClosure closure_;

  DISALLOW_COPY_AND_ASSIGN(ScopedClosureRunner);
};

}  // namespace base

#endif  // BASE_CALLBACK_HELPERS_H_