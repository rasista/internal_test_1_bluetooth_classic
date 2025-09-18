/**
 * @file roleswitch_peripheral.c
 * @author  (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#include "roleswitch_peripheral.h"
#include "btc_device.h"
#include "btc_rail.h"
#include "acl_common.h"
#include "lpw_to_host_interface.h"
#include "roleswitch_peripheral_sm_autogen.h"
#include "lpw_scheduler.h"
#ifdef SIM
#include "rail_simulation.h"
#else
#include "btc_rail_intf.h"
#endif

static void configure_and_update_channel(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs, uint8_t clk_offset)
{
  configure_conn_slot_params(connection_info_p, clk_offset, btc_trx_configs);
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
}

static bool check_clock_and_configure(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs, uint8_t clk_offset)
{
  if (!IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), connection_info_p->end_tsf)) {
    SM_STATE(connection_info_p->connection_info_sm, roleswitch_peripheral_sm_idle);
    return false;
  }
  configure_and_update_channel(connection_info_p, btc_trx_configs, clk_offset);
  return true;
}

static void handle_schedule_failure(sm_t *connection_info_sm, uint8_t state)
{
  SM_STATE(connection_info_sm, state);
  SM_STEP(ROLESWITCH_PERIPHERAL_SM, connection_info_sm, (state == RX_ABORT ? RX_ABORT : TX_ABORT));
}

/**
 * @brief function is used to schedule reception to receive packet, is called in case of rx done timeout
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
static void rs_peripheral_new_central_rx_complete(sm_t *connection_info_sm)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  btc_trx_configs->ignore_rx_pkt = 1; // To avoid DMA transfer for the received POLL packet
  SM_STATE(connection_info_sm, roleswitch_peripheral_sm_idle);
  roleswitch_activity_complete(connection_info_sm, CONN_SUCCESS);
}

/**
 * @brief function is used to initialise roleswitch parameters and is used to start the roleswitch procedure.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void roleswitch_peripheral_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_START_ROLESWITCH_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  connection_info->connection_info_sm = &roleswitch_peripheral_sm;
  connection_info->connection_info_sm->current_device_index = *HSS_CMD_START_ROLESWITCH_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd);
  connection_info->priority = *HSS_CMD_START_ROLESWITCH_PERIPHERAL_PRIORITY(hss_cmd);
  connection_info->step_size = *HSS_CMD_START_ROLESWITCH_PERIPHERAL_STEP_SIZE(hss_cmd);
  connection_info->end_tsf = *HSS_CMD_START_ROLESWITCH_PERIPHERAL_END_TSF(hss_cmd);
  //calculating end tsf and adding clk value that has been elapsed. (Procedure activity duration)
  connection_info->end_tsf =  btc_dev_p->roleswitch_data.roleswitch_instant + btc_get_current_bd_clk_cnt(&connection_info->trx_config.bd_clk_counter_handle) + (connection_info->end_tsf * HALF_SLOT_FACTOR);
  update_aperiodic_role(btc_dev_p, connection_info->connection_info_sm);
  SM_STEP(ROLESWITCH_PERIPHERAL_SM, connection_info->connection_info_sm, START_PROCEDURE);
}

/**
 * @brief function is used to stop roleswitch procedure.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void stop_roleswitch_peripheral(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_STOP_ROLESWITCH_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  procedure_activity_complete(connection_info->connection_info_sm, roleswitch_peripheral_sm_idle);
}

/**
 * @brief function handles the start procedure trigger for roleswitch state machine
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void roleswitch_peripheral_start_procedure_handler(sm_t *connection_info_sm_p)
{
  btc_dev_t *dev = BTC_DEV_PTR;
  connection_info_t *connection_info_p = &dev->connection_info[connection_info_sm_p->current_device_index];
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  btc_trx_configs->btc_rail_trx_config.txPower = connection_info_p->tx_pwr_index;
  dev->l2_scheduled_sm = connection_info_sm_p;
  btc_trx_configs->btc_rail_trx_config.clk = dev->roleswitch_data.roleswitch_instant;
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
  form_fhs_pkt(btc_trx_configs, dev->fhs_pkt, dev->connection_info[dev->roleswitch_data.old_connection_idx].lt_addr, connection_info_p->lt_addr);
  update_fhs_pkt_clk(btc_trx_configs, dev->fhs_pkt, dev->connection_info[dev->roleswitch_data.old_connection_idx].lt_addr, dev->roleswitch_data.new_clk_roleswitch_instant);
  dev->roleswitch_data.last_retry_instant = (dev->roleswitch_data.roleswitch_instant + (NEW_CONN_TIME_OUT << 2));
  if (schedule_tx(btc_trx_configs) == BTC_RAIL_FAIL_STATUS) {
    handle_schedule_failure(connection_info_sm_p, FHS_TX_SCHEDULED);
  } else {
    SM_STATE(connection_info_sm_p, FHS_TX_SCHEDULED);
  }
}
/**
 * @brief function is used to schedule reception to receieve the packet.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 * @param
 *
 */
void rs_peripheral_fhs_tx_done_handler(sm_t *connection_info_sm)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  configure_and_update_channel(connection_info_p, btc_trx_configs, BTC_BD_CLK_2NP2);
  SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);
  btc_trx_configs->rx_window = BTC_RAIL_RX_TIMEOUT;
  btc_trx_configs->rx_hdr_needed = 1;
  if (schedule_rx(btc_trx_configs) == BTC_RAIL_FAIL_STATUS) {
    handle_schedule_failure(connection_info_sm, ID_RX_SCHEDULED);
  } else {
    SM_STATE(connection_info_sm, ID_RX_SCHEDULED);
  }
}
/**
 * @brief function is used to schedule reception again to receieve the packet after getting abort in previous state.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 * @param
 *
 */
void rs_peripheral_fhs_tx_abort_handler(sm_t *connection_info_sm)
{
  roleswitch_peripheral_retransmit_fhs(connection_info_sm);
}

/**
 * @brief function schedules tx to transfer next packet
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_peripheral_id_rx_done_handler(sm_t *connection_info_sm)
{
  btc_dev_t *dev = BTC_DEV_PTR;
  uint8_t state = connection_info_sm->current_state;
  stop_aperiodic_role(dev);
  connection_info_t *connection_info_p = &dev->connection_info[dev->roleswitch_data.new_connection_idx];
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  connection_info_p->connection_info_sm = &roleswitch_peripheral_sm;
  connection_info_sm->current_device_index = dev->roleswitch_data.new_connection_idx;
  connection_info_p->connection_info_sm->current_state = state;
  update_aperiodic_role(dev, connection_info_p->connection_info_sm);
  dev->l2_scheduled_sm = connection_info_p->connection_info_sm;
  roleswitch_enable_new_role_params_peri(connection_info_p, dev);
  if (!check_clock_and_configure(connection_info_p, btc_trx_configs, BTC_BD_CLK_2NP0)) {
    SM_STATE(connection_info_sm, roleswitch_peripheral_sm_idle);
    return;
  }

  dev->roleswitch_data.last_retry_instant = (btc_trx_configs->btc_rail_trx_config.clk + (NEW_CONN_TIME_OUT << 2));
  // set_enhanced_acl_config(btc_trx_configs); TODO : why added this ?
  form_poll_and_null_pkt(btc_trx_configs, dev->connection_info[dev->roleswitch_data.new_connection_idx].lt_addr, BT_POLL_PKT_TYPE);
  if (schedule_tx(btc_trx_configs) == BTC_RAIL_FAIL_STATUS) {
    handle_schedule_failure(connection_info_sm, NEW_CENTRAL_TX_SCHEDULED);
  } else {
    SM_STATE(connection_info_sm, NEW_CENTRAL_TX_SCHEDULED);
  }
}

/**
 * @brief function schedules tx to transfer next packet after receiving abort in previous state.
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_peripheral_id_rx_timeout_handler(sm_t *connection_info_sm)
{
  roleswitch_peripheral_retransmit_fhs(connection_info_sm);
}

/**
 * @brief function is used to open for reception to receive packets [after roleswitch].
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_peripheral_new_central_tx_done_handler(sm_t *connection_info_sm)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;

  if (!check_clock_and_configure(connection_info_p, btc_trx_configs, BTC_BD_CLK_2NP2)) {
    SM_STATE(connection_info_sm, roleswitch_peripheral_sm_idle);
    return;
  }
  btc_trx_configs->rx_hdr_needed = 1;
  configure_trx_acl_rx(btc_trx_configs, connection_info_p->br_edr_mode, BTC_RAIL_RX_TIMEOUT);
  if (schedule_rx(btc_trx_configs) == BTC_RAIL_FAIL_STATUS) {
    handle_schedule_failure(connection_info_sm, NEW_CENTRAL_WAITING_FOR_RX_PACKET_HEADER);
  } else {
    SM_STATE(connection_info_sm, NEW_CENTRAL_WAITING_FOR_RX_PACKET_HEADER);
  }
}

/**
 * @brief function schedules transmission again.
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_peripheral_new_central_tx_abort_handler(sm_t *connection_info_sm)
{
  roleswitch_new_central_poll_retransmit(connection_info_sm);
}

/**
 * @brief function is used to open for reception to receive packets.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_peripheral_new_central_rx_header_received_handler(sm_t *connection_info_sm)
{
  SM_STATE(connection_info_sm,
           NEW_CENTRAL_WAITING_FOR_RX_PKT_COMPLETE); // Updating state machine to NEW_CENTRAL_WAITING_FOR_RX_PKT_COMPLETE
}

/**
 * @brief function is used to open for reception to receive packets after getting an abort in previous state.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_peripheral_new_central_rx_header_timeout_handler(sm_t *connection_info_sm)
{
  roleswitch_new_central_poll_retransmit(connection_info_sm);
}

/**
 * @brief function is used to schedule reception to receive packet, is called in case of rx done timeout
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_peripheral_rx_poll_null_done_handler(sm_t *connection_info_sm)
{
  rs_peripheral_new_central_rx_complete(connection_info_sm);
}
/**
 * @brief function is used to call roleswitch_connection_configure function to inform rail activity and status.
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_peripheral_new_central_rx_complete_handler(sm_t *connection_info_sm)
{
  rs_peripheral_new_central_rx_complete(connection_info_sm);
}

/**
 * @brief function is used to call roleswitch_connection_configure function to inform rail activity and status after getting an abort.
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 */
void rs_peripheral_new_central_rx_complete_timeout_handler(sm_t *connection_info_sm)
{
  roleswitch_new_central_poll_retransmit(connection_info_sm);
}

static void common_retransmit_logic(sm_t *connection_info_sm, uint8_t next_state)
{
  btc_dev_t *dev = BTC_DEV_PTR;
  connection_info_t *connection_info_p = &dev->connection_info[connection_info_sm->current_device_index];
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  uint32_t status;
  if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), connection_info_p->end_tsf)) {
    configure_conn_slot_params(connection_info_p, BTC_BD_CLK_2NP0, btc_trx_configs);
    if (next_state == FHS_TX_SCHEDULED) {
      update_fhs_pkt_clk(btc_trx_configs, dev->fhs_pkt, dev->connection_info[dev->roleswitch_data.new_connection_idx].lt_addr, dev->roleswitch_data.new_clk_roleswitch_instant);
    } else if (next_state == NEW_CENTRAL_TX_SCHEDULED) {
      form_poll_and_null_pkt(btc_trx_configs, dev->connection_info[dev->roleswitch_data.new_connection_idx].lt_addr, BT_POLL_PKT_TYPE);
    }
    #ifdef SIM
    packetization_acl(connection_info_p, btc_trx_configs);
    #endif
    update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);

    if (IS_BT_CLOCK_LT(btc_trx_configs->btc_rail_trx_config.clk, dev->roleswitch_data.last_retry_instant)) {
      status = schedule_tx(btc_trx_configs);
      SM_STATE(connection_info_sm, next_state);

      if (status != BTC_RAIL_PASS_STATUS) {
        SM_STEP(ROLESWITCH_PERIPHERAL_SM, connection_info_sm, TX_ABORT);
      }
    }
  } else {
    SM_STATE(connection_info_sm, roleswitch_peripheral_sm_idle);
    roleswitch_activity_complete(connection_info_sm, CONN_FAILURE);
  }
}

/**
 * @brief common function is used to schedule transmission again in case of TX Abort or RX timeout.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 * @param
 *
 */
void roleswitch_peripheral_retransmit_fhs(sm_t *connection_info_sm)
{
  common_retransmit_logic(connection_info_sm, FHS_TX_SCHEDULED);
}

/**
 * @brief common function is used to schedule transmission again in case of TX Abort or RX timeout after becoming new central.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow
 * @param
 *
 */
void roleswitch_new_central_poll_retransmit(sm_t *connection_info_sm)
{
  common_retransmit_logic(connection_info_sm, NEW_CENTRAL_TX_SCHEDULED);
}

void roleswitch_enable_new_role_params_peri(connection_info_t *connection_info_p, btc_dev_t *btc_dev)
{
  RAIL_SEQ_BTC_setAccessAddress(btc_dev->sync_word);
  RAIL_SEQ_BTC_setHecInit(btc_dev->bd_addr[3]);
  RAIL_SEQ_BTC_setCrcInit(btc_dev->bd_addr[3]);
  connection_info_p->trx_config.bd_clk_counter_handle.bd_clk_counter_id = BTC_NATIVE_CLK;
  RAIL_SEQ_BTC_ConfigAutoMode(connection_info_p->trx_config.bd_clk_counter_handle.bd_clk_counter_id, BTCCLKMODE_NONE);
  rail_set_btc_clk_instance(&connection_info_p->trx_config.bd_clk_counter_handle);
  connection_info_p->end_tsf =  btc_dev->roleswitch_data.roleswitch_instant + btc_get_current_bd_clk_cnt(&connection_info_p->trx_config.bd_clk_counter_handle) + (btc_dev->connection_info[btc_dev->roleswitch_data.old_connection_idx].end_tsf * HALF_SLOT_FACTOR);
}
