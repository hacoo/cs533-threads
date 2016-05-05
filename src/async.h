// Henry Cooney <email: hacoo36@gmail.com> <Github: hacoo>
// 3 May 2016
//
// 533-threads/asnc.
//
// Allows threads to perform blocking IO by polling on an asychronous
// UNIX IO call


#ifndef ASYNC_H
#define ASYNC_H

#include <aio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "scheduler.h"

ssize_t read_wrap (int fd, void* buf, size_t count);

#endif
