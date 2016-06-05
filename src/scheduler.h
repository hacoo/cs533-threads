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
#include <atomic_ops.h>


// thread-safe malloc macros
extern void * safe_mem(int, void*);
#define malloc(arg) safe_mem(0, ((void*)(arg)))
#define free(arg) safe_mem(1, arg)

// Stack size is 1Mb
#define STACK_SIZE 1048576 

// To avoid refactoring code, current_thread now calls
// the get_current_thread function
#define current_thread (get_current_thread())

// Defined in threadmap.c
extern struct thread * get_current_thread();
extern void set_current_thread(struct thread*);

typedef enum {
  RUNNING,
  READY,
  BLOCKED,
  DONE
} state_t;

// Mutex -- use ONLY with non-concurrent / fully cooperative threads
struct mutex {
  int held;
  struct queue waiting_threads;
  AO_TS_t sl;
};

void block(AO_TS_t*);
void mutex_init(struct mutex*);
void mutex_lock(struct mutex*);
void mutex_unlock(struct mutex*);

struct condition {
  struct queue waiting_threads;
  AO_TS_t sl;
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


// Spinlock -- for concurrent or preemtable threads
void spinlock_lock(AO_TS_t* lock);

void spinlock_unlock(AO_TS_t* lock);

#endif
