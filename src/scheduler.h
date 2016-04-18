// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 17 Apr. 2016
// 
// 533-threads/scheduler.h
// 
// Thread scheduler for user level threads

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

// Stack size is 1Mb
#define STACK_SIZE 1048576 

typedef enum {
  RUNNING,
  READY,
  BLOCKED,
  DONE
} state_t;

//typedef struct thread thread;

typedef struct thread thread;
struct thread {
  unsigned char* stack_pointer;
  void (*initial_function)(void*);
  void* initial_argument;
  state_t state;
};

extern thread* current_thread;
thread* inactive_thread;

void scheduler_begin();
void thread_fork(void (*target)(void*), void* arg);
void yield();
void scheduler_end();






#endif
