#include <pthread.h>

#ifndef _BARRIER_H
#define _BARRIER_H

typedef struct {
    pthread_mutex_t     mutex;          // control access to barrier 
    pthread_cond_t      cv;             // wait for barrier
    int                 threshold;      // number of threads required 
    int                 counter;        // current number of threads 
    int                 cycle;          // alternate wait cycles (0 or 1) 
} mybarrier;

void barrier_init (mybarrier *barrier, int count);
void barrier_destroy (mybarrier *barrier);
void barrier_wait (mybarrier *barrier);

#endif
