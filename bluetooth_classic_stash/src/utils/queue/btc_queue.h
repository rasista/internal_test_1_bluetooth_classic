#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>
#include <stddef.h>
#include "btc_plt_deps.h"
#if !(SIM || ULC_FW)
#include "rail_user_sequencer.h"
#include "rail_seq.h"
extern RAIL_Handle_t btc_rail_handle;
#endif
// Generic node structure
typedef struct node_s {
  struct node_s *next;  // pointer to the next node
} node_t;

// Generic lock structure
typedef struct lock_s {
  btc_lock_t lock;
  #if SIM || ULC_FW
  int (*acquire_lock)(btc_lock_t *lock);
  int (*release_lock)(btc_lock_t *lock);
  #else
  #if LLC_SEQUENCER
  uint32_t (*acquire_lock)(btc_lock_t *lock);
  uint32_t (*release_lock)(btc_lock_t *lock);
  #else
  uint32_t (*acquire_lock)(RAIL_Handle_t btc_rail_handle, btc_lock_t *lock);
  uint32_t (*release_lock)(RAIL_Handle_t btc_rail_handle, btc_lock_t *lock);
  #endif
  #endif
} btc_linux_lock_t;

// queue structure
typedef struct queue_s {
  node_t *head;  // pointer to the head of the queue
  node_t *tail;  // pointer to the tail of the queue
  uint32_t size;  // size of the queue // TODO: make it as volatile
  btc_linux_lock_t plt_lock;  // lock to protect the queue
} queue_t;

typedef struct {
  queue_t pending_queue;
  queue_t processed_queue;
  queue_t rx_precommit_buffers_queue;
  queue_t rx_event_buffers_queue;
  uint32_t metadata_size; //offset
  uint32_t tx_pkt;
  uint32_t tx_pkt_len;
} shared_mem_t;

#if SIM || ULC_FW
  #define RELEASE_LOCK(lock) queue->plt_lock.release_lock(&lock);
  #define ACQUIRE_LOCK(lock) queue->plt_lock.acquire_lock(&lock);
#else
#if LLC_SEQUENCER
  #define RELEASE_LOCK(lock) RAIL_SEQ_ReleaseSemaphore(&lock);
  #define ACQUIRE_LOCK(lock) RAIL_SEQ_LockSemaphore(&lock);
#else
  #define RELEASE_LOCK(lock) queue->plt_lock.release_lock(btc_rail_handle, &lock);
  #define ACQUIRE_LOCK(lock) queue->plt_lock.acquire_lock(btc_rail_handle, &lock);
#endif
#endif

//
/* Function to add to the queue*/
void enqueue(queue_t *queue, node_t *node);

// Function to remove from the queue
node_t *dequeue(queue_t *queue);

#endif
