// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 6 Apr. 2016
//
// 533-threads/thread.
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
void thread_fork(void(*target)(void*), void* arg) {
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
}


// Yield to the next available thread
void yield() {
  // If the current thread isn't done, swap it.
  if (current_thread->state != DONE) {
    
    // Save old thread in queue
    thread* old = current_thread;
    old->state = READY;    
    thread_enqueue(&ready_list, old);
    
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
    thread* old = current_thread;
    thread* new = thread_dequeue(&ready_list);
    current_thread = new;
    new->state = READY;
    thread_switch(old, new);
  }  
}



