// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/thread_task_runner_handle.h"

#include <utility>

#include "base/logging.h"
#include "base/run_loop.h"
#include "base/threading/sequenced_task_runner_handle.h"
#include "../callback_helpers.h"

namespace base {

namespace {

thread_local base::ThreadTaskRunnerHandle* thread_task_runner_tls = nullptr;

}  // namespace

// static
scoped_refptr<SingleThreadTaskRunner> ThreadTaskRunnerHandle::Get() {
  CHECK(thread_task_runner_tls)
      << "Error: This caller requires a single-threaded context "
         "(i.e. the current task needs to run from a "
         "SingleThreadTaskRunner).";
  return thread_task_runner_tls->task_runner_;
}

// static
bool ThreadTaskRunnerHandle::IsSet() {
  return !!thread_task_runner_tls;
}

// static
ScopedClosureRunner ThreadTaskRunnerHandle::OverrideForTesting(
    scoped_refptr<SingleThreadTaskRunner> overriding_task_runner) {
  // OverrideForTesting() is not compatible with a SequencedTaskRunnerHandle
  // being set (but SequencedTaskRunnerHandle::IsSet() includes
  // ThreadTaskRunnerHandle::IsSet() so that's discounted as the only valid
  // excuse for it to be true). Sadly this means that tests that merely need a
  // SequencedTaskRunnerHandle on their main thread can be forced to use a
  // ThreadTaskRunnerHandle if they're also using test task runners (that
  // OverrideForTesting() when running their tasks from said main thread). To
  // solve this: sequence_task_runner_handle.cc and thread_task_runner_handle.cc
  // would have to be merged into a single impl file and share TLS state. This
  // was deemed unecessary for now as most tests should use higher level
  // constructs and not have to instantiate task runner handles on their own.
  DCHECK(!SequencedTaskRunnerHandle::IsSet() || IsSet());

  if (!IsSet()) {
    auto top_level_ttrh = base::MakeUnique<ThreadTaskRunnerHandle>(
        std::move(overriding_task_runner));
    OnceClosure closure = std::bind(
        [&top_level_ttrh] () {});
    return ScopedClosureRunner(closure);
  }

  ThreadTaskRunnerHandle* ttrh = thread_task_runner_tls;
  // Swap the two (and below bind |overriding_task_runner|, which is now the
  // previous one, as the |task_runner_to_restore|).
  ttrh->task_runner_.swap(overriding_task_runner);

  auto no_running_during_override =
      base::MakeUnique<RunLoop::ScopedDisallowRunningForTesting>();

  OnceClosure closure = std::bind(
      [&no_running_during_override](scoped_refptr<SingleThreadTaskRunner> task_runner_to_restore,
                                    SingleThreadTaskRunner* expected_task_runner_before_restore) {
        ThreadTaskRunnerHandle* ttrhl = thread_task_runner_tls;

        DCHECK_EQ(expected_task_runner_before_restore, ttrhl->task_runner_.get())
            << "Nested overrides must expire their ScopedClosureRunners "
               "in LIFO order.";

          ttrhl->task_runner_.swap(task_runner_to_restore);
      },
      overriding_task_runner,
      ttrh->task_runner_.get());

  return ScopedClosureRunner(OnceClosure());
}

ThreadTaskRunnerHandle::ThreadTaskRunnerHandle(
    scoped_refptr<SingleThreadTaskRunner> task_runner)
    : task_runner_(std::move(task_runner)) {
  DCHECK(task_runner_->BelongsToCurrentThread());
  // No SequencedTaskRunnerHandle (which includes ThreadTaskRunnerHandles)
  // should already be set for this thread.
  DCHECK(!SequencedTaskRunnerHandle::IsSet());
  thread_task_runner_tls = this;
}

ThreadTaskRunnerHandle::~ThreadTaskRunnerHandle() {
  DCHECK(task_runner_->BelongsToCurrentThread());
  DCHECK_EQ(thread_task_runner_tls, this);
  thread_task_runner_tls = nullptr;
}

}  // namespace base