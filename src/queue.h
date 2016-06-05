/*
 * CS533 Course Project
 * Thread Queue ADT
 * queue.h
 *
 * Feel free to modify this file. Please thoroughly comment on
 * any changes you make.
 */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_node queue_node;
struct queue_node {
  struct thread * t;
  struct queue_node * next;  
};

typedef struct queue queue;
struct queue {
  struct queue_node * head;
  struct queue_node * tail;
};

void thread_enqueue(queue * q, struct thread * t);
struct thread * thread_dequeue(queue * q);
int is_empty(queue * q);

#endif
