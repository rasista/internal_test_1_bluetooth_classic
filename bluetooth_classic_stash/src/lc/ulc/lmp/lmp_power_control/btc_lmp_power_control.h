#ifndef _BTC_LMP_POWER_CONTROL_INTF_H
#define _BTC_LMP_POWER_CONTROL_INTF_H

#include "btc_dev.h"
#include "btc_hci.h"
#include "btc_hci_rx_pkt_handlers.h"

#define STEP_POWER_CHANGE 0
#define MIN_POWER 1
#define MAX_POWER 2

/*Enhanced Power Control Flags*/
#define DECR_PWR_ONE_STEP 0
#define INCR_PWR_ONE_STEP 1
#define INCR_TO_MAX_PWR   2

#define  PWR_CHANGE_STEP_SIZE 3

#define MIN_POWER_FOR_ALL_MODULATIONS        0x3F
#define MAX_POWER_FOR_ALL_MODULATIONS        0x2A
#define CHANGED_ONE_STEP_FOR_ALL_MODULATIONS 0x15

/* Vendor power control subevent notifications to intimate host about the LMP_RECEIVED as response */
#define VENDOR_POWER_CONTROL_RES_RX  1
#define VENDOR_POWER_CONTROL_RES_RX_MIN_POWER_REACHED 2
#define VENDOR_POWER_CONTROL_RES_RX_MAX_POWER_REACHED 3
#define VENDOR_POWER_CONTROL_MIN_POWER_RX 4
#define VENDOR_POWER_CONTROL_MAX_POWER_RX 5
#define VENDOR_POWER_CONTROL_POWER_INDEX_UPDATED 6


#ifdef SCHED_SIM

#define VENDOR_UPDATE_TEST_PACKET_PACKET_TYPE(pkb_data, pkt_type) \
  do { \
    pkb_data[META_DATA_LENGTH] &= ~(PACKET_TYPE_MASK << PACKET_TYPE_SHIFT); \
    pkb_data[META_DATA_LENGTH] |= ((pkt_type & PACKET_TYPE_MASK) << PACKET_TYPE_SHIFT); \
  } while (0)

#define VENDOR_UPDATE_TEST_PACKET_RSSI(pkb_data, rssi)   pkb_data[1] = rssi
void receive_done_callback(uint8_t *data, uint8_t length);
#endif

void update_lpw_with_new_power_index(ulc_t *ulc, uint8_t peer_id);
void lmp_power_control_req_tx(ulc_t *ulc, uint8_t peer_id);
void lmp_power_control_manager(ulc_t *ulc, uint8_t peer_id, int8_t rssi);
#endif
