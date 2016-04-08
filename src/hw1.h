// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 6 Apr. 2016
// 
// 533-threads/hw1.h
// 
// Functions, etc. for homework 1 -- context switching

#ifndef HW1_H
#define HW1_H

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "thread.h"

int factorial(int n);
void fun_with_threads(void* arg);
int call_clunky_thread_function(thread* current_thread);
int try_to_switch_and_segfault_possibly();


#endif
