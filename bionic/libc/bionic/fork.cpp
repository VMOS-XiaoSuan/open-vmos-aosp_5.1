/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <unistd.h>
#include <sys/syscall.h>

#include "pthread_internal.h"

// #define FORK_FLAGS (CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | SIGCHLD)

// int fork() {
//   __bionic_atfork_run_prepare();

//   pthread_internal_t* self = __get_thread();

//   // Remember the parent pid and invalidate the cached value while we fork.
//   pid_t parent_pid = self->invalidate_cached_pid();

// #if defined(__x86_64__) // sys_clone's last two arguments are flipped on x86-64.
//   int result = syscall(__NR_clone, FORK_FLAGS, NULL, NULL, &(self->tid), NULL);
// #else
//   int result = syscall(__NR_clone, FORK_FLAGS, NULL, NULL, NULL, &(self->tid));
// #endif
//   if (result == 0) {
//     self->set_cached_pid(gettid());
//     __bionic_atfork_run_child();
//   } else {
//     self->set_cached_pid(parent_pid);
//     __bionic_atfork_run_parent();
//   }
//   return result;
// }
