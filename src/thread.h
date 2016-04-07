// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 6 Apr. 2016
// 
// 533-threads/thread.h
// 
// Implements a user-level thread library.

#ifndef THREAD_H
#define THREAD_H

typedef struct thread thread;
struct thread {
  unsigned char* stack_pointer;
  void (*initial_function)(void*);
  void* initial_argument;
};

#endif 


