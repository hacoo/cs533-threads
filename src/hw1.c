// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 6 Apr. 2016
// 
// 533-threads/hw1.
// Functions, etc. for homework 1 -- context switching

#include "hw1.h"


int factorial(int n) {
  return (n <= 1) ? 1 : n * factorial(n-1);
}

// Basic function for playing with context switch
void fun_with_threads(void* arg) {
  int n = *(int*) arg; // dereference arg and cast
  printf("%d! = %d\n", n, factorial(n));
}

// For step 1
int call_clunky_thread_function(thread* _current_thread) {
  
  // create an initial argument
  int* p = malloc(sizeof(int));
  *p = 5;

  _current_thread = malloc(sizeof(thread));
  _current_thread->initial_function = fun_with_threads;  
  _current_thread->initial_argument = p;
  // make the call
  _current_thread->initial_function(_current_thread->initial_argument);

  return 0;
}


// For step 2 / 3 / 4
int try_to_switch_and_segfault_possibly() {
  int* p = malloc(sizeof(int));
  *p = 5;
  current_thread->initial_function = fun_with_threads;  
  current_thread->initial_argument = p;
  
  thread_start(inactive_thread, current_thread);

  return 0;
}


// 
