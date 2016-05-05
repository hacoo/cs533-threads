#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "scheduler.h"
#include "stdio.h"
#include "errno.h"
#include "async.h"
#include "unistd.h"
#include "fcntl.h"

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

void read_file_test() {
  // First read asynchronously, then synchronously. You should see the same thing.

  printf("Reading asynchronously... \n");
  FILE* fp = fopen("test_file.txt", "r");

  if(!fp) {
    printf("ERROR -- fopen failed with code: %d\n", errno);
    return;
  }

  int fd = fileno(fp);
  char buf[124];
  memset(buf, 0, 124);

  int retval = read_wrap(fd, buf, 10);

  if (retval >= 0)
    printf("%s\n", buf);
  else {
    printf("ERROR -- read_wrap failed. Errno: %d\n", errno);
  }
  
  memset(buf, 0, 124);
  retval = read_wrap(fd, buf, 10);
  
  if (retval >= 0)
    printf("%s\n", buf);
  else {
    printf("ERROR -- read_wrap failed. Errno: %d\n", errno);
  }


  fclose(fp);
  
  printf("Now reading the normal way... \n");

  fp = fopen("test_file.txt", "r");

  if(!fp) {
    printf("ERROR -- fopen failed with code: %d\n", errno);
    return;
  }

  fd = fileno(fp);

  memset(buf, 0, 124);
  retval = read(fd, buf, 10);

  if (retval >= 0)
    printf("%s\n", buf);
  else {
    printf("ERROR -- read_wrap failed. Errno: %d\n", errno);
  }

  memset(buf, 0, 124);
  retval = read_wrap(fd, buf, 10);

  if (retval >= 0)
    printf("%s\n", buf);
  else {
    printf("ERROR -- read_wrap failed. Errno: %d\n", errno);
  }


  fclose(fp);
}

// Wait until something is entered on stdin, for testing async IO blocking
void wait_on_stdin() {
  char buf[124];
  memset(buf, 0, 124);
  printf("ENTER YOUR COMMAND! \n");
  read_wrap(STDIN_FILENO, buf, 10);
  printf("YOU ENTERED: %s\n", buf);
}


int main(void) {
  scheduler_begin();
  printf("Running nth prime. The 10000th prime should appear first, then the 20000th, then the 30000th. \n");
  int n1 = 200, n2 = 100, n3 = 300;

  thread_fork(wait_on_stdin, NULL);
  
  thread_fork(print_nth_prime, &n1);
  thread_fork(print_nth_prime, &n2);
  thread_fork(print_nth_prime, &n3);

  //thread_fork(read_file_test, NULL);
  sleep(2);

  scheduler_end();
  return 0;
}
