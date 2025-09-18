#include "esco.h"
#include "btc_device.h"
#include "btc_rail.h"
#include "esco.h"
#include "acl_common.h"
#include "arqn_seqn_check.h"
#include "esco_peripheral_sm_autogen.h"
#include "lpw_scheduler.h"

/**
 * @brief function is used to initialise esco parameters and is used to start the esco procedure.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void esco_peripheral_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_START_ESCO_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  connection_info->esco_sm = &esco_peripheral_sm;
  SM_STATE(connection_info->esco_sm, esco_peripheral_sm_idle);
  connection_info->esco_sm->current_device_index = *HSS_CMD_START_ESCO_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd);
  connection_info->priority = *HSS_CMD_START_ESCO_PERIPHERAL_PRIORITY(hss_cmd);
  connection_info->step_size = *HSS_CMD_START_ESCO_PERIPHERAL_STEP_SIZE(hss_cmd);
  connection_info->end_tsf = *HSS_CMD_START_ESCO_PERIPHERAL_END_TSF(hss_cmd);

  memcpy(connection_info->bd_address, HSS_CMD_START_ESCO_PERIPHERAL_BD_ADDRESS(hss_cmd), BD_ADDR_LEN);
  memcpy(connection_info->sync_word, HSS_CMD_START_ESCO_PERIPHERAL_SYNC_WORD(hss_cmd), SYNC_WORD_LEN);

  SM_STEP(ESCO_PERIPHERAL_SM, connection_info->esco_sm, START_PROCEDURE);
}

/**
 * @brief function is used to stop esco procedure.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void stop_esco_peripheral(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_STOP_ESCO_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  SM_STATE(connection_info->esco_sm, esco_peripheral_sm_idle);
  btc_trx_configs_t *btc_trx_configs = &btc_dev_p->connection_info->btc_trx_configs_esco;
  stop_periodic_role(btc_dev_p, btc_trx_configs);
}

/**
 *@brief function handles transmission of tx packets for esco state machine.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,esco_info
 *
 *
 */
void esco_peripheral_start_procedure_handler(sm_t *esco_sm)
{
  uint32_t status;
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  esco_info_t *esco_info_p = &connection_info_p->esco_info;
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->btc_trx_configs_esco;
  connection_info_p->esco_info.valid_esco_pkt_rcvd = 0;
  calculate_first_esco_window_start(connection_info_p);

  //  all necessary parameters for slot programming
  configure_conn_slot_params_for_esco_window_start(connection_info_p, btc_trx_configs, esco_info_p);
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);

  /* ESCO respective parameters are updated to RAIL config */
  update_esco_parameters(connection_info_p, btc_trx_configs);

  if (esco_info_p->is_esco == 1) {
    // Configure data rate (BDR or EDR)
    if (esco_info_p->is_edr == 1) {
      btc_trx_configs->btc_rail_trx_config.linkConfig.enhancedRate = BASIC_RATE;
    } else {
      btc_trx_configs->btc_rail_trx_config.linkConfig.enhancedRate = ENHANCED_RATE;
    }
    // Enable eSCO and disable SCO
    btc_trx_configs->btc_rail_trx_config.linkConfig.esco = 1;
    btc_trx_configs->btc_rail_trx_config.linkConfig.sco = 0;
  } else {
    // Handle SCO configuration
    btc_trx_configs->btc_rail_trx_config.specialPktType = SL_RAIL_BTC_NORMAL_PKT; // Default for SCO
    btc_trx_configs->btc_rail_trx_config.linkConfig.enhancedRate = BASIC_RATE; // SCO only supports BDR
    btc_trx_configs->btc_rail_trx_config.linkConfig.sco = 1; // Enable SCO
    btc_trx_configs->btc_rail_trx_config.linkConfig.esco = 0; // Disable eSCO
  }

  status = schedule_rx(btc_trx_configs);
  SM_STATE(esco_sm, ESCO_PERIPHERAL_RX_HEADER_SCHEDULED);
  if (status != BTC_RAIL_PASS_STATUS) {
    // if RX reception schedule failed, move forward assuming RX TIMEOUT
    SM_STEP(ESCO_PERIPHERAL_SM, esco_sm, RX_ABORT);
  }
}

/**
 * @brief function is used to call functions to process rx header and trigger circular buffer
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,esco_info
 * @param
 *
 */
void esco_peripheral_rx_header_done_handler(sm_t *esco_sm)
{
  uint8_t length = 0;
  buffc_t buffc;  //TODO: Temporary variables will be integrated with buffer management implementation
  btc_arqn_stage1();
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  esco_info_t       *esco_info_p = &connection_info_p->esco_info;
  if (esco_info_p->is_esco == 1) {
    if ((is_hec_check_passed() == FALSE)) {
      SM_STEP(ESCO_PERIPHERAL_SM, esco_sm, RX_ABORT); //rx abort being sent in case of crc and hec check failure
    }
  }
  //if(!(packet_type== NULL/POLL))    //This part will used after all Packet types implementation in place.
  //{
  btc_trigger_buffc(&buffc, length); //Function to trigger circular buffer
  //}
  SM_STATE(esco_sm, ESCO_PERIPHERAL_RX_COMPLETE_SCHEDULED);
}

/**
 * @brief function schedules tx to transfer next packet and calls function to check if circular buffer triggered or not
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,esco_info
 */
void esco_peripheral_rx_complete_handler(sm_t *esco_sm)
{
  uint32_t status;
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->btc_trx_configs_esco;
  esco_info_t       *esco_info_p = &connection_info_p->esco_info;
  buffc_t buffc;
  btc_is_buffc_triggered(&buffc); //check if circular buffer is triggered or not
  btc_arqn_stage2();      //process_arqn_stage2() call after implementation
  btc_wait_for_bufc_transfer_status();
  //  all necessary parameters for slot programming
  configure_conn_slot_params(connection_info_p, BTC_BD_CLK_2NP2, btc_trx_configs);
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
  // packet formation
  packetization_acl(connection_info_p, btc_trx_configs);
  /* ESCO respective parameters are updated to RAIL config */
  update_esco_parameters(connection_info_p, btc_trx_configs);
  //schedule tx
  status = schedule_tx(btc_trx_configs);
  SM_STATE(esco_sm, ESCO_PERIPHERAL_TX_SCHEDULED);
  if (esco_info_p->is_esco == 1) {
    if (is_crc_check_passed() == FALSE) {
      //TODO: update the next tx packet header with arqn as zero (schedule tx happens with arqn update)
    }
  }

  if (status != BTC_RAIL_PASS_STATUS) {
    SM_STEP(ESCO_PERIPHERAL_SM, esco_sm, TX_ABORT);
  }
  connection_info_p->esco_info.valid_esco_pkt_rcvd = 1; //will be removed after ARQN proper updates,change to be reviewed
}

/**
 * @brief function schedules reception, as the previous packet reception recieved and timeout or abort trigger.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,esco_info
 */
void esco_peripheral_rx_timeout_handler(sm_t *esco_sm)
{
  esco_peripheral_retransmit(esco_sm, TRUE);
}

/**
 * @brief function is used to open for reception to receive packets from peripheral.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,esco_info
 */
void esco_peripheral_tx_done_handler(sm_t *esco_sm)
{
  esco_peripheral_retransmit(esco_sm, FALSE);
}

/**
 *@brief function handles transmission of tx packets for esco state machine after the transmission fails to be scheduled.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,esco_info
 *
 *
 */
void esco_peripheral_tx_abort_handler(sm_t *esco_sm)
{
  esco_peripheral_retransmit(esco_sm, FALSE);
}

/**
 * @brief function is used to open for reception to receive packets from peripheral with check of retransmission.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,esco_info
 */
void esco_peripheral_retransmit(sm_t *esco_sm, bool is_rx_timeout)
{
  uint32_t status;
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->btc_trx_configs_esco;
  esco_info_t       *esco_info_p = &connection_info_p->esco_info;

  // ARQ logic is only needed for eSCO and for retransmitting packet only
  if ((esco_info_p->is_esco == 1) && (check_slot_is_esco_retransmit(connection_info_p, btc_trx_configs) == TRUE)) {
    //  all necessary parameters for slot programming
    uint8_t slot_config = is_rx_timeout ? BTC_BD_CLK_2NP2 : BTC_BD_CLK_2NP0;
    configure_conn_slot_params(connection_info_p, slot_config, btc_trx_configs);
    update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
    packetization_acl(connection_info_p, btc_trx_configs);
    /* ESCO respective parameters are updated to RAIL config */
    update_esco_parameters(connection_info_p, btc_trx_configs);
    if (is_rx_timeout) {
      status = schedule_tx(btc_trx_configs);
      SM_STATE(esco_sm, ESCO_PERIPHERAL_TX_SCHEDULED);
    } else {
      status = schedule_rx(btc_trx_configs);
      SM_STATE(esco_sm, ESCO_RETRANSMIT_PERIPHERAL_WAITING_FOR_RX_HEADER);
    }
    if (status != BTC_RAIL_PASS_STATUS) {
      SM_STEP(ESCO_PERIPHERAL_SM, esco_sm, RX_ABORT);
    }
  } else {
    esco_peripheral_calculate_next_esco_window_start(esco_sm);
  }
}

/**
 * @brief function calculates next esco window start and then schedules tx
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 */
void esco_peripheral_calculate_next_esco_window_start(sm_t *esco_sm)
{
  uint32_t status;
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->btc_trx_configs_esco;
  esco_info_t *esco_info_p = &connection_info_p->esco_info;
  connection_info_p->esco_info.valid_esco_pkt_rcvd = 0;
  //updating next ESCO interval
  esco_info_p->esco_window_start = (esco_info_p->esco_window_start) + (esco_info_p->t_esco << 1);   //converting all in half slots
  //  all necessary parameters for slot programming
  configure_conn_slot_params_for_esco_window_start(connection_info_p, btc_trx_configs, esco_info_p);
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
  /* ESCO respective parameters are updated to RAIL config */
  update_esco_parameters(connection_info_p, btc_trx_configs);
  /* As the periodic activity is being configured for next interval, we are removing from queue in schedule_rx, so that in schedule_rx,
     it gets updated properly in the queue in case of multiple periodic activities are handled */
  SET_CONF_FLAG_NEW_WINDOW(btc_trx_configs->flags);
  //TODO, in horizontal integration, update in btc_rail_trx_configs_t
  status = schedule_rx(btc_trx_configs);
  SM_STATE(esco_sm, ESCO_PERIPHERAL_RX_HEADER_SCHEDULED);
  if (status != BTC_RAIL_PASS_STATUS) {
    SM_STEP(ESCO_PERIPHERAL_SM, esco_sm, RX_ABORT);
  }
  restore_aperiodic_role_punctured();
}
