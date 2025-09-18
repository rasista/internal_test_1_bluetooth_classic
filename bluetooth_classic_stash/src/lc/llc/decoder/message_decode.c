#include "message_decode.h"
#include "sl_host_to_lpw_interface.h"
#include "btc_device.h"
#include "lpw_scheduler.h"
#include "inquiry.h"
#include "inquiry_scan.h"
#include "page.h"
#include "page_scan.h"
#include "acl_central.h"
#include "acl_peripheral.h"
#include "roleswitch_central.h"
#include "roleswitch_peripheral.h"
#include "esco.h"
#ifdef COMPILE_TEST_MODE
#include "test_mode.h"
#endif
#ifndef SIM
#include "rail_seq.h"
#else
#include "rail_simulation.h"
#endif

#ifndef SIM
extern shared_mem_t *ulc_llc_share_mem;
#endif

const hss_cmd_start_procedure_lut_t hss_cmd_start_proc_lut[] = {
  { init_btc_device },
#ifndef RUN_TEST_MODE
  { start_page },
  { start_page_scan },
  { start_inquiry },
  { start_inquiry_scan },
  { acl_central_start_connection },
  { acl_peripheral_start_connection },
  { esco_central_start_connection },
  { esco_peripheral_start_connection },
  { roleswitch_central_start_connection },
  { roleswitch_peripheral_start_connection },
#else
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { start_test_mode },
#endif
};

const hss_cmd_set_procedure_lut_t hss_cmd_set_proc_lut[] = {
#ifndef RUN_TEST_MODE
  { set_page },
  { set_page_scan },
  { set_inquiry },
  { set_inquiry_scan },
  { set_afh_parameters_acl },
  { set_sniff_parameters_acl },
  { set_device_parameters_acl },
  { set_device_parameters_esco },
  { set_device_parameters_roleswitch },
  { set_roleswitch_parameters },
  { set_encryption_parameters },
#else
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { set_test_mode_encryption },
#endif
  { set_fixed_btc_channel },
};

const hss_cmd_stop_procedure_lut_t hss_cmd_stop_proc_lut[] = {
#ifndef RUN_TEST_MODE
  { stop_page },
  { stop_inquiry },
  { stop_page_scan },
  { stop_inquiry_scan },
  { stop_acl },
  { stop_acl_peripheral },
  { stop_roleswitch },
  { stop_roleswitch_peripheral },
  { NULL },
  { stop_esco },
  { stop_esco_peripheral },
#else
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
  { NULL },
#endif
};

//TODO: change signature of get lut to remove the passing of unused variables
const hss_cmd_get_procedure_lut_t hss_cmd_get_proc_lut[] = {
  { get_roleswitch_parameter_slot_offset },
};

const hss_cmd_type_lut_t hss_cmd_type_lut[] = {
  { handle_hss_cmd_start_procedure }, { handle_hss_cmd_stop_procedure },
  { handle_hss_cmd_set_procedure }, { handler_hss_cmd_get_procedure }
};

/**
 * @brief this function extracts the cmd_id and call the start handlers of the
 * respective state machines on the basis of cmd_id
 *
 * @param hss_cmd
 */
void handle_hss_cmd_start_procedure(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  (void)hss_cmd;
  uint8_t start_procedure_cmd_id = *HSS_CMD_PAYLOAD_START_PROCEDURE_ID(hss_cmd);
  if (start_procedure_cmd_id < max_hss_cmd_lpw_procedures) {
    hss_cmd_start_proc_lut[start_procedure_cmd_id].handler(hss_cmd, btc_dev_p);
  } else {
    BTC_ASSERT(INVALID_CMD_ID);
  }
}

/**
 * @brief for future use , will call the stop handlers of the respective state
 * machines on the basis of cmd_id
 *
 * @param hss_cmd
 */
void handle_hss_cmd_stop_procedure(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint8_t stop_procedure_cmd_id = *HSS_CMD_PAYLOAD_STOP_PROCEDURE_ID(hss_cmd);
  if (stop_procedure_cmd_id < max_hss_cmd_stop_procedures_aperiodic) {
    hss_cmd_stop_proc_lut[stop_procedure_cmd_id].handler(hss_cmd, btc_dev_p);
  } else if (stop_procedure_cmd_id < max_hss_cmd_stop_procedures_periodic) {
    hss_cmd_stop_proc_lut[stop_procedure_cmd_id].handler(hss_cmd, btc_dev_p);
  } else {
    BTC_ASSERT(INVALID_CMD_ID);
  }
}

/**
 * @brief for future use , will call the set handlers of the respective state
 * machines on the basis of cmd_id
 *
 * @param hss_cmd
 */
void handle_hss_cmd_set_procedure(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint8_t set_procedure_cmd_id = *HSS_CMD_PAYLOAD_SET_PROCEDURE_ID(hss_cmd);
  if (set_procedure_cmd_id < max_hss_cmd_set_procedures) {
    hss_cmd_set_proc_lut[set_procedure_cmd_id].handler(hss_cmd, btc_dev_p);
  } else {
    BTC_ASSERT(INVALID_CMD_ID);
  }
}

/**
 * @brief for future use , will call the get handlers of the respective state
 * machines on the basis of cmd_id
 *
 * @param hss_cmd
 */
void handler_hss_cmd_get_procedure(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint8_t get_procedure_cmd_id = *HSS_CMD_PAYLOAD_GET_PROCEDURE_ID(hss_cmd);
  if (get_procedure_cmd_id < max_hss_cmd_get_procedures) {
    hss_cmd_get_proc_lut[get_procedure_cmd_id].handler(hss_cmd, btc_dev_p);
  } else {
    BTC_ASSERT(INVALID_CMD_ID);
  }
}

/**
 * @brief decode handler  , which decodes the hss_message and calls the
 * respective start, stop, set,get hanlders on the basis of cmd_type
 *
 * @param hss_cmd
 */
void hss_msg_decode(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint8_t command_type = *HSS_CMD_CMD_TYPE(hss_cmd);
  if (command_type < max_hss_cmd_types) {
    hss_cmd_type_lut[command_type].handler(hss_cmd, btc_dev_p);
  } else {
    BTC_ASSERT(INVALID_CMD_TYPE);
  }
  return;
}

/**
 * @brief lpw_mailbox_handler, which dequeues the events from LPW
 * command pending queue and processes it. After processing, enqueue
 * the same event into LPW command processed queue to be further
 * processed by stack.
 *
 * @param queue
 * @param btc_dev_p
 */
void lpw_mailbox_handler(shared_mem_t *queue, btc_dev_t *btc_dev_p)
{
  while (queue->pending_queue.size) {
    pkb_t *hss_cmd_data = (pkb_t *)dequeue(&queue->pending_queue);
    uint8_t *hss_cmd = (uint8_t *)((uint8_t *)hss_cmd_data + queue->metadata_size);
    btc_rx_meta_data_t rx_meta_data;
    hss_msg_decode(hss_cmd, btc_dev_p);
#ifndef SIM
    // Enqueue the event into processed queue
    rx_meta_data.rx_pkt_meta_data.event_type = BTC_LLC_TO_ULC_CMD_PROCESSED_EVENT;
    *((uint32_t *)((uint8_t *)hss_cmd_data + ulc_llc_share_mem->metadata_size)) = rx_meta_data.data;
#else
    (void)rx_meta_data;
#endif
    enqueue(&queue->processed_queue, &hss_cmd_data->queue_node);
    RAIL_SEQ_SendMbox(BTC_LLC_TO_ULC_CMD_PROCESSED_EVENT);
  }
}
