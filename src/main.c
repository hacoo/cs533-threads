

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "thread.h"
#include "hw1.h"

thread* current_thread;
thread* new_thread;


int main() {

  //call_clunky_thread_function(current_thread);
  
  current_thread = malloc(sizeof(thread)); 
  new_thread = malloc(sizeof(thread)); 

  int x = 0;
  int y = 1;



  thread_switch(current_thread, new_thread);

  return 0;
}
