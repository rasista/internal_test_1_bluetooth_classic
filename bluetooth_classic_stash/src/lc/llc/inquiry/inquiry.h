#ifndef inquiry_H_
#define inquiry_H_
#include "btc_common_defines.h"
#include "sl_btc_seq_interface.h"
#include "state_machine.h"
#include "btc_rail.h"
#ifdef SIM
#include "protocol_btc_rail.h"
#endif

#define BT_INQUIRY_HEC_INIT_VALUE     0x00
#define BT_INQUIRY_CRC_INIT_VALUE     0x00

typedef struct inquiry_s {
  uint8_t LAP_address[LAP_ADDR_LEN]; //  LAP ADDRESS derived form BD_ADDRESS
  uint8_t num_response; //  Number of Reponses that can be recieved from scanning
                        //  device[configurable]
  uint8_t tx_fail_status;  //  store the number of time TX failed
  uint8_t EIR_value;       //  stores the value of EIR passed from host side
  uint8_t priority;
  uint8_t step_size;
  uint8_t hci_trigger; //flag hci_trigger(inquiry)
  uint32_t end_tsf;
  uint32_t length; //  Inquiry procedure will run for this much
  //  length.[configurable]
  uint32_t inquiry_end_time; //  stores added value of current clk time and
                             //  inquiry length
  uint32_t inquiry_start_time;  //stores value when inquiry started for the first time from upper layer
  RAIL_SEQ_BtcFhConfig_t btc_fh_data;
  uint32_t sync_word[2];
  uint8_t kOffset;
  BtcSeqSelect_t seqSelect;
  sm_t *inquiry_sm;
  btc_trx_configs_t trx_config;
} inquiry_t;

void inquiry_id_1_tx_done_handler(sm_t *inquiry_sm);
void inquiry_id_1_tx_abort_handler(sm_t *inquiry_sm);
void inquiry_id_2_tx_done_handler(sm_t *inquiry_sm);
void inquiry_id_2_tx_abort_handler(sm_t *inquiry_sm);
void inquiry_rx_1_done_handler(sm_t *inquiry_sm);
void inquiry_rx_1_timeout_handler(sm_t *inquiry_sm);
void inquiry_rx_1_abort_handler(sm_t *inquiry_sm);
void inquiry_rx_1_handler(sm_t *inquiry_sm, uint8_t tx_slot, uint8_t rx_slot);
void inquiry_rx_2_done_handler(sm_t *inquiry_sm);
void inquiry_rx_2_timeout_handler(sm_t *inquiry_sm);
void inquiry_EIR_rx_done_handler(sm_t *inquiry_sm);
void inquiry_EIR_rx_timeout_handler(sm_t *inquiry_sm);
void inquiry_activity_complete(sm_t *inquiry_sm);
void update_channel_info_inquiry(btc_trx_configs_t *btc_trx_configs);

#endif
