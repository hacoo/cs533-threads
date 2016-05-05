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
  // Get the current offset into the file
  off_t offset;
  if (fd != STDIN_FILENO)
    offset = lseek(fd, 0, SEEK_CUR);
  else
    offset = 0;


  // Set up the request control block
  request.aio_fildes			= fd;
  request.aio_offset			= offset;
  request.aio_buf			= buf;
  request.aio_nbytes			= count;
  request.aio_reqprio			= 1; // not implemented yet
  request.aio_sigevent.sigev_notify	= SIGEV_NONE; // will poll for completion


  // Make the request
  int read_ret_code = aio_read(&request);
  int error_code;
  

  if (read_ret_code) {
    printf("ERROR -- aio_read failed with error code: %d\n", read_ret_code);
    errno = read_ret_code;
    return -1;
  }
  
  while (1) {
    error_code = aio_error(&request);

    if (error_code == 0) {
      // If read was succesful, seek the file forward and return
      lseek(fd, offset+count, SEEK_SET);
      return count;
    }
    if (error_code !=  EINPROGRESS) {
      if (error_code == ECANCELED) {
	printf("Request canceled. \n");
	errno = aio_return(&request);
	return -1;
      } else {
	printf("ERROR -- aio_read failed with error code: %d\n", error_code);
	errno = aio_return(&request);
	return -1;
      }      
    }
    // Else continue to poll
    yield();
  }
}