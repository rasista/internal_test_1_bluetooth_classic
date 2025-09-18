#include "stdint.h"
#include <stdio.h>
#include "pthread.h"
#include "sl_btdm_logging.h"
#include "sl_btdm_log_autogen.h"
#include "time.h"

void *fast_log_producer(void * ctx);
void *fast_log_consumer(void * ctx);
void *log_consumer(void *ctx);
sl_btdm_fast_log_entry_t log_entries[35];
sl_btdm_logging_meta_data_t logging_metadata;

extern uint32_t plt_get_timestamp();

sl_btdm_log_db_t sl_btdm_log_db;

//! global lock for fast logging buffer
pthread_mutex_t fast_logging_lock;

uint32_t plt_get_timestamp()
{
    //! get timestamp in micro seconds
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

void plt_enter_critical()
{
    pthread_mutex_lock(&fast_logging_lock);
}

void plt_exit_critical()
{
    pthread_mutex_unlock(&fast_logging_lock);
}

int main()
{
    pthread_t fast_log_producer_thread;
    pthread_t fast_log_consumer_thread;

    //! initalize lock
    pthread_mutex_init(&fast_logging_lock, NULL);

    sl_btdm_fast_logging_init(log_entries, &logging_metadata, 35);
    
    //! Create a task to produce the fast_log messages
    pthread_create(&fast_log_producer_thread, NULL, fast_log_producer, NULL);
    //! Create a task to consume the fast_log messages
    pthread_create(&fast_log_consumer_thread, NULL, fast_log_consumer, NULL);
    //! Wait for the fast_log producer to finish
    pthread_join(fast_log_producer_thread, NULL);
    return 0;
}