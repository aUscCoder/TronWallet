// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "scoped_handle.h"

#include <stddef.h>

#include <mutex>
#include <functional>
#include <unordered_map>

#include "../build_config.h"
#include "../logging.h"
#include "../macros.h"

#include "current_module.h"

extern "C" {
__declspec(dllexport) void* GetHandleVerifier();
typedef void* (*GetHandleVerifierFn)();
}

namespace {

struct HandleHash {
  size_t operator()(const HANDLE& handle) const {
    char buffer[sizeof(handle)];
    memcpy(buffer, &handle, sizeof(handle));
    return std::hash<std::string>()(std::string(buffer, sizeof(buffer)));
  }
};

struct Info {
  const void* owner;
  const void* pc1;
  const void* pc2;
  DWORD thread_id;
};
typedef std::unordered_map<HANDLE, Info, HandleHash> HandleMap;

// g_lock protects the handle map and setting g_active_verifier within this
// module.
std::mutex g_lock;

// Implements the actual object that is verifying handles for this process.
// The active instance is shared across the module boundary but there is no
// way to delete this object from the wrong side of it (or any side, actually).
class ActiveVerifier {
 public:
  explicit ActiveVerifier(bool enabled)
      : enabled_(enabled), lock_(&g_lock) {
  }

  // Retrieves the current verifier.
  static ActiveVerifier* Get();

  // The methods required by HandleTraits. They are virtual because we need to
  // forward the call execution to another module, instead of letting the
  // compiler call the version that is linked in the current module.
  virtual bool CloseHandle(HANDLE handle);
  virtual void StartTracking(HANDLE handle, const void* owner,
                             const void* pc1, const void* pc2);
  virtual void StopTracking(HANDLE handle, const void* owner,
                            const void* pc1, const void* pc2);
  virtual void Disable();
  virtual void OnHandleBeingClosed(HANDLE handle);
  virtual HMODULE GetModule() const;

 private:
  ~ActiveVerifier();  // Not implemented.

  static void InstallVerifier();

  bool enabled_;
  static thread_local bool closing_;
  std::mutex* lock_;
  HandleMap map_;
  DISALLOW_COPY_AND_ASSIGN(ActiveVerifier);
};
ActiveVerifier* g_active_verifier = NULL;

bool thread_local ActiveVerifier::closing_ = false;

// static
ActiveVerifier* ActiveVerifier::Get() {
  if (!g_active_verifier)
    ActiveVerifier::InstallVerifier();

  return g_active_verifier;
}

bool CloseHandleWrapper(HANDLE handle) {
  if (!::CloseHandle(handle))
    CHECK(false);  // CloseHandle failed.
  return true;
}

// Assigns the g_active_verifier global within the g_lock lock.
// If |existing_verifier| is non-null then |enabled| is ignored.
void ThreadSafeAssignOrCreateActiveVerifier(ActiveVerifier* existing_verifier,
                                            bool enabled) {
  std::lock_guard<std::mutex> locked(g_lock);
  // Another thread in this module might be trying to assign the global
  // verifier, so check that within the lock here.
  if (g_active_verifier)
    return;
  g_active_verifier =
      existing_verifier ? existing_verifier : new ActiveVerifier(enabled);
}

// static
void ActiveVerifier::InstallVerifier() {
/*
#if BUILDFLAG(SINGLE_MODULE_MODE_HANDLE_VERIFIER)
  // Component build has one Active Verifier per module.
  ThreadSafeAssignOrCreateActiveVerifier(nullptr, true);
#else
*/
  // If you are reading this, wondering why your process seems deadlocked, take
  // a look at your DllMain code and remove things that should not be done
  // there, like doing whatever gave you that nice windows handle you are trying
  // to store in a ScopedHandle.
  HMODULE main_module = ::GetModuleHandle(NULL);
  GetHandleVerifierFn get_handle_verifier =
      reinterpret_cast<GetHandleVerifierFn>(::GetProcAddress(
          main_module, "GetHandleVerifier"));

  // This should only happen if running in a DLL is linked with base but the
  // hosting EXE is not. In this case, create an ActiveVerifier for the current
  // module but leave it disabled.
  if (!get_handle_verifier) {
    ThreadSafeAssignOrCreateActiveVerifier(nullptr, false);
    return;
  }

  // Check if in the main module.
  if (get_handle_verifier == GetHandleVerifier) {
    ThreadSafeAssignOrCreateActiveVerifier(nullptr, true);
    return;
  }

  ActiveVerifier* main_module_verifier =
      reinterpret_cast<ActiveVerifier*>(get_handle_verifier());

  // Main module should always on-demand create a verifier.
  DCHECK(main_module_verifier);

  ThreadSafeAssignOrCreateActiveVerifier(main_module_verifier, false);
/*
#endif
*/
}

bool ActiveVerifier::CloseHandle(HANDLE handle) {
  if (!enabled_)
    return CloseHandleWrapper(handle);

  closing_ = true;
  CloseHandleWrapper(handle);
  closing_ = false;

  return true;
}

void ActiveVerifier::StartTracking(HANDLE handle, const void* owner,
                                   const void* pc1, const void* pc2) {
  if (!enabled_)
    return;

  // Grab the thread id before the lock.
  DWORD thread_id = GetCurrentThreadId();

  std::lock_guard<std::mutex> locked(*lock_);

  Info handle_info = { owner, pc1, pc2, thread_id };
  std::pair<HANDLE, Info> item(handle, handle_info);
  std::pair<HandleMap::iterator, bool> result = map_.insert(item);
  CHECK(result.second);  // Attempt to start tracking already tracked handle.
}

void ActiveVerifier::StopTracking(HANDLE handle, const void* owner,
                                  const void* pc1, const void* pc2) {
  if (!enabled_)
    return;

  std::lock_guard<std::mutex> locked(*lock_);
  HandleMap::iterator i = map_.find(handle);
  CHECK(i != map_.end());  // Attempting to close an untracked handle.

  Info other = i->second;
  CHECK(other.owner == owner);  // Attempting to close a handle not owned by opener.

  map_.erase(i);
}

void ActiveVerifier::Disable() {
  enabled_ = false;
}

void ActiveVerifier::OnHandleBeingClosed(HANDLE handle) {
  if (!enabled_)
    return;

  if (closing_)
    return;

  std::lock_guard<std::mutex> locked(*lock_);
  HandleMap::iterator i = map_.find(handle);
  if (i == map_.end())
    return;

  Info other = i->second;
  CHECK(false);  // CloseHandle called on tracked handle.
}

HMODULE ActiveVerifier::GetModule() const {
  return CURRENT_MODULE();
}

}  // namespace

void* GetHandleVerifier() {
  return ActiveVerifier::Get();
}

namespace base {
namespace win {

const void* GetProgramCounter() {
#if defined(COMPILER_MSVC)
  return _ReturnAddress();
#elif defined(COMPILER_GCC)
  return __builtin_extract_return_addr(__builtin_return_address(0));
#else
  return NULL;
#endif
}

// Static.
bool HandleTraits::CloseHandle(HANDLE handle) {
  return ActiveVerifier::Get()->CloseHandle(handle);
}

// Static.
void VerifierTraits::StartTracking(HANDLE handle, const void* owner,
                                   const void* pc1, const void* pc2) {
  return ActiveVerifier::Get()->StartTracking(handle, owner, pc1, pc2);
}

// Static.
void VerifierTraits::StopTracking(HANDLE handle, const void* owner,
                                  const void* pc1, const void* pc2) {
  return ActiveVerifier::Get()->StopTracking(handle, owner, pc1, pc2);
}

void DisableHandleVerifier() {
  return ActiveVerifier::Get()->Disable();
}

void OnHandleBeingClosed(HANDLE handle) {
  return ActiveVerifier::Get()->OnHandleBeingClosed(handle);
}

HMODULE GetHandleVerifierModuleForTesting() {
  return g_active_verifier->GetModule();
}

}  // namespace win
}  // namespace base