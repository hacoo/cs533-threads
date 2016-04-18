// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 6 Apr. 2016
//
// 533-threads/thread.
//
// User-level thread scheduler

#include "scheduler.h"

void thread_switch(thread* old, thread* new) {
  // requires assembly -- implemented in thread_switch.s
  thread_switch_asm(old, new);
}

void thread_start(thread* old, thread* new) {
  // requires assembly -- implemented in thread_switch.s
  thread_start_asm(old, new);
}

// Used to start a new thread with requested function
void thread_wrap() {
  current_thread->initial_function(current_thread->initial_argument);
  yield();
}

// Yield and swap current/active threads
void yield() {
  thread* temp = current_thread;
  current_thread = inactive_thread;
  inactive_thread = temp;
  thread_switch(inactive_thread, current_thread);
}


