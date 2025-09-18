#ifndef __BTC_PLT_DEPS_H__
#define __BTC_PLT_DEPS_H__
#ifdef LINUX_UTILS
#include <pthread.h>
#include <semaphore.h>
typedef pthread_mutex_t btc_lock_t;
typedef sem_t btc_pend_t;
#else
#include <stdint.h>
typedef uint32_t btc_lock_t;
typedef uint32_t btc_pend_t;
#endif

#ifdef LINUX_UTILS
int btc_linux_create_lock(pthread_mutex_t *lock);
int btc_linux_destroy_lock(pthread_mutex_t *lock);
int btc_linux_acquire_lock(pthread_mutex_t *lock);
int btc_linux_release_lock(pthread_mutex_t *lock);
int btc_linux_create_pend(btc_pend_t *cond);
int btc_linux_destroy_pend(btc_pend_t *cond);
int btc_linux_add_to_pend(btc_pend_t *pend_list);
int btc_linux_remove_from_pend(btc_pend_t *pend_list);
#endif

int btc_plt_create_lock(btc_lock_t* lock);
int btc_plt_destroy_lock(btc_lock_t* lock);
int btc_plt_acquire_lock(btc_lock_t* lock);
int btc_plt_release_lock(btc_lock_t* lock);
int btc_plt_create_pend(btc_pend_t* pend);
int btc_plt_destroy_pend(btc_pend_t* pend);
int btc_plt_add_to_pend(btc_pend_t* pend_list);
int btc_plt_remove_from_pend(btc_pend_t* pend_list);

#ifdef RAIL_UTILS   // for future
int rail_plt_acquire_lock(void *lock);
int rail_plt_release_lock(void *lock);
#endif
#endif 
