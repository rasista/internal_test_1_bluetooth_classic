#include "btc.h"

btc_t btc = { .ulc = NULL };  // cleanup, need to check this in future or figure out a better way to initialize in review

void init_btc_pool(pkb_pool_t *pkb_pool,
                   pkb_chunk_size_t pkb_length,
                   pkb_chunk_type_t pkb_chunk_type,
                   uint32_t pkb_num_max,
                   uint32_t allocation_block_threshold)
{
  pkb_pool->pkb_queue.head = NULL;
  pkb_pool->pkb_queue.tail = NULL;
  pkb_pool->pkb_queue.size = 0;

#if SIM
  pkb_pool->pkb_queue.plt_lock.acquire_lock = btc_plt_acquire_lock;
  pkb_pool->pkb_queue.plt_lock.release_lock = btc_plt_release_lock;
#endif
  pkb_pool->pkb_length = pkb_length;
  pkb_pool->pkb_chunk_type = pkb_chunk_type;
  pkb_pool->pkb_num_max = pkb_num_max;
  pkb_pool->allocation_block_threshold = allocation_block_threshold;
  pkb_pool->pend_count = 0;

  btc_plt_create_lock(&pkb_pool->intra_allocation_type_lock);
  btc_plt_create_lock(&pkb_pool->inter_allocation_type_lock);
  btc_plt_create_pend(&pkb_pool->intra_allocation_type_pend);
  BTC_PRINT("pkb_pool: %p, pkb_length: %d, pkb_chunk_type: %d, pkb_num_max: %d, allocation_block_threshold: %d\n",
            pkb_pool, pkb_length, pkb_chunk_type, pkb_num_max, allocation_block_threshold);
}
void add_buffer_to_btc_pool(pkb_pool_t *pkb_pool, pkb_t *pkb)
{
  pkb->pkb_chunk_type = pkb_pool->pkb_chunk_type;
  enqueue(&pkb_pool->pkb_queue, &pkb->queue_node);
}
