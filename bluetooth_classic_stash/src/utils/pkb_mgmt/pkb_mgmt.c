#include "pkb_mgmt.h"
#include "btc_plt_deps.h"
#ifdef MONITORED_BY_VALGRIND
#include "valgrind/memcheck.h"
#endif
/***
 * pkb_alloc_no_fail is internal function to allocate packet buffers from pkb_pool.
 * if the pkb_pool is empty, it asserts.
 */
static pkb_t *pkb_alloc_no_fail(pkb_pool_t *pkb_pool)
{
  btc_plt_acquire_lock(&pkb_pool->inter_allocation_type_lock);
  //! check that pkb_queue is not empty
  if (pkb_pool->pkb_queue.size == 0) {
    BTC_ASSERT(QUEUE_EMPTY_ERROR);
  }
  //! allocate pkb from pkb_pool
  pkb_t *pkb = (pkb_t *)dequeue(&pkb_pool->pkb_queue);
  btc_plt_release_lock(&pkb_pool->inter_allocation_type_lock);
  return pkb;
}
/***
 * pkb_alloc_block is internal function to allocate packet buffers from pkb_pool.
 * if the pkb_pool is empty, it blocks until a pkb is available.
 */
static pkb_t *pkb_alloc_block(pkb_pool_t *pkb_pool)
{
  //! intra_alloc_type lock is required to ensure that only one thread is
  //! pending on pkb_pool this avoids the complexity around multiple threads
  //! registering a pend on pkb_pool
  btc_plt_acquire_lock(&pkb_pool->intra_allocation_type_lock);

  //! inter_alloc_type lock  is required to ensure pkb_queue size is
  //! not modified due to allocations by pkb_alloc_no_fail
  btc_plt_acquire_lock(&pkb_pool->inter_allocation_type_lock);

  //! block until pkb is below threshold
  while (pkb_pool->pkb_queue.size < pkb_pool->allocation_block_threshold) {
    pkb_pool->pend_count++;
    //! Release lock so that pkb_alloc_no_fail can allocate pkb
    btc_plt_release_lock(&pkb_pool->inter_allocation_type_lock);
    //! Add to pend list, this will cause the thread to block until pkb is available
    btc_plt_add_to_pend(&pkb_pool->intra_allocation_type_pend);
    //! acquire lock so that pkb_alloc_no_fail cannot allocate pkb
    btc_plt_acquire_lock(&pkb_pool->inter_allocation_type_lock);
  }
  //! allocate pkb from pkb_pool
  pkb_t *pkb = (pkb_t *)dequeue(&pkb_pool->pkb_queue);

  //! release inter_alloc_type lock
  btc_plt_release_lock(&pkb_pool->inter_allocation_type_lock);
  //! release intra_alloc_type lock
  btc_plt_release_lock(&pkb_pool->intra_allocation_type_lock);
  return pkb;
}
/***
 * pkb_alloc is a generic function used to allocate packet buffers across all layers.
 * A packet can be classified as per two categrizations:
 * 1. Packet from specific pool as per the packet size.
 * 2. Packet allocation strategy.
 * pkb_alloc api picks the pkb pool and attempts packet allocation using the strategy.
 */
pkb_t *pkb_alloc(pkb_pool_t *pkb_pool, pkb_alloc_type alloc_type)
{
  pkb_t *pkb = NULL;
  //! check that allocation type is valid
  if (alloc_type >= num_supported_alloc_type) {
    BTC_ASSERT(INVALID_PARAM);
  }
  //! pick pkb_pool by pkb_chunk_size
  if (alloc_type == pkb_alloc_type_no_fail) {
    pkb = pkb_alloc_no_fail(pkb_pool);
  } else {
    pkb = pkb_alloc_block(pkb_pool);
  }
#ifdef MONITORED_BY_VALGRIND
  VALGRIND_MALLOCLIKE_BLOCK(pkb, pkb_pool->pkb_length, 0, 0);
#endif
  pkb->queue_node.next = NULL;
  pkb->scatter_node.next = NULL;
  pkb->pkb_chunk_type = pkb_pool->pkb_chunk_type;

  return pkb;
}

/**
 * pkb_free is a generic function used to free packet buffers across all layers.
 *
 */
void pkb_free(pkb_t *pkb, pkb_pool_t *pkb_pool)
{
  //! check that pkb is not NULL
  if (pkb == NULL) {
    BTC_ASSERT(NULL_POINTER);
  }
  //! pick pkb_pool by pkb_chunk_size
  //! add pkb to pkb_pool
  enqueue(&pkb_pool->pkb_queue, &pkb->queue_node);
#ifdef MONITORED_BY_VALGRIND
  VALGRIND_FREELIKE_BLOCK(pkb, 0);
#endif

  //! inter_alloc_type lock  is required to ensure pkb_queue size is
  //! not modified due to allocations by pkb_alloc_no_fail
  btc_plt_acquire_lock(&pkb_pool->inter_allocation_type_lock);
  if (pkb_pool->pend_count > 0) {
    //! remove from pend list
    btc_plt_remove_from_pend(&pkb_pool->intra_allocation_type_pend);
    //! pend count should be decremented only if there is a thread pending
    //! it is decremented while free and not when the allocation is resumes
    //! so that add and remove from pend are in sync
    pkb_pool->pend_count--;
  }
  //! release inter_alloc_type lock
  btc_plt_release_lock(&pkb_pool->inter_allocation_type_lock);
}

//! function returns the max user data len a pkb can hold depending upon the pkb_chunk_type present in pkb_pool
uint32_t pkb_get_max_user_data_len(pkb_pool_t *pkb_pool)
{
  //! pick pkb_pool by pkb_chunk_size
  return pkb_pool->pkb_length - sizeof(pkb_t);
}

//! function to free scatter list
void scatter_free(pkb_t *pkb, pkb_pool_t *pkb_pool)
{
  pkb_t *scatter_pkb = pkb;
  while (scatter_pkb != NULL) {
    pkb_t *next_scatter_pkb = (pkb_t *)scatter_pkb->scatter_node.next;
    BTC_PRINT("scatter_free: %d\n", scatter_pkb);
    scatter_pkb->scatter_node.next = NULL;    //! This is important to avoid memory leaks
    pkb_free(scatter_pkb, pkb_pool);
    scatter_pkb = next_scatter_pkb;
  }
}
