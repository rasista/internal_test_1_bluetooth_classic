#include "lpw_to_host_mbox_interface.h"

extern pkb_pool_t pkb_pool_g[num_supported_pkb_chunk_type];
uint32_t rx_pkt[1088 / 4]; // Max size of a packet is 1024 + 64 bytes of metadata
hss_event_handler_t hss_event_cb_lut[BTC_MAX_INTERNAL_EVENT_TYPES];
process_rx_pkt_cb_t process_rx_pkt_cb;

void parse_internal_event(uint8_t *hss_event , uint32_t event_size)
{
  uint8_t event_type = *HSS_EVENT_EVENT_TYPE(hss_event);
  hss_event_cb_lut[event_type](hss_event, event_size);
}

void process_mbox_msg(shared_mem_t *shared_mem)
{
  pkb_t* pkb =  NULL;
  btc_rx_meta_data_t *rx_metadata = NULL;
  uint8_t event_type = 0;
  uint8_t *hss_event;
  // Process regular mailbox messages
  while (shared_mem->processed_queue.size) {
    pkb = (pkb_t *)dequeue(&shared_mem->processed_queue);
    rx_metadata = (btc_rx_meta_data_t *)((uint8_t *)pkb + shared_mem->metadata_size);
    event_type = rx_metadata->rx_pkt_meta_data.event_type;
    pkb_free(pkb, &pkb_pool_g[pkb->pkb_chunk_type]);
  }
  while (shared_mem->rx_event_buffers_queue.size) {
    pkb = (pkb_t *)dequeue(&shared_mem->rx_event_buffers_queue);
    rx_metadata = (btc_rx_meta_data_t *)((uint8_t *)pkb + shared_mem->metadata_size);
    event_type = rx_metadata->rx_pkt_meta_data.event_type;
    if (event_type == BTC_LLC_TO_ULC_DMA_TRANSFER_DONE) {
      // Handle scatter-gather DMA transfer with multiple PKBs using static buffer
      uint32_t total_packet_size = 0;
      uint32_t offset = 0;
      pkb_t *current_pkb = pkb;
      
      // Single pass: copy actual packet data (skip reserved metadata space)
      while (current_pkb != NULL) {
        uint8_t *pkb_data = ((uint8_t *)current_pkb + shared_mem->metadata_size);
        uint32_t data_size = current_pkb->pkt_len;
        
        if (current_pkb == pkb) {
          // First PKB: skip BTC_LPW_RX_METADATA_SIZE reserved space
          pkb_data += BTC_LPW_RX_METADATA_SIZE;
          data_size -= BTC_LPW_RX_METADATA_SIZE;
        }
        
        // Copy actual packet data (excluding reserved metadata space)
        memcpy((uint8_t *)rx_pkt + offset, pkb_data, data_size);
        offset += data_size;
        total_packet_size += data_size;
        
        current_pkb = (pkb_t *)current_pkb->scatter_node.next;
      }
      
      // Process complete packet from rx_pkt buffer (clean data without reserved space)
      process_rx_pkt_cb((uint8_t *)rx_pkt, total_packet_size);
    } else if (event_type == BTC_LLC_TO_ULC_INTERNAL_EVENT) {
      hss_event = ((uint8_t *)pkb + shared_mem->metadata_size + BTC_LPW_RX_METADATA_SIZE);
      parse_internal_event(hss_event , (pkb->pkt_len - BTC_LPW_RX_METADATA_SIZE));
    } else {
    }
    
    // Free all PKBs in the scatter-gather chain
    pkb_t *current_pkb = pkb;
    while (current_pkb != NULL) {
      pkb_t *next_pkb = (pkb_t *)current_pkb->scatter_node.next;
      enqueue(&shared_mem->rx_precommit_buffers_queue, &current_pkb->queue_node);
      current_pkb = next_pkb;
    }
  }
  // reset the riscv address in the FIFO instance
  for (uint8_t i = 0; i < fifo_manager.active_instances; i++) {
    reset_riscv_addr(i);
  }
}

uint8_t register_hss_event_cb(btc_internal_event_type_t event, hss_event_handler_t handler)
{
  if ((handler != NULL) && (event < BTC_MAX_INTERNAL_EVENT_TYPES)) {
    hss_event_cb_lut[event] = handler;
    return 1;
  }
  return 0;
}

uint8_t register_pkt_rx_cb(process_rx_pkt_cb_t pkt_rx_handler)
{
  if (pkt_rx_handler != NULL) {
    process_rx_pkt_cb = pkt_rx_handler;
    return 1;
  }
  return 0;
}

