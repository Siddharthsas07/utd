#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <pthread.h>
#include "NaiveConcurrentQueue.h"
#include "network_constructs.h"
#include "thread_constructs.h"

// The single calculation thread ID
extern thread_t           calc_thread_id;

// Calculator spawn calculation thread and begin calculation
int calc_start();

// Thread entry functions
void* calc_thread_start( void* data );

// Wait for the calculation to complete
void calc_to_completion();

// Calculator interaction functions
int calc_stop();

#endif