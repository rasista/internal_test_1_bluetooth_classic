#ifndef __PKB_MGMT_H__
#define __PKB_MGMT_H__
#include <stdint.h>
#include <stddef.h>
#include "pkb_mgmt_struct.h"
#include "btc_plt_deps.h"

//! enum to list different allocation strategies
typedef enum {
  pkb_alloc_type_no_fail,
  pkb_alloc_type_block,
  num_supported_alloc_type,
} pkb_alloc_type;

//! Structure to hold pkb pools
typedef struct pkb_pool_s {
  queue_t pkb_queue;
  uint32_t pkb_num_max;
  uint32_t allocation_block_threshold;
  uint32_t pend_count;
  pkb_chunk_size_t pkb_length;
  pkb_chunk_type_t pkb_chunk_type;
  btc_pend_t intra_allocation_type_pend;
  btc_lock_t inter_allocation_type_lock;
  btc_lock_t intra_allocation_type_lock;
} pkb_pool_t;

void pkb_free(pkb_t *pkb, pkb_pool_t *pkb_pool);
uint32_t pkb_get_max_user_data_len(pkb_pool_t *pkb_pool);
pkb_t *pkb_alloc(pkb_pool_t *pkb_pool, pkb_alloc_type alloc_type);
void scatter_free(pkb_t *pkb, pkb_pool_t *pkb_pool);
#endif
