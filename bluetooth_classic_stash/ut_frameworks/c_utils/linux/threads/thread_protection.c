//! file implements thread protection mechanism
#include "thread_protection.h"

//! Function to create Mutex lock
//! @param lock Mutex lock to create
//! @return 0 on success, -1 on failure
int btc_linux_create_lock(pthread_mutex_t *lock)
{
  if (pthread_mutex_init(lock, NULL) != 0) {
    return -1;
  }
  return 0;
}

//! Function to destroy Mutex lock
//! @param lock Mutex lock to destroy
//! @return 0 on success, -1 on failure
int btc_linux_destroy_lock(pthread_mutex_t *lock)
{
  if (pthread_mutex_destroy(lock) != 0) {
    return -1;
  }
  return 0;
}

//!function to acquire lock
//! @param lock Mutex lock to acquire
//! @return 0 on success, -1 on failure
int btc_linux_acquire_lock(pthread_mutex_t *lock)
{
  if (pthread_mutex_lock(lock) != 0) {
    return -1;
  }
  return 0;
}

//! Function to release lock
//! @param lock Mutex lock to release
//! @return 0 on success, -1 on failure
int btc_linux_release_lock(pthread_mutex_t *lock)
{
  if (pthread_mutex_unlock(lock) != 0) {
    return -1;
  }
  return 0;
}
//! Function to create condition variable
//! @param cond Condition variable to create
//! @return 0 on success, -1 on failure
int btc_linux_create_pend(btc_pend_t *pend)
{
  if (sem_init(pend, 0, 0) != 0) {
    return -1;
  }
  return 0;
}

//! Function to destroy condition variable
//! @param cond Condition variable to destroy
//! @return 0 on success, -1 on failure
int btc_linux_destroy_pend(btc_pend_t *pend)
{
  if (sem_destroy(pend) != 0) {
    return -1;
  }
  return 0;
}

//! function to add to pend list
//! @param pend_list Pend list to add to
//! @return 0 on success, -1 on failure
int btc_linux_add_to_pend(btc_pend_t *pend_list)
{
  btc_pend_t *pend = (btc_pend_t *)pend_list; // Cast to btc_pend_t *
  if (sem_wait(pend) != 0) {
    return -1;
  }
  return 0;
}

//! function to remove from pend list
//! @param pend_list Pend list to remove from
//! @return 0 on success, -1 on failure
int btc_linux_remove_from_pend(btc_pend_t *pend_list)
{
  btc_pend_t *pend = (btc_pend_t *)pend_list; // Cast to btc_pend_t *
  if (sem_post(pend) != 0) {
    return -1;
  }
  return 0;
}