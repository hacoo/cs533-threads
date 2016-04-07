// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 6 Apr. 2016
//
// 533-threads/thread.c
// 
// Implements a user-level thread library.

#include "thread.h"

// Save old's state and switch to thread new.
void thread_switch(struct thread * old, struct thread * new) {
  // requires assembly -- implemented in thread_switch.s
  thread_switch_asm(old, new);
}
