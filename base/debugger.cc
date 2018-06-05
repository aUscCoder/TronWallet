// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "debugger.h"
#include "logging.h"
#include "build_config.h"

namespace base {
namespace debug {

static bool is_debug_ui_suppressed = false;

bool WaitForDebugger(int wait_seconds, bool silent) {
  for (int i = 0; i < wait_seconds * 10; ++i) {
    if (BeingDebugged()) {
      if (!silent)
        BreakDebugger();
      return true;
    }
    Sleep(100);
  }
  return false;
}

void SetSuppressDebugUI(bool suppress) {
  is_debug_ui_suppressed = suppress;
}

bool IsDebugUISuppressed() {
  return is_debug_ui_suppressed;
}

}  // namespace debug
}  // namespace base
