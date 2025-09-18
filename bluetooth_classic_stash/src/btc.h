#ifndef __BTC_H__
#define __BTC_H__
#include "pkb_mgmt.h"

typedef struct btc_s {
    void *ulc;    // cleanup, is void * correct?
    pkb_pool_t pkb_pool[num_supported_pkb_chunk_type];
} btc_t;

extern btc_t btc;

void init_btc_pool(pkb_pool_t *pkb_pool, 
    pkb_chunk_size_t pkb_length,
    pkb_chunk_type_t pkb_chunk_type, 
    uint32_t pkb_num_max, 
    uint32_t allocation_block_threshold);
void add_buffer_to_btc_pool(pkb_pool_t *pkb_pool, pkb_t *pkb);
void killFirmware();
#endif