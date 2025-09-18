//! File provides utilities to create a destroy threads
//! @file thread.c
//! @brief Thread utilities

#include "thread.h"

//! Function to create a thread
//! @param thread Thread to create
//! @param function Function to run in the thread
//! @param arg Argument to pass to the function
//! @return 0 on success, -1 on failure
int thread_create(pthread_t *thread, void *(*function)(void *), void *arg)
{
    if (pthread_create(thread, NULL, function, arg) != 0)
    {
        return -1;
    }
    return 0;
}

//! Function to destroy a thread
//! @param thread Thread to destroy
//! @return 0 on success, -1 on failure
int thread_destroy(pthread_t thread)
{
    if (pthread_cancel(thread) != 0)
    {
        return -1;
    }
    return 0;
}

//! Function to join a thread
//! @param thread Thread to join
//! @return 0 on success, -1 on failure
int thread_join(pthread_t thread)
{
    if (pthread_join(thread, NULL) != 0)
    {
        return -1;
    }
    return 0;
}

int __attribute__((weak)) main()
{
    pthread_t thread;
    thread_create(&thread, NULL, NULL);
    thread_destroy(thread);
    thread_join(thread);
    return 0;
}
