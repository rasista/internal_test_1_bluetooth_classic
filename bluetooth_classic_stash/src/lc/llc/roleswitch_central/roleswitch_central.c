/**
 * @file roleswitch_central.c
 * @author  (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#include "roleswitch_central.h"
#include "btc_device.h"
#include "btc_rail.h"
#include "acl_common.h"
#include "roleswitch_central_sm_autogen.h"
#include "lpw_to_host_interface.h"
#include "lpw_scheduler.h"
#ifdef SIM
#include "rail_simulation.h"
#else
#include "btc_rail_intf.h"
#endif

/**
 * @brief Helper function to configure slot params and update channel info
 */
static void configure_and_update_channel(connection_info_t *connection_info_p,
                                         btc_trx_configs_t *btc_trx_configs,
                                         uint8_t clk_offset)
{
  configure_conn_slot_params(connection_info_p, clk_offset, btc_trx_configs);
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
}

/**
 * @brief Helper function to check clock and configure common parameters
 * @return true if clock check passes, false otherwise
 */
static bool check_clock_and_configure(connection_info_t *connection_info_p,
                                      btc_trx_configs_t *btc_trx_configs,
                                      uint8_t clk_offset)
{
  if (!IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle),
                      connection_info_p->end_tsf)) {
    SM_STATE(connection_info_p->connection_info_sm, roleswitch_central_sm_idle);
    return false;
  }
  configure_and_update_channel(connection_info_p, btc_trx_configs, clk_offset);
  return true;
}

/**
 * @brief Helper function to handle BTC rail failure status
 * @param status The status returned by the BTC rail operation
 * @param connection_info_sm The state machine pointer
 * @param fail_state The state to transition to in case of failure
 * @return true if the status indicates failure, false otherwise
 */
void handle_btc_rail_failure(uint32_t status, sm_t *connection_info_sm, uint8_t fail_state)
{
  if (status != BTC_RAIL_PASS_STATUS) {
    SM_STEP(ROLESWITCH_CENTRAL_SM, connection_info_sm, fail_state);
  }
}

/**
 * @brief Helper function to handle RX scheduling with retry check
 */
static bool schedule_rx_with_retry(sm_t *connection_info_sm,
                                   btc_trx_configs_t *btc_trx_configs,
                                   btc_dev_t *dev,
                                   uint8_t next_state)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm);
  if (!IS_BT_CLOCK_LT(btc_trx_configs->btc_rail_trx_config.clk,
                      dev->roleswitch_data.last_retry_instant)) {
    return false;
  }
  btc_trx_configs->rx_hdr_needed = 1;
  configure_trx_acl_rx(btc_trx_configs, connection_info_p->br_edr_mode, BTC_RAIL_RX_TIMEOUT);
  uint32_t status = schedule_rx(btc_trx_configs);
  SM_STATE(connection_info_sm, next_state);

  handle_btc_rail_failure(status, connection_info_sm, RX_ABORT);
  return true;
}

/**
 * @brief Retry RX scheduling with clock check
 */
static void retry_rx_scheduling(sm_t *connection_info_sm, connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs, btc_dev_t *dev, uint8_t next_state)
{
  if (check_clock_and_configure(connection_info_p, btc_trx_configs, BTC_BD_CLK_2NP0)) {
    if (!schedule_rx_with_retry(connection_info_sm, btc_trx_configs, dev, next_state)) {
      roleswitch_activity_complete(connection_info_sm, CONN_FAILURE);
    }
  } else {
    roleswitch_activity_complete(connection_info_sm, CONN_FAILURE);
  }
}

/**
 * @brief function is used to schedule reception to receive packet, is called in case of rx done timeout
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
static void rs_central_new_peripheral_rx_complete(sm_t *connection_info_sm)
{
  btc_dev_t *dev = BTC_DEV_PTR;
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  if (check_clock_and_configure(connection_info_p, btc_trx_configs, BTC_BD_CLK_2NP2)) {
    btc_trx_configs->ignore_rx_pkt = 1;  // To avoid DMA transfer for the received POLL packet
    form_poll_and_null_pkt(btc_trx_configs, dev->connection_info[dev->roleswitch_data.new_connection_idx].lt_addr, BT_NULL_PKT_TYPE);
    uint32_t status = schedule_tx(btc_trx_configs);
    SM_STATE(connection_info_sm, NEW_PERIPHERAL_TX_SCHEDULED);
    handle_btc_rail_failure(status, connection_info_sm, RX_ABORT);  // TODO this should be RX_DONE
  }
}

/**
 * @brief function is used to initialise roleswitch parameters and is used to start the roleswitch procedure.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void roleswitch_central_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_START_ROLESWITCH_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  connection_info->connection_info_sm = &roleswitch_central_sm;
  connection_info->connection_info_sm->current_device_index = *HSS_CMD_START_ROLESWITCH_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd);
  connection_info->priority = *HSS_CMD_START_ROLESWITCH_CENTRAL_PRIORITY(hss_cmd);
  connection_info->step_size = *HSS_CMD_START_ROLESWITCH_CENTRAL_STEP_SIZE(hss_cmd);
  connection_info->end_tsf = *HSS_CMD_START_ROLESWITCH_CENTRAL_END_TSF(hss_cmd);
  //calculating end tsf and adding clk value that has been elapsed. (Procedure activity duration)
  connection_info->end_tsf =  btc_dev_p->roleswitch_data.roleswitch_instant + btc_get_current_bd_clk_cnt(&connection_info->trx_config.bd_clk_counter_handle) + (connection_info->end_tsf * HALF_SLOT_FACTOR);
  update_aperiodic_role(btc_dev_p, connection_info->connection_info_sm);

  SM_STEP(ROLESWITCH_CENTRAL_SM, connection_info->connection_info_sm, START_PROCEDURE);
}

/**
 * @brief function handles the start procedure trigger for roleswitch state machine
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void roleswitch_central_start_procedure_handler(sm_t *connection_info_sm_p)
{
  btc_dev_t *dev = BTC_DEV_PTR;
  connection_info_t *connection_info_p = &dev->connection_info[connection_info_sm_p->current_device_index];
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  btc_trx_configs->btc_rail_trx_config.txPower = connection_info_p->tx_pwr_index;
  dev->l2_scheduled_sm = connection_info_sm_p;
  uint32_t status;
  //  all necessary parameters for slot programming
  btc_trx_configs->btc_rail_trx_config.clk = dev->roleswitch_data.roleswitch_instant;
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
  dev->roleswitch_data.last_retry_instant = (dev->roleswitch_data.roleswitch_instant + (NEW_CONN_TIME_OUT << 2));//updating clk value
  btc_trx_configs->rx_hdr_needed = 1;
  configure_trx_acl_rx(btc_trx_configs, connection_info_p->br_edr_mode, BTC_RAIL_RX_TIMEOUT);
  status = schedule_rx(btc_trx_configs);
  SM_STATE(connection_info_sm_p, WAITING_FOR_FHS_RX_PKT_HEADER);
  handle_btc_rail_failure(status, connection_info_sm_p, RX_ABORT);
}

/**
 * @brief function is used to stop roleswitch procedure.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void stop_roleswitch(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_STOP_ROLESWITCH_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  procedure_activity_complete(connection_info->connection_info_sm, roleswitch_central_sm_idle);
}

/**
 * @brief function is used to schedule reception to receieve the packet and check arqn stage 1
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 * @param
 *
 */
void rs_central_rx_header_received_handler(sm_t *connection_info_sm)
{
  SM_STATE(connection_info_sm,
           WAITING_FOR_FHS_RX_PKT_COMPLETE); // Updating state machine to CENTRAL_WAITING_FOR_RX_PACKET_HEADER
}

/**
 * @brief function schedules reception to receive packet.
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_central_fhs_rx_timeout_handler(sm_t *connection_info_sm)
{
  roleswitch_central_fhs_rx_retry_and_rx_timeout(connection_info_sm, WAITING_FOR_FHS_RX_PKT_HEADER);
}

/**
 * @brief function schedules tx to transfer next packet and function exits on successful reception of FHS
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_central_fhs_rx_complete_handler(sm_t *connection_info_sm)
{
  btc_dev_t *dev = BTC_DEV_PTR;
  connection_info_t *connection_info_p = &dev->connection_info[connection_info_sm->current_device_index];
  uint32_t status;
  memcpy(dev->fhs_rx_pkt, received_rx_pkt, received_rx_pkt_len);
  update_rcvd_fhs_data_for_new_role(&dev->roleswitch_data, &dev->connection_info[dev->roleswitch_data.new_connection_idx], (uint8_t *)&dev->fhs_rx_pkt[0], connection_info_p);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), connection_info_p->end_tsf)) {
    //   all necessary parameters for slot programming
    configure_nonconn_slot_params(btc_trx_configs, BTC_BD_CLK_2NP2);
    update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
    // Schedule first ID packet TX
    SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);
    status = schedule_tx(btc_trx_configs);
    SM_STATE(connection_info_sm, ID_PKT_TX_SCHEDULED);
    handle_btc_rail_failure(status, connection_info_sm, RX_ABORT);
  } else {
    SM_STATE(connection_info_sm, roleswitch_central_sm_idle);
  }
}

/**
 * @brief function schedules reception to receive packet, is called in case of rx done timeout
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_central_fhs_rx_complete_timeout_handler(sm_t *connection_info_sm)
{
  roleswitch_central_fhs_rx_retry_and_rx_timeout(connection_info_sm, WAITING_FOR_FHS_RX_PKT_HEADER);
}

/**
 * @brief function is used to open for reception to receive packets from peripheral.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_central_id_tx_done_handler(sm_t *connection_info_sm)
{
  btc_dev_t *dev = BTC_DEV_PTR;
  uint8_t state = connection_info_sm->current_state;
  stop_aperiodic_role(dev);
  connection_info_t *connection_info_p = &dev->connection_info[dev->roleswitch_data.new_connection_idx];
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  connection_info_p->connection_info_sm = &roleswitch_central_sm;
  connection_info_sm->current_device_index = dev->roleswitch_data.new_connection_idx;
  connection_info_p->connection_info_sm->current_state = state;
  update_aperiodic_role(dev, connection_info_p->connection_info_sm);
  dev->l2_scheduled_sm = connection_info_p->connection_info_sm;
  roleswitch_enable_new_role_params(dev, connection_info_p);
  if (check_clock_and_configure(connection_info_p, btc_trx_configs, BTC_BD_CLK_2NP0)) {
    dev->roleswitch_data.last_retry_instant =
      (btc_trx_configs->btc_rail_trx_config.clk + (NEW_CONN_TIME_OUT << 2));

    btc_trx_configs->rx_window = BTC_RAIL_RX_TIMEOUT;
    schedule_rx_with_retry(connection_info_sm, btc_trx_configs, dev,
                           NEW_PERIPHERAL_WAITING_FOR_RX_PKT_HEADER);
  }
}

/**
 *@brief function handles transmission of tx packets for roleswitch state machine after the transmission fails to be scheduled.
 *@param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 *
 *
 */
void rs_central_id_tx_abort_handler(sm_t *connection_info_sm)
{
  roleswitch_central_fhs_rx_retry_and_rx_timeout(connection_info_sm, WAITING_FOR_FHS_RX_PKT_HEADER);
}

/**
 * @brief function is used to schedule reception to receieve the packet and check arqn stage 1 after the roleswitch has been done.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 * @param
 *
 */
void rs_central_new_peripheral_rx_header_handler(sm_t *connection_info_sm)
{
  SM_STATE(
    connection_info_sm,
    NEW_PERIPHERAL_WAITING_FOR_RX_PKT_COMPLETE); // Updating state machine to NEW_PERIPHERAL_WAITING_FOR_RX_PKT_COMPLETE
}

/**
 * @brief function schedules reception again.
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_central_new_peripheral_rx_timeout_handler(sm_t *connection_info_sm)
{
  roleswitch_central_fhs_rx_retry_and_rx_timeout(connection_info_sm, NEW_PERIPHERAL_WAITING_FOR_RX_PKT_HEADER);
}

void rs_central_rx_poll_null_done_handler(sm_t *connection_info_sm)
{
  rs_central_new_peripheral_rx_complete(connection_info_sm);
}

/**
 * @brief function schedules tx to transfer next packet
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_central_new_peripheral_rx_complete_handler(sm_t *connection_info_sm)
{
  rs_central_new_peripheral_rx_complete(connection_info_sm);
}

/**
 * @brief function schedules reception again.
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_central_new_peripheral_rx_complete_timeout_handler(sm_t *connection_info_sm)
{
  roleswitch_central_fhs_rx_retry_and_rx_timeout(connection_info_sm, NEW_PERIPHERAL_WAITING_FOR_RX_PKT_HEADER);
}

/**
 * @brief function is used to call roleswitch connection configure idle function to inform rail activity and status.
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_central_new_peripheral_tx_scheduled_done_handler(sm_t *connection_info_sm)
{
  SM_STATE(connection_info_sm, roleswitch_central_sm_idle);
  roleswitch_activity_complete(connection_info_sm, CONN_SUCCESS);
}

/**
 * @brief function is used to call roleswitch connection configure idle function function to inform rail activity and status.
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_central_new_peripheral_tx_scheduled_abort_handler(sm_t *connection_info_sm)
{
  roleswitch_central_fhs_rx_retry_and_rx_timeout(connection_info_sm, NEW_PERIPHERAL_WAITING_FOR_RX_PKT_HEADER);
}

/**
 * @brief function is used for FHS details to be updated into respective fields
 *
 * @param- data, pointer that is received from host pointing at the fhs packet structure received from host.
 */
void update_rcvd_fhs_data_for_new_role(roleswitch_data_t *roleswitch_data, connection_info_t *connection_info_new, uint8_t *data, connection_info_t *connection_info_old)
{
  uint64_t ProtimerDate = 0;
  uint32_t BtcDate = 0;

  rail_set_btc_clk_instance(&connection_info_old->trx_config.bd_clk_counter_handle);
  RAIL_SEQ_BTC_GetLastRxDate(connection_info_old->trx_config.bd_clk_counter_handle.bd_clk_counter_id, &BtcDate, &ProtimerDate);
  roleswitch_data->class_of_device = ((fhs_data_t *)data)->class_of_device;
  *(uint32_t *)((&connection_info_new->bd_address[0])) = ((fhs_data_t *)data)->lap;
  connection_info_new->bd_address[3] = ((fhs_data_t *)data)->uap;
  *(uint16_t *)((&connection_info_new->bd_address[4])) = ((fhs_data_t *)data)->nap;
  connection_info_new->lt_addr = ((fhs_data_t *)data)->lt_addr;
  llc_process_rx_fhs_pkt((uint32_t *)data, &roleswitch_data->received_rx_fhs_pkt, roleswitch_data->received_rx_fhs_pkt.fhs_rx_bd_addr);
  connection_info_new->sync_word[0] = roleswitch_data->received_rx_fhs_pkt.rx_sync_word[0];
  connection_info_new->sync_word[1] = roleswitch_data->received_rx_fhs_pkt.rx_sync_word[1];
  roleswitch_data->received_rx_fhs_pkt.fhs_rx_tsf = ProtimerDate >> TSF_SCALE_FACTOR;
}

/**
 * @brief function to indicate that roleswitch activity is complete with success or failure status, function call to indicate status to ull
 *
 * @param buffer
 * @param status
 */
void roleswitch_activity_complete(sm_t *connection_info_sm, uint32_t conn_status)
{
  uint8_t *buffer = NULL;
  pkb_t *pkb;
  (void)connection_info_sm;
  (void)conn_status;
  pkb = get_buffer_for_notification(&buffer);
  prep_hss_internal_event(buffer, BTC_ROLE_SWITCH_COMPLETE, BTC_ACTIVITY_DURATION_COMPLETED);
  btc_lpw_activity_complete_indication_to_ull(pkb);
  rail_configure_idle();
}

/**
 *@brief function schedules reception again after receiving abort
 *@param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 *
 *
 */
void roleswitch_central_fhs_rx_retry_and_rx_timeout(sm_t *connection_info_sm, uint8_t state)
{
  btc_dev_t *dev = BTC_DEV_PTR;
  connection_info_t *connection_info_p = &dev->connection_info[connection_info_sm->current_device_index];
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  if (state == WAITING_FOR_FHS_RX_PKT_HEADER) {
    retry_rx_scheduling(connection_info_sm, connection_info_p, btc_trx_configs, dev, WAITING_FOR_FHS_RX_PKT_HEADER);
  } else if (state == NEW_PERIPHERAL_WAITING_FOR_RX_PKT_HEADER) {
    retry_rx_scheduling(connection_info_sm, connection_info_p, btc_trx_configs, dev, NEW_PERIPHERAL_WAITING_FOR_RX_PKT_HEADER);
  }
}

/**
 * @brief function is used for new role details to be updated into respective fields
 *
 * @param- data, pointer that is received from host pointing at the fhs packet structure received from host.
 */
void roleswitch_enable_new_role_params(btc_dev_t *btc_dev, connection_info_t *connection_info_p)
{
  //TODO : convey to host that sometimes roleswitch fails.
  uint32_t btcReference = btc_dev->roleswitch_data.received_rx_fhs_pkt.fhs_rx_clk;
  uint64_t protimerReference = btc_dev->roleswitch_data.received_rx_fhs_pkt.fhs_rx_tsf << TSF_SCALE_FACTOR;
  connection_info_p->trx_config.bd_clk_counter_handle.bd_clk_counter_id = BTC_INIT_CLK;
  // RAIL_SEQ_BTC_InitBtcDate(connection_info_p->trx_config.bd_clk_counter_handle.bd_clk_counter_id, btcReference, protimerReference + btc_dev->roleswitch_data.roleswitch_slot_offset); // TODO : sync after id reception is failing if slot offset is computed to be a large value
  RAIL_SEQ_BTC_InitBtcDate(connection_info_p->trx_config.bd_clk_counter_handle.bd_clk_counter_id, btcReference, protimerReference);
  RAIL_SEQ_BTC_ConfigAutoMode(connection_info_p->trx_config.bd_clk_counter_handle.bd_clk_counter_id, BTCCLKMODE_AUTO);
  RAIL_SEQ_BTC_setAccessAddress(btc_dev->connection_info[btc_dev->roleswitch_data.new_connection_idx].sync_word);
  RAIL_SEQ_BTC_setHecInit(btc_dev->roleswitch_data.received_rx_fhs_pkt.fhs_rx_bd_addr[3]);
  RAIL_SEQ_BTC_setCrcInit(btc_dev->roleswitch_data.received_rx_fhs_pkt.fhs_rx_bd_addr[3]);
  rail_set_btc_clk_instance(&connection_info_p->trx_config.bd_clk_counter_handle);
  connection_info_p->end_tsf =  btc_dev->roleswitch_data.roleswitch_instant + btc_get_current_bd_clk_cnt(&connection_info_p->trx_config.bd_clk_counter_handle) + (btc_dev->connection_info[btc_dev->roleswitch_data.old_connection_idx].end_tsf * HALF_SLOT_FACTOR);
}
