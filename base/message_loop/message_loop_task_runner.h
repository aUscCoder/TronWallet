// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MESSAGE_LOOP_MESSAGE_LOOP_TASK_RUNNER_H_
#define BASE_MESSAGE_LOOP_MESSAGE_LOOP_TASK_RUNNER_H_

#include <thread>
#include <mutex>

#include "base/callback_forward.h"
#include "base/macros.h"
#include "base/ref_counted.h"
#include "base/pending_task.h"
#include "base/single_thread_task_runner.h"

namespace base {
namespace internal {

class IncomingTaskQueue;

// A stock implementation of SingleThreadTaskRunner that is created and managed
// by a MessageLoop. For now a MessageLoopTaskRunner can only be created as
// part of a MessageLoop.
class MessageLoopTaskRunner : public SingleThreadTaskRunner {
 public:
  explicit MessageLoopTaskRunner(
      scoped_refptr<IncomingTaskQueue> incoming_queue);

  // Initialize this message loop task runner on the current thread.
  void BindToCurrentThread();

  // SingleThreadTaskRunner implementation
  bool PostDelayedTask(const Location& from_here,
                       OnceClosure task,
                       TimeDelta delay) override;
  bool PostNonNestableDelayedTask(const Location& from_here,
                                  OnceClosure task,
                                  TimeDelta delay) override;
  bool RunsTasksInCurrentSequence() const override;

 private:
  friend class RefCountedThreadSafe<MessageLoopTaskRunner>;
  ~MessageLoopTaskRunner() override;

  // The incoming queue receiving all posted tasks.
  scoped_refptr<IncomingTaskQueue> incoming_queue_;

  // ID of the thread |this| was created on.  Could be accessed on multiple
  // threads, protected by |valid_thread_id_lock_|.
  std::thread::id valid_thread_id_;
  mutable std::mutex valid_thread_id_lock_;

  DISALLOW_COPY_AND_ASSIGN(MessageLoopTaskRunner);
};

}  // namespace internal
}  // namespace base

#endif  // BASE_MESSAGE_LOOP_MESSAGE_LOOP_TASK_RUNNER_H_
