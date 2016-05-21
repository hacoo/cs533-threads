// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 6 Apr. 2016
//
// 533-threads/thread.h
//
// User-level thread scheduler

#include "scheduler.h"


// extern -- visibile to all
thread* current_thread;

// global thread queue
queue ready_list;

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
  // After completion, mark the thread as done and yield.
  current_thread->state = DONE;
  // Signal any threads waiting for this thread to complete
  condition_broadcast(&(current_thread->thread_done_cond));
  yield();
}



// Start the scheduler and run on current_thread
void scheduler_begin() {
  // Allocate the current thread
  current_thread = malloc(sizeof(thread));
  current_thread->state = RUNNING;
  // Control flow will continue unthreaded until fork() is called.
  // At that point, the main thread will be saved in current_thread.
  // For now, current_thread remains empty.

  // initialize the ready list
  ready_list.head = NULL;
  ready_list.tail = NULL;
}

// End the scheduler. Will continue until there are no more runnable threads.
void scheduler_end() {
  while(!is_empty(&ready_list))
    yield();
}

// Create a thread with starting function target, and argument
// arg. Will start immediately.
thread* thread_fork(void(*target)(void*), void* arg) {
  // Create a new TCB
  
  thread* new = malloc(sizeof(thread));
  new->initial_function = target;
  new->initial_argument = arg;

  // Allocate the new thread's stack:
  new->stack_pointer = malloc(STACK_SIZE);
  new->state = RUNNING;
  
  // Save the old thread
  thread* old = current_thread;
  old->state = READY;
  thread_enqueue(&ready_list, old);
  
  // Swap running threads
  current_thread = new;
  thread_start(old, new);
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
    current_thread = new;
    
    thread_switch(old, new);
  }
  // Otherwise, destroy current_thread and run the next available thread.
  else {
    // Currently there is no memory management -- the old TCB is leaked
    thread* new = thread_dequeue(&ready_list);
    new->state = RUNNING;
    current_thread = new;
    thread_switch(old, new);
  }  
}


// Mutex -- since mutexes must access the ready list, they 
// are included in this file directly.
void mutex_init(struct mutex* mtx) {
  mtx->held = 0;
  mtx->waiting_threads.head = NULL;
  mtx->waiting_threads.tail = NULL;
}

void mutex_lock(struct mutex* mtx) {
  // If the mutex is not held, lock and continue continue
  if (mtx->held == 0) {
    mtx->held = 1;
  } else {		 
    current_thread->state = BLOCKED;
    thread_enqueue(&(mtx->waiting_threads), current_thread);
    yield();
  }
}

void mutex_unlock(struct mutex* mtx) {
  if (is_empty(&(mtx->waiting_threads))) {
    // No one is waiting -- free the mutex
    mtx->held = 0;
  } else {
    // Otherwise, transfer mutex control to the next thread
    struct thread* awoken = thread_dequeue(&(mtx->waiting_threads));
    awoken->state = READY;
    thread_enqueue(&ready_list, awoken);
  }
}


// Condition -- for condition synchronization.
void condition_init(struct condition* cond) {
  cond->waiting_threads.head = NULL;
  cond->waiting_threads.tail = NULL;
}

// Thread will wait on the condition until another thread SIGNALS it. 
// The thread is responsible for  rechecking the
// condition when it wakes up.
void condition_wait(struct condition* cond, struct mutex* mtx) {
  current_thread->state = BLOCKED;
  thread_enqueue(&(cond->waiting_threads), current_thread);
  mutex_unlock(mtx);
  yield();
  mutex_lock(mtx); // After waiting, the thread will still hold the mutex,
  // but should recheck the condition
}


// Will wake up the next available thread waiting on the condition COND.
// If no threads are waiting, this action does nothing.
void condition_signal(struct condition* cond) {

  if(is_empty(&(cond->waiting_threads)))
    return; // Do nothing if no one is waiting on cond
  
  // Otherwise, wake up the next thread.
  struct thread* next = thread_dequeue(&(cond->waiting_threads));
  next->state = READY;
  thread_enqueue(&ready_list, next);
}

// Wake up ALL threads waiting on cond.
void condition_broadcast(struct condition* cond) {
  struct thread* next;
  while(!is_empty(&(cond->waiting_threads))) {
    next = thread_dequeue(&(cond->waiting_threads));
    next->state = READY;
    thread_enqueue(&ready_list, next);
  }
}
