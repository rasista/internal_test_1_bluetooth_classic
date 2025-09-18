#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>
#include <stdio.h>
//! Function to create a thread
//! Function to create a thread
//! @param thread Thread to create
//! @param function Function to run in the thread
//! @param arg Argument to pass to the function
//! @return 0 on success, -1 on failure
int thread_create(pthread_t *thread, void *(*function)(void *), void *arg);

//! Function to destroy a thread
//! @param thread Thread to destroy
//! @return 0 on success, -1 on failure
int thread_destroy(pthread_t thread);

//! Function to join a thread
//! @param thread Thread to join
//! @return 0 on success, -1 on failure
int thread_join(pthread_t thread);
#endif