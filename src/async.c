// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 3 May 2016
//
// 533-threads/asnc.
//
// Allows threads to perform blocking IO by polling on an asychronous
// UNIX IO call

#include "async.h"

// Implements a blocking thread IO call based on polling an asynchronous read.
ssize_t read_wrap (int fd, void* buf, size_t count) {

  struct aiocb request;

  // Set up the request control block
  request.aio_fildes			= fd;
  request.aio_offset			= 0; // not implemented yet
  request.aio_buf			= buf;
  request.aio_nbytes			= count;
  request.aio_reqprio			= 1; // not implemented yet
  request.aio_sigevent.sigev_notify	= SIGEV_NONE; // will poll for completion

  // Make the request
  int read_ret_code = aio_read(&request);

  if (read_ret_code) {
    fprintf(stderr, "ERROR -- aio_read failed with error code: %d\n", read_ret_code);
  }
  
  return 0;
}
