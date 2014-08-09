#ifndef CQUEUE_H
#define CQUEUE_H

#include "tasks.h"

#define QUEUE_SIZE 20

/* APERIODIC and SPORADIC queues */
typedef struct
{
    Task * arr[QUEUE_SIZE];
    int rear;
    int front;
} Cqueue;

extern Cqueue _aperiodicQ;
extern Cqueue _sporadicQ;

extern void * aperiodicQ;
extern void * sporadicQ;

int queueInsert(Task * tsk, void * queue);

Task * queueRemove(void * queue);

int isEmpty(void * queue);

Task * queuePeek(void * queue);

#endif