

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "thread.h"
#include "hw1.h"

int main() {

  //call_clunky_thread_function(current_thread);

  // malloc each thread
  current_thread = malloc(sizeof(thread));
  inactive_thread = malloc(sizeof(thread));

  // Current_thread will be switched to shortly -- so it needs
  // a stacko
  current_thread->stack_pointer = malloc(STACK_SIZE) + STACK_SIZE;
  
  
  try_to_switch_and_segfault_possibly();
  
  
  swtich_between_threads_a_bunch_of_times();
  return 0;
}


