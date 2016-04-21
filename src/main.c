

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "scheduler.h"
#include "stdio.h"

void print_nth_prime(void * pn) {
  int n = *(int *) pn;
  int c = 1, i = 1;
  while(c <= n) {
    ++i;
    int j, isprime = 1;
    for(j = 2; j < i; ++j) {
      if(i % j == 0) {
        isprime = 0;
        break;
      }
    }
    if(isprime) {
      ++c;
    }
    yield();
  }
  printf("%dth prime: %d\n", n, i);
 
}


int main(void) {
  scheduler_begin();
  printf("Running nth prime. The 10000th prime should appear first, then the 20000th, then the 30000th. \n");
  int n1 = 20000, n2 = 10000, n3 = 30000;

  thread_fork(print_nth_prime, &n1);
  thread_fork(print_nth_prime, &n2);
  thread_fork(print_nth_prime, &n3);

  sleep(2);
  scheduler_end();
  return 0;
}

