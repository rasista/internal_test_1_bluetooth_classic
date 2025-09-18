#include "btc_plt_deps.h"
#include "debug.h"
int btc_plt_create_lock(btc_lock_t* lock)
{
    #ifdef LINUX_UTILS
  return btc_linux_create_lock((pthread_mutex_t *)lock);
    #else
    (void)lock;
  return 0;
    #endif
}

int btc_plt_destroy_lock(btc_lock_t* lock)
{
    #ifdef LINUX_UTILS
  return btc_linux_destroy_lock((pthread_mutex_t *)lock);
    #else
    (void)lock;
  return 0;
    #endif
}

int btc_plt_acquire_lock(btc_lock_t* lock)
{
    #ifdef LINUX_UTILS
  return btc_linux_acquire_lock((pthread_mutex_t *)lock);
    #else
    (void)lock;
  return 0;
    #endif
}

int btc_plt_release_lock(btc_lock_t* lock)
{
    #ifdef LINUX_UTILS
  return btc_linux_release_lock((pthread_mutex_t *)lock);
    #else
    (void)lock;
  return 0;
    #endif
}

int btc_plt_create_pend(btc_pend_t* pend)
{
    #ifdef LINUX_UTILS
  return btc_linux_create_pend((btc_pend_t*)pend);
    #else
    (void)pend;
  return 0;
    #endif
}

int btc_plt_destroy_pend(btc_pend_t* pend)
{
    #ifdef LINUX_UTILS
  return btc_linux_destroy_pend((btc_pend_t*)pend);
    #else
    (void)pend;
  return 0;
    #endif
}

int btc_plt_add_to_pend(btc_pend_t* pend_list)
{
#ifdef LINUX_UTILS
  return btc_linux_add_to_pend((btc_pend_t*)pend_list);
#else
  (void)pend_list;
  return 0;
#endif
}

int btc_plt_remove_from_pend(btc_pend_t* pend_list)
{
#ifdef LINUX_UTILS
  return btc_linux_remove_from_pend((btc_pend_t*)pend_list);
#else
  (void)pend_list;
  return 0;
#endif
}

#ifdef RAIL_UTILS   // for future 
//!function to acquire lock
//! @param lock Mutex lock to acquire
//! @return 0 on success, -1 on failure
int rail_plt_acquire_lock(void *lock)
{
  (void)lock;
  return 0;
}

//! Function to release lock
//! @param lock Mutex lock to release
//! @return 0 on success, -1 on failure
int rail_plt_release_lock(void *lock)
{
  (void)lock;
  return 0;
}
#endif