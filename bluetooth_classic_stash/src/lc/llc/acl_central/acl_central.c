/**
 * @file acl.c
 * @author  (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#include "acl_central.h"
#include "btc_device.h"
#include "btc_rail.h"
#include "acl_central_sm_autogen.h"
#include "acl_common.h"
#include "lpw_scheduler.h"
#include "arqn_seqn_check.h"
#ifdef SIM
#include "rail_simulation.h"
#else
#include "btc_rail_intf.h"
extern shared_mem_t *ulc_llc_share_mem;
#endif

static void configure_and_schedule_tx(connection_info_t *connection_info_p)
{
  uint32_t status;
  sm_t *connection_info_sm_p = connection_info_p->connection_info_sm;
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  arqn_seqn_scheme_status_t *arqn_seqn_status = &connection_info_p->acl_arqn_seqn_info;

  // Handle multi-slot timing: TX scheduling always follows RX, so only check last_rx_slots
  if (connection_info_sm_p->last_rx_slots > 1) {
    uint32_t BtcDate;
    uint64_t ProtimerDate;
    RAIL_SEQ_BTC_GetLastRxDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, &BtcDate, &ProtimerDate);
    btc_trx_configs->btc_rail_trx_config.clk = BtcDate + (connection_info_sm_p->last_rx_slots * SLOT_WIDTH);
    connection_info_sm_p->last_rx_slots = 0;
  } else {
    configure_conn_slot_params(connection_info_p, BTC_BD_CLK_2NP0, btc_trx_configs);
  }

  packetization_acl(connection_info_p, btc_trx_configs); // TODO: Can be removed. Required for simulation tests
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
#ifndef SIM
  linear_buffer_t *linear_buffer_ptr = NULL;
  if ((linear_buffer_ptr = (linear_buffer_t *)get_tx_from_fifo_if_available()) != NULL  && arqn_seqn_status->remote_flow) {
    uint32_t len = linear_buffer_ptr->length;
    uint8_t *tx_pkt = (uint8_t *)linear_buffer_ptr->data;
    tx_packetization(connection_info_p, btc_trx_configs, tx_pkt, len);
  } else {
#endif
  if (arqn_seqn_status->arqn_status == ACK) {
    form_null_arqn_flow(btc_trx_configs, connection_info_p);
    // Set the arqn status to NAK for the next packet to convery that the current packet does not require an ACK
    arqn_seqn_status->arqn_status = NAK;
  } else {
    form_poll_and_null_pkt(btc_trx_configs, connection_info_p->lt_addr, BT_POLL_PKT_TYPE);
    // Set the arqn status to NAK for the next packet to convery that the current packet does not require an ACK
    arqn_seqn_status->arqn_status = NAK;
  }

#ifndef SIM
}
#endif
  update_encryption_parameters(connection_info_p, btc_trx_configs, true, CENTRAL_TO_PERIPHERAL);

  // Set last_tx_slots based on transmitted packet type (applies to both data and POLL/NULL packets)
  connection_info_sm_p->last_tx_slots = packet_type_slot_lookup_table[btc_trx_configs->btc_rail_trx_config.packetConfig.packetType][connection_info_p->br_edr_mode];

  status = schedule_tx(btc_trx_configs);
  SM_STATE(connection_info_sm_p, ACL_CENTRAL_TX_SCHEDULED);
  if (status != BTC_RAIL_PASS_STATUS) {
    SM_STEP(ACL_CENTRAL_SM, connection_info_sm_p, TX_ABORT);
  }
}

static void configure_and_schedule_rx(connection_info_t *connection_info_p)
{
  uint32_t status;
  sm_t *connection_info_sm_p = connection_info_p->connection_info_sm;
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;

  // Handle multi-slot timing: RX scheduling always follows TX, so only check last_tx_slots
  if (connection_info_sm_p->last_tx_slots > 1) {
    btc_trx_configs->btc_rail_trx_config.clk = btc_trx_configs->btc_rail_trx_config.clk + connection_info_sm_p->last_tx_slots * SLOT_WIDTH;
    connection_info_sm_p->last_tx_slots = 0;
  } else {
    configure_conn_slot_params(connection_info_p, BTC_BD_CLK_2NP2, btc_trx_configs);
  }

  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
  configure_trx_acl_rx(btc_trx_configs, connection_info_p->br_edr_mode, BTC_RAIL_RX_TIMEOUT);
  update_encryption_parameters(connection_info_p, btc_trx_configs, false, PERIPHERAL_TO_CENTRAL);
  btc_trx_configs->rx_hdr_needed = 1;
  status = schedule_rx(btc_trx_configs);
  SM_STATE(connection_info_sm_p, ACL_CENTRAL_WAITING_FOR_RX_PKT_HEADER);
  if (status != BTC_RAIL_PASS_STATUS) {
    SM_STEP(ACL_CENTRAL_SM, connection_info_sm_p, RX_ABORT);
  }
}

static void handle_tx_rx(connection_info_t *connection_info_p, bool is_tx)
{
  sm_t *connection_info_sm_p = connection_info_p->connection_info_sm;
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), connection_info_p->acl_info.acl_end_tsf)) {
    if (is_tx) {
      configure_and_schedule_tx(connection_info_p);
    } else {
      configure_and_schedule_rx(connection_info_p);
    }
  } else {
    procedure_activity_complete(connection_info_sm_p, acl_central_sm_idle);
  }
}

static void update_connection_info(connection_info_t *connection_info, uint8_t *hss_cmd)
{
  connection_info->priority = *HSS_CMD_START_ACL_CENTRAL_PRIORITY(hss_cmd);
  connection_info->step_size = *HSS_CMD_START_ACL_CENTRAL_STEP_SIZE(hss_cmd);
  connection_info->acl_info.acl_end_tsf = *HSS_CMD_START_ACL_CENTRAL_END_TSF(hss_cmd);
  memcpy(connection_info->bd_address, HSS_CMD_START_ACL_CENTRAL_BD_ADDRESS(hss_cmd), BD_ADDR_LEN);
  connection_info->acl_info.acl_end_tsf = btc_get_current_bd_clk_cnt(&connection_info->trx_config.bd_clk_counter_handle) + (connection_info->acl_info.acl_end_tsf * HALF_SLOT_FACTOR);
}

/**
 * @brief function is used to initialise acl parameters and is used to start the acl procedure.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void acl_central_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_START_ACL_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  connection_info->connection_info_sm = &acl_central_sm;
  SM_STATE(connection_info->connection_info_sm, acl_central_sm_idle);
  connection_info->acl_arqn_seqn_info.arqn_status = NAK; // Set the ARQN status as NAK
  connection_info->acl_arqn_seqn_info.seqn_status = 1; // Set the SEQN status as 1
  connection_info->acl_arqn_seqn_info.remote_seqn = 0; // Set to 0 before starting the ACL procedure
  connection_info->acl_arqn_seqn_info.remote_flow = 1; // Set to 1 before starting the ACL procedure
  memcpy(connection_info->sync_word, HSS_CMD_START_ACL_CENTRAL_SYNC_WORD(hss_cmd), SYNC_WORD_LEN);
  connection_info->connection_info_sm->current_device_index = *HSS_CMD_START_ACL_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd);

  btc_dev_p->current_slot_info.current_role_acl_lt_addr = connection_info->lt_addr;
  btc_dev_p->current_slot_info.current_role_lt_addr = connection_info->lt_addr;

  update_aperiodic_role(btc_dev_p, connection_info->connection_info_sm);
  update_connection_info(connection_info, hss_cmd);
  RAIL_SEQ_BTC_setAccessAddress(connection_info->sync_word);
  RAIL_SEQ_BTC_setHecInit(btc_dev_p->bd_addr[3]);
  RAIL_SEQ_BTC_setCrcInit(btc_dev_p->bd_addr[3]);
  SM_STEP(ACL_CENTRAL_SM, connection_info->connection_info_sm, START_PROCEDURE);
}

/**
 * @brief function is used to stop acl.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void stop_acl(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_STOP_ACL_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  procedure_activity_complete(connection_info->connection_info_sm, acl_central_sm_idle);
}

/**
 *@brief function handles transmission of tx packets for connection_info state machine.
 *@param connection_info_sm-structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow and acl_info, esco _info
 *
 *
 */
void acl_central_start_procedure_handler(sm_t *connection_info_sm_p)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm_p);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  btc_trx_configs->btc_rail_trx_config.txPower = connection_info_p->tx_pwr_index;
  btc_trx_configs->ignore_whitening = TRUE; // TODO: Remove this once hopping is properly tested in statemachines
  btc_trx_configs->sm = connection_info_p->connection_info_sm;
  btc_trx_configs->e0EncConfig = &connection_info_p->e0EncConfig;
  btc_trx_configs->aesEncConfig = &connection_info_p->aesEncConfig;
  // Initialize multi-slot counters
  connection_info_sm_p->last_tx_slots = 0;
  connection_info_sm_p->last_rx_slots = 0;

  rail_set_btc_clk_instance(&btc_trx_configs->bd_clk_counter_handle);
  update_channel_info_connected_state(connection_info_sm_p);
  configure_and_schedule_tx(connection_info_p);
}

/**
 * @brief function is used to open for reception to receive packets from peripheral.
 * @param connection_info-structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow and acl_info, esco _info
 */
void acl_central_tx_done_handler(sm_t *connection_info_sm_p)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm_p);
  handle_tx_rx(connection_info_p, false);
}

/**
 *@brief function handles transmission of tx packets for connection_info state machine after the transmission fails to be scheduled.
 *@param connection_info_sm-structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow and acl_info, esco _info
 *
 *
 */
void acl_central_tx_abort_handler(sm_t *connection_info_sm_p)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm_p);
  // Reset multi-slot counters since TX failed
  connection_info_sm_p->last_tx_slots = 0;
  handle_tx_rx(connection_info_p, true);
}

/**
 * @brief function is used to call functions to process rx header and trigger circular buffer
 * @param connection_info-structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow and acl_info, esco _info
 * @param
 *
 */
void acl_central_header_received_done_handler(sm_t *connection_info_sm_p)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm_p);
  arqn_stage_1_acl(connection_info_p);

  SM_STATE(connection_info_sm_p, ACL_CENTRAL_WAITING_FOR_RX_COMPLETE); // Updating state machine to CENTRAL_WAITING_FOR_RX_COMPLETE
}

void acl_central_rx_poll_null_done_handler(sm_t *connection_info_sm_p)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm_p);
  uint8_t status = arqn_stage_1_acl(connection_info_p);

  if (status) {
    arqn_stage_2_acl(connection_info_p);
  }
  handle_tx_rx(connection_info_p, true);
}

/**
 * @brief function schedules tx to transfer next packet, as the previous packet reception recieved and timeout or abort trigger.
 * @param connection_info_sm-structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow and acl_info, esco _info
 */
void acl_central_header_received_timeout_handler(sm_t *connection_info_sm_p)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm_p);
  arqn_seqn_scheme_status_t *arqn_seqn_status = &connection_info_p->acl_arqn_seqn_info;

  connection_info_sm_p->last_rx_slots = 0;
  arqn_seqn_status->arqn_status = NAK;
  handle_tx_rx(connection_info_p, true);
}

/**
 * @brief function schedules tx to transfer next packet and calls function to check if circular buffer triggered or not
 *
 * @param connection_info_sm-structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow and acl_info, esco _info
 */
void acl_central_rx_done_handler(sm_t *connection_info_sm_p)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm_p);

  arqn_stage_2_acl(connection_info_p);
  handle_tx_rx(connection_info_p, true);
}

/**
 * @brief function schedules transmission again.
 *
 * @param connection_info_sm-structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow and acl_info, esco _info
 */
void acl_central_rx_abort_handler(sm_t *connection_info_sm_p)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm_p);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  arqn_seqn_scheme_status_t *arqn_seqn_status = &connection_info_p->acl_arqn_seqn_info;

  // Reset multi-slot counters since RX failed
  connection_info_sm_p->last_tx_slots = 0;
  connection_info_sm_p->last_rx_slots = 0;

  arqn_seqn_status->arqn_status = NAK;
  btc_trx_configs->ignore_rx_pkt = 1; // To avoid DMA transfer for CRC/HEC failed packets
  handle_tx_rx(connection_info_p, true);
}
