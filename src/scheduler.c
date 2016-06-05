// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 6 Apr. 2016
//
// 533-threads/thread.h
//
// User-level thread scheduler

#define _GNU_SOURCE
#include <sched.h>
#include "scheduler.h"

#undef malloc
#undef free

static AO_TS_t spinlock = AO_TS_INITIALIZER;
void * safe_mem(int op, void * arg) {

  void * result = 0;
  spinlock_lock(&spinlock);
  if(op == 0) {
    result = malloc((size_t)arg);
  } else {
    free(arg);
  }
  spinlock_unlock(&spinlock);
  return result;
}
#define malloc(arg) safe_mem(0, ((void*)(arg)))
#define free(arg) safe_mem(1, arg)

// global thread queue
queue ready_list;
// ready list lock
static AO_TS_t ready_list_lock = AO_TS_INITIALIZER;


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

  // Release the ready list lock -- it should have been locked before
  // calling thread start, but now we are done manipulating ready list
  spinlock_unlock(&ready_list_lock);

  current_thread->initial_function(current_thread->initial_argument);
  // After completion, mark the thread as done and yield.
  current_thread->state = DONE;
  // Signal any threads waiting for this thread to complete
  condition_broadcast(&(current_thread->thread_done_cond));
  yield();
}

// Starter function for a new kernel thread. The thread will initialize
// itself in the thread table, and then spin until it has work to do.
int kernel_thread_begin(void* arg) {
  // Create a TCB for this thread. This is an idle thread -- it does
  // not need a stack or initial function.
  
  thread* thread_block = malloc(sizeof(thread));
  thread_block->initial_function = NULL;
  // On yield, the newly created thread's stack pointer will be saved,
  // so no need to create one
  thread_block->stack_pointer = NULL;
  thread_block->state = RUNNING; 
  mutex_init(&(thread_block->mtx));
  condition_init(&(thread_block->thread_done_cond));

  // Set the idle thread as the current thread for this CPU.
  // Allocating a thread table entry should be handled automatically
  set_current_thread(thread_block);

  // Idle
  while(1) {
    yield();
  }

  return 0;
}

// Start the scheduler and run on current_thread
void scheduler_begin() {
  // Allocate the current thread
  set_current_thread(malloc(sizeof(thread)));
  current_thread->state = RUNNING;
  mutex_init(&(current_thread->mtx));
  condition_init(&(current_thread->thread_done_cond));
  // Control flow will continue unthreaded until fork() is called.
  // At that point, the main thread will be saved in current_thread.
  // For now, current_thread remains empty.

  // initialize the ready list
  ready_list.head = NULL;
  ready_list.tail = NULL;
  
  
  char* child_stack = malloc(STACK_SIZE);
  clone(&kernel_thread_begin, child_stack + STACK_SIZE,
  	(CLONE_THREAD | CLONE_VM | CLONE_SIGHAND |
  	 CLONE_FILES  | CLONE_FS | CLONE_IO), NULL);
}

// End the scheduler. Will continue until there are no more runnable threads.
void scheduler_end() {
  spinlock_lock(&ready_list_lock);
  while(!is_empty(&ready_list)) {
    spinlock_unlock(&ready_list_lock);
    yield();
    spinlock_lock(&ready_list_lock);
  }
  spinlock_unlock(&ready_list_lock);
}

// Create a thread with starting function target, and argument
// arg. Will start immediately.
thread* thread_fork(void(*target)(void*), void* arg) {
  // Create a new TCB
  thread* new = malloc(sizeof(thread));
  new->initial_function = target;
  new->initial_argument = arg;
  mutex_init(&(new->mtx));
  condition_init(&(new->thread_done_cond));
  

  // Allocate the new thread's stack:
  void* new_stack = malloc(STACK_SIZE);
  new->stack_pointer = new_stack + STACK_SIZE;
  new->state = RUNNING;
  
  // Lock ready list
  spinlock_lock(&ready_list_lock);

  // Save the old thread
  thread* old = current_thread;
  old->state = READY;
  thread_enqueue(&ready_list, old);
  
  // Swap running threads
  set_current_thread(new);
  thread_start(old, new);
  
  spinlock_unlock(&ready_list_lock);
  return new;
}

// Wait for th to complete.
void thread_join(thread* th) {
  mutex_lock(&(th->mtx));
  while(th->state != DONE) {
    condition_wait(&(th->thread_done_cond), &(th->mtx));
  }
  mutex_unlock(&(th->mtx));
}

// Yield to the next available thread
void yield() {

  thread* old = current_thread;
 
  // Lock the ready queue -- will need to be released
  // after switch
  spinlock_lock(&ready_list_lock);

  // If the current thread isn't done, swap it.
  if (current_thread->state != DONE) {
    
    if (current_thread->state != BLOCKED) {
      // Put the current thread back on the ready queue
      old->state = READY;    
      thread_enqueue(&ready_list, old);
    } else {
      // Otherwise, the thread is BLOCKED somewhere else,
      // and should not reenqueue. We need to check that the 
      // ready list isn't empty, if it is, the program crashes.
      if (is_empty(&ready_list)) {
	printf("FATAL ERROR -- Tried to block when the ready queue was empty\n");
	exit(1);
      }
    }

    // Swap in the new thread. This is done after enqueue the old
    // thread, so there is guaranteed to be a waiting thread
    thread* new = thread_dequeue(&ready_list);
    new->state = RUNNING;
    set_current_thread(new);
    
    thread_switch(old, new);
  }
  // Otherwise, destroy current_thread and run the next available thread.
  else {
    // Currently there is no memory management -- the old TCB is leaked
    thread* new = thread_dequeue(&ready_list);
    new->state = RUNNING;
    set_current_thread(new);
    thread_switch(old, new);
  }  
  
  // Eventually, we will switch back into this thread,
  // at which point it is safe to unlock t
  spinlock_unlock(&ready_list_lock);
}

// Mutex -- since mutexes must access the ready list, they 
// are included in this file directly.
void mutex_init(struct mutex* mtx) {
  mtx->sl = AO_TS_INITIALIZER;
  mtx->held = 0;
  mtx->waiting_threads.head = NULL;
  mtx->waiting_threads.tail = NULL;
}

// Behaves identically to yield(), but should be used
// on blocking yields only. This will release the lock in mutex
// mtx (since this must be done after aquiring the ready list lock)
void block(AO_TS_t* sl) {

  thread* old = current_thread;
 
  // Lock the ready queue -- will need to be released
  // after switch
  spinlock_lock(&ready_list_lock);

  // It is now safe to unlock the mutex, since no-one else
  // can reschedule the current thread
  spinlock_unlock(sl);

  // If the current thread isn't done, swap it.
  if (current_thread->state != DONE) {
    
    if (current_thread->state != BLOCKED) {
      // Put the current thread back on the ready queue
      old->state = READY;    
      thread_enqueue(&ready_list, old);
    } else {
      // Otherwise, the thread is BLOCKED somewhere else,
      // and should not reenqueue. We need to check that the 
      // ready list isn't empty, if it is, the program crashes.
      if (is_empty(&ready_list)) {
	printf("FATAL ERROR -- Tried to block when the ready queue was empty\n");
	exit(1);
      }
    }

    // Swap in the new thread. This is done after enqueue the old
    // thread, so there is guaranteed to be a waiting thread
    thread* new = thread_dequeue(&ready_list);
    new->state = RUNNING;
    set_current_thread(new);
    
    thread_switch(old, new);
  }
  // Otherwise, destroy current_thread and run the next available thread.
  else {
    // Currently there is no memory management -- the old TCB is leaked
    thread* new = thread_dequeue(&ready_list);
    new->state = RUNNING;
    set_current_thread(new);
    thread_switch(old, new);
  }  
  
  // Eventually, we will switch back into this thread,
  // at which point it is safe to unlock t
  spinlock_unlock(&ready_list_lock);
}


void mutex_lock(struct mutex* mtx) {
  spinlock_lock(&(mtx->sl));
  if (mtx->held == 0) {
    mtx->held = 1;
    spinlock_unlock(&(mtx->sl));
  } else {		 
    current_thread->state = BLOCKED;
    thread_enqueue(&(mtx->waiting_threads), current_thread);
    block(&(mtx->sl));
  }
}

void mutex_unlock(struct mutex* mtx) {
  spinlock_lock(&(mtx->sl));
  if (is_empty(&(mtx->waiting_threads))) {
    // No one is waiting -- free the mutex
    mtx->held = 0;
  } else {
    // Otherwise, transfer mutex control to the next thread
    spinlock_lock(&ready_list_lock);
    struct thread* awoken = thread_dequeue(&(mtx->waiting_threads));
    awoken->state = READY;
    thread_enqueue(&ready_list, awoken);
    spinlock_unlock(&ready_list_lock);
  }
  spinlock_unlock(&(mtx->sl));
}


// Condition -- for condition synchronization.
void condition_init(struct condition* cond) {
  cond->sl = AO_TS_INITIALIZER;
  cond->waiting_threads.head = NULL;
  cond->waiting_threads.tail = NULL;
}

// Thread will wait on the condition until another thread SIGNALS it. 
// The thread is responsible for  rechecking the
// condition when it wakes up.
void condition_wait(struct condition* cond, struct mutex* mtx) {
  spinlock_lock(&(cond->sl)); 
  current_thread->state = BLOCKED;
  thread_enqueue(&(cond->waiting_threads), current_thread);
  spinlock_unlock(&(cond->sl));
  mutex_unlock(mtx);
  yield();
  mutex_lock(mtx); // After waiting, the thread will still hold the mutex,
  // but should recheck the condition
}


// Will wake up the next available thread waiting on the condition COND.
// If no threads are waiting, this action does nothing.
void condition_signal(struct condition* cond) {
  spinlock_lock(&(cond->sl));
  if(is_empty(&(cond->waiting_threads))) {
    spinlock_unlock(&(cond->sl));
    return; // Do nothing if no one is waiting on cond
  }
  // Otherwise, wake up the next thread.
  struct thread* next = thread_dequeue(&(cond->waiting_threads));
  next->state = READY;
  spinlock_lock(&ready_list_lock);
  thread_enqueue(&ready_list, next);
  spinlock_unlock(&ready_list_lock);
  spinlock_unlock(&(cond->sl));
}

// Wake up ALL threads waiting on cond.
void condition_broadcast(struct condition* cond) {
  struct thread* next;
  spinlock_lock(&(cond->sl));
  spinlock_lock(&ready_list_lock);
  while(!is_empty(&(cond->waiting_threads))) {
    next = thread_dequeue(&(cond->waiting_threads));
    next->state = READY;
    thread_enqueue(&ready_list, next);
  }
  spinlock_unlock(&ready_list_lock);
  spinlock_unlock(&(cond->sl));
}


// Spinlock -- for concurrent or preemtable threads
void spinlock_lock(AO_TS_t* lock) {
  while(AO_test_and_set_acquire(lock) == AO_TS_SET){
    ;
  }
}

void spinlock_unlock(AO_TS_t* lock) {
  AO_CLEAR(lock);
}
