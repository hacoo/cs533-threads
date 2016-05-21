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

struct mutex {
  int held;
  struct queue waiting_threads;
};

void mutex_init(struct mutex*);
void mutex_lock(struct mutex*);
void mutex_unlock(struct mutex*);

struct condition {
  struct queue waiting_threads;
};

void condition_init(struct condition*);
void condition_wait(struct condition*, struct mutex*);
void condition_signal(struct condition*);
void condition_broadcast(struct condition*);

typedef struct thread thread;
struct thread {
  unsigned char* stack_pointer;
  void (*initial_function)(void*);
  void* initial_argument;
  state_t state;
  struct condition thread_done_cond;
  struct mutex mtx;
};

extern thread* current_thread;
thread* inactive_thread;

void scheduler_begin();
thread* thread_fork(void (*target)(void*), void* arg);
void thread_join(thread* th);
void yield();
void scheduler_end();

#endif
