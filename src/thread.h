// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 6 Apr. 2016
// 
// 533-threads/thread.h
// 
// Implements a user-level thread library.

#ifndef THREAD_H
#define THREAD_H

#include "stdio.h"
#include "stdlib.h"

// Stack size is 1Mb
#define STACK_SIZE 1048576 

typedef struct thread thread;
struct thread {
  unsigned char* stack_pointer;
  void (*initial_function)(void*);
  void* initial_argument;
};

thread* current_thread;
thread* inactive_thread;

void thread_switch(struct thread * old, struct thread * new);
void yield();
void thread_wrap();

#endif 


