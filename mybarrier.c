// 
// This is a VERY rudimentary way to handle thread
// barriers.
// 
#include <pthread.h>
#include "mybarrier.h"

// 
// barrier_init
//
// Initialize a barrier for use.
//
void barrier_init(mybarrier *barrier, int count) {
  barrier->threshold = barrier->counter = count;
  barrier->cycle = 0;
  pthread_mutex_init(&barrier->mutex, NULL);
  pthread_cond_init(&barrier->cv, NULL);
}

//
// barrier_destroy
//
// Destroy a barrier when done using it.
//
void barrier_destroy(mybarrier *barrier) {
  pthread_mutex_destroy(&barrier->mutex);
  pthread_cond_destroy(&barrier->cv);
}

//
// barrier_wait
//
// Wait for all members of a barrier to reach the barrier. When
// the count (of remaining members) reaches 0, release all threads
//
void barrier_wait(mybarrier *barrier) {
  int status, cancel, tmp, cycle;
  pthread_mutex_lock(&barrier->mutex);
  

  cycle = barrier->cycle;   
  
  // The thread entering is the last thread to be accounted for
  //
  if(--barrier->counter == 0) {
    
    // Broadcast our condition variable, release waiting
    // threads
    //
    barrier->cycle = !barrier->cycle;
    barrier->counter = barrier->threshold;
    pthread_cond_broadcast(&barrier->cv);
  
  // Thread entering is not the last thread
  //
  } else {
     
    // Wait with cancellation disabled, because barrier_wait
    // should not be a cancellation point. (This is one of
    // the things that I pulled from the other implementation
    // that I do not fully understand -- my understanding is
    // that this prevents any threads from ceasing execution.
    // Were this not to be here, waiting threads may cease
    // functioning.)
    //
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel);

    // Wait until the barrier's cycle changes, which means
    // that it has been broadcast, and we don't want to wait
    // anymore.
    //
    while (cycle == barrier->cycle) {
     
      // Once condition is signaled, break out of
      // while loop
      //
      status = pthread_cond_wait(&barrier->cv, &barrier->mutex);
      if (status != 0) break;
    } 
    // Reset the cancel state
    //
    pthread_setcancelstate(cancel, &tmp);
  }
  
  // Release threads
  //
  pthread_mutex_unlock(&barrier->mutex);
}
