
#include "cqueue.h"

Cqueue _aperiodicQ = { .rear = -1, .front = -1};
Cqueue _sporadicQ = { .rear = -1, .front = -1};

void * aperiodicQ = (void *)&_aperiodicQ;
void * sporadicQ = (void *)&_sporadicQ;

/***************** QUEUE OPERATIONS *************************/

/* Insert a task into the queue*/ 
int queueInsert(Task * tsk, void * queue)
{
    Cqueue * q = (Cqueue *)queue; 
    
    if(q->front == (q->rear+1)%QUEUE_SIZE)
    {
        return -1; /* queue is FULL */
    }
    else
    {
        if(q->rear == -1)
        {
            q->front = q->rear = 0; 
        }
        else
        {
            q->rear = (q->rear+1)%QUEUE_SIZE;
        }
        
        q->arr[q->rear] = tsk;
    }
    
    return 0;
}

int isEmpty(void * queue)
{
    Cqueue * q = (Cqueue *)queue; 
    if(q->front == -1)
    {
        return 1;
    }
    
    return 0;
}

/* remove a task from the queue */ 
Task * queueRemove(void * queue)
{
    Task * item;
    Cqueue * q = (Cqueue *)queue;
    
    if(q->front == -1)
    {
        return 0;
    }
    else
    {
        item = q->arr[q->front];
        if(q->front == q->rear)
        {
            q->front = -1;
            q->rear = -1;
        }
        else
        {
            q->front = (q->front+1)%QUEUE_SIZE;
        }
    }
    
    return item;
    
}

/* return front of queue without actually removing it from the queue */
Task * queuePeek(void * queue)
{
    //Task * item;    
    Cqueue * q = (Cqueue *)queue;
    
    if(q->front == -1)
    {
        return 0;
    }
    else
    {
        return q->arr[q->front];
        
    }
    
}