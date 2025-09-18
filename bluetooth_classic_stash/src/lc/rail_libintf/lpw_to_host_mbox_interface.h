#ifndef LPW_TO_HOST_MBOX_INTF_H_
#define LPW_TO_HOST_MBOX_INTF_H_

#include "host_to_lpw_interface.h"
#include "pkb_mgmt.h"
#include "data_path.h"


typedef void (*hss_event_handler_t)(uint8_t *hss_event , uint32_t event_size);

typedef void (*process_rx_pkt_cb_t)(uint8_t *hss_event , uint32_t event_size);

uint8_t register_hss_event_cb(btc_internal_event_type_t event, hss_event_handler_t handler);
uint8_t register_pkt_rx_cb(process_rx_pkt_cb_t pkt_rx_handler);
void process_mbox_msg(shared_mem_t *shared_mem);

#endif
