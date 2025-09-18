#ifndef __PKB_MGMT_STRUCT_H__
#define __PKB_MGMT_STRUCT_H__
#include <stdint.h>
#include <stddef.h>
#include "btc_queue.h"
#include "debug.h"

//! Enum to list size of individual pkb chunks for different types
typedef enum {
  pkb_chunk_size_small = 80,
  pkb_chunk_size_medium = 320,
} pkb_chunk_size_t;
//! Enum to list different types of pkb chunks
typedef enum {
  pkb_chunk_type_small,
  pkb_chunk_type_medium,
  num_supported_pkb_chunk_type,
  pkb_chunk_type_force_size = 0xFFFFFFFF
} pkb_chunk_type_t;

typedef struct pkb_s {
  //! node to hold pkb in different queues
  node_t queue_node;
  //! node to continue scatter list
  node_t scatter_node;
  pkb_chunk_type_t pkb_chunk_type;
  uint16_t pkt_len;
  uint8_t data[0];
} pkb_t;

#endif
