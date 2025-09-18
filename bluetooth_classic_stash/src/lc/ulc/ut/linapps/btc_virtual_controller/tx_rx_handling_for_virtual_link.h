#ifndef RX_PKT_FROM_VIRTUAL_LINK_H
#define RX_PKT_FROM_VIRTUAL_LINK_H
#include "btc_hci_rx_pkt_handlers.h"
#include <stdint.h>

#define ON_THE_AIR_PKT_TYPE 0x01
void receive_done_callback(uint8_t *data, uint8_t length);
void virtual_tx_done_handler(void *ctx);

#endif
