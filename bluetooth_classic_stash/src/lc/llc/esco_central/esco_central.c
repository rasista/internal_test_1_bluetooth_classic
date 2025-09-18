#include "esco.h"
#include "btc_device.h"
#include "btc_rail.h"
#include "esco_central_sm_autogen.h"
#include "acl_common.h"
#include "lpw_scheduler.h"

static void configure_and_schedule_rx(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs, sm_t *esco_sm, uint32_t clk)
{
  uint32_t status;
  configure_conn_slot_params(connection_info_p, clk, btc_trx_configs);
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
  update_esco_parameters(connection_info_p, btc_trx_configs);
  status = schedule_rx(btc_trx_configs);
  SM_STATE(esco_sm, ESCO_CENTRAL_RX_HEADER_SCHEDULED);
  if (status != BTC_RAIL_PASS_STATUS) {
    SM_STEP(ESCO_CENTRAL_SM, esco_sm, RX_ABORT);
  }
}

/**
 * @brief function is used to initialise esco parameters and is used to start the esco procedure.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void esco_central_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_START_ESCO_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd)];

  connection_info->esco_sm = &esco_central_sm;
  SM_STATE(connection_info->esco_sm, esco_central_sm_idle);
  connection_info->esco_sm->current_device_index = *HSS_CMD_START_ESCO_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd);
  connection_info->priority = *HSS_CMD_START_ESCO_CENTRAL_PRIORITY(hss_cmd);
  connection_info->step_size = *HSS_CMD_START_ESCO_CENTRAL_STEP_SIZE(hss_cmd);
  connection_info->end_tsf = *HSS_CMD_START_ESCO_CENTRAL_END_TSF(hss_cmd);

  memcpy(connection_info->bd_address, HSS_CMD_START_ESCO_CENTRAL_BD_ADDRESS(hss_cmd), BD_ADDR_LEN);
  memcpy(connection_info->sync_word, HSS_CMD_START_ESCO_CENTRAL_SYNC_WORD(hss_cmd), SYNC_WORD_LEN);
  SM_STEP(ESCO_CENTRAL_SM, connection_info->esco_sm, START_PROCEDURE);
}

/**
 * @brief function is used to stop esco procedure.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void stop_esco(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_STOP_ESCO_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  SM_STATE(connection_info->esco_sm, esco_central_sm_idle);
  btc_trx_configs_t *btc_trx_configs = &btc_dev_p->connection_info->btc_trx_configs_esco;
  stop_periodic_role(btc_dev_p, btc_trx_configs);
}

/**
 *@brief function handles transmission of tx packets for esco state machine.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 *
 *
 */
void esco_central_start_procedure_handler(sm_t *esco_sm)
{
  uint32_t status;
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  esco_info_t *esco_info_p = &connection_info_p->esco_info;
  connection_info_p->esco_info.valid_esco_pkt_rcvd = 0;
  connection_info_p->esco_info.crc_status = 0;
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->btc_trx_configs_esco;
  calculate_first_esco_window_start(connection_info_p);

  //  all necessary parameters for slot programming
  configure_conn_slot_params_for_esco_window_start(connection_info_p, btc_trx_configs, esco_info_p);
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);

  // packet formation
  packetization_acl(connection_info_p, btc_trx_configs);

  /* ESCO respective parameters are updated to RAIL config */
  update_esco_parameters(connection_info_p, btc_trx_configs);

  if (esco_info_p->is_esco == 1) {
    // Configure packet type for eSCO (EV3 or 2EV3)
    // if (EV3 packet */) {
    //     btc_trx_configs->btc_rail_trx_config.specialPktType = EV3_PKT;
    // } else if (2EV3 packet */) {
    //     btc_trx_configs->btc_rail_trx_config.specialPktType = EV5_PKT; // Whatever the naming
    // } else {
    //     btc_trx_configs->btc_rail_trx_config.specialPktType = SL_RAIL_BTC_NORMAL_PKT; // Default for eSCO
    // }

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
  //schedule tx
  status = schedule_tx(btc_trx_configs);
  SM_STATE(esco_sm, ESCO_CENTRAL_TX_SCHEDULED);
  if (status != BTC_RAIL_PASS_STATUS) {
    SM_STEP(ESCO_CENTRAL_SM, esco_sm, TX_ABORT);
  }
}

/**
 * @brief function is used to open for reception to receive packets from peripheral.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 */
void esco_central_tx_done_handler(sm_t *esco_sm)
{
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->btc_trx_configs_esco;
  configure_and_schedule_rx(connection_info_p, btc_trx_configs, esco_sm, BTC_BD_CLK_2NP2);
}

/**
 *@brief function handles transmission of tx packets for esco state machine after the transmission fails to be scheduled.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 *
 *
 */
void esco_central_tx_abort_handler(sm_t *esco_sm)
{
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->btc_trx_configs_esco;
  configure_and_schedule_rx(connection_info_p, btc_trx_configs, esco_sm, BTC_BD_CLK_2NP2);
}

/**
 * @brief function is used to call functions to process rx header and trigger circular buffer
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 * @param
 *
 */
void esco_central_rx_header_done_handler(sm_t *esco_sm)
{
  buffc_t buffc;
  uint8_t length = 0;
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  esco_info_t       *esco_info_p = &connection_info_p->esco_info;
  btc_arqn_stage1();
  if (esco_info_p->is_esco == 1) {
    if ((is_hec_check_passed() == FALSE)) {
      SM_STEP(ESCO_CENTRAL_SM, esco_sm, RX_ABORT); //rx abort being sent in case of crc and hec check failure
    }
  }
  //if(!(packet_type== NULL/POLL))    //This part will used after all Packet types implementation in place.
  //{
  btc_trigger_buffc(&buffc, length); //Function to trigger circular buffer
  //}
  SM_STATE(esco_sm, ESCO_CENTRAL_RX_COMPLETE_SCHEDULED);
}

/**
 * @brief function schedules tx to transfer next packet, as the previous packet reception recieved and timeout or abort trigger.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 */
void esco_central_rx_timeout_handler(sm_t *esco_sm)
{
  esco_central_retransmit(esco_sm);
}

/**
 * @brief function schedules tx to transfer next packet and calls function to check if circular buffer triggered or not
 *
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 */
void esco_central_rx_complete_handler(sm_t *esco_sm)
{
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  buffc_t buffc;
  btc_is_buffc_triggered(&buffc); //check if circular buffer is triggered or not
  btc_arqn_stage2();      //process_arqn_stage2() call after implementation
  btc_wait_for_bufc_transfer_status();
  if (is_crc_check_passed() == FALSE) {
    //TODO: update the next tx packet header with arqn as zero (schedule tx happens with arqn update)
  }
  connection_info_p->esco_info.valid_esco_pkt_rcvd = 1; //will be removed after ARQN proper updates
  esco_central_retransmit(esco_sm);
}

/**
 * @brief function schedules tx to transfer next packet, as the previous packet reception recieved and timeout or abort trigger if retransmission is needed.
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 */
void esco_central_retransmit(sm_t *esco_sm)
{
  uint32_t status;
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->btc_trx_configs_esco;
  esco_info_t       *esco_info_p = &connection_info_p->esco_info;
  if ((esco_info_p->is_esco == 1) && (check_slot_is_esco_retransmit(connection_info_p, btc_trx_configs) == TRUE)) {
    //  all necessary parameters for slot programming
    configure_conn_slot_params(connection_info_p, BTC_BD_CLK_2NP0, btc_trx_configs);
    update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
    // packet formation
    packetization_acl(connection_info_p, btc_trx_configs);
    /* ESCO respective parameters are updated to RAIL config */
    update_esco_parameters(connection_info_p, btc_trx_configs);
    //schedule tx
    status = schedule_tx(btc_trx_configs);
    SM_STATE(esco_sm,
             ESCO_RETRANSMIT_CENTRAL_TX_SCHEDULED);   //Updating state machine to ESCO_RETRANSMIT_CENTRAL_TX_SCHEDULED
    if (status != BTC_RAIL_PASS_STATUS) {
      SM_STEP(ESCO_CENTRAL_SM, esco_sm, TX_ABORT);
    }
  } else {
    esco_central_calculate_next_esco_window_start(esco_sm);
  }
}

/**
 * @brief function calculates next esco window start and then schedules tx
 * @param connection_info - structure of connection_info state machine, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 */
void esco_central_calculate_next_esco_window_start(sm_t *esco_sm)
{
  uint32_t status;
  connection_info_t *connection_info_p = get_connection_info(esco_sm);
  esco_info_t *esco_info_p = &connection_info_p->esco_info;
  connection_info_p->esco_info.valid_esco_pkt_rcvd = 0;
  connection_info_p->esco_info.crc_status = 0;
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->btc_trx_configs_esco;
//updating next ESCO interval
  esco_info_p->esco_window_start = (esco_info_p->esco_window_start) + (esco_info_p->t_esco << 1);  //converting all in half slots
//TODO, in horizontal integration, update in btc_rail_trx_configs_t
  //  all necessary parameters for slot programming
  configure_conn_slot_params_for_esco_window_start(connection_info_p, btc_trx_configs, esco_info_p);
  // packet formation
  packetization_acl(connection_info_p, btc_trx_configs);
  update_channel_info(btc_trx_configs, &connection_info_p->btc_fh_data);
  /* ESCO respective parameters are updated to RAIL config */
  update_esco_parameters(connection_info_p, btc_trx_configs);
  /* As the periodic activity is being configured for next interval, we are removing from queue in schedule_tx, so that in schedule_tx,
     it gets updated properly in the queue in case of multiple periodic activities are handled */
  SET_CONF_FLAG_NEW_WINDOW(btc_trx_configs->flags);
  //schedule tx
  status = schedule_tx(btc_trx_configs);
  SM_STATE(esco_sm, ESCO_CENTRAL_TX_SCHEDULED); //Updating state machine to ESCO_CENTRAL_TX_SCHEDULED
  if (status != BTC_RAIL_PASS_STATUS) {
    SM_STEP(ESCO_CENTRAL_SM, esco_sm, TX_ABORT);
  }
  restore_aperiodic_role_punctured();
}

/**
 * @brief function checks if retransmission is needed or not based on the if conditions.
 * @param connection_info - structure of connection_info, containing information of bd _address, lt address, clk_offset,clk handles,
 * current device index,flow,acl_info,_esco_info
 */
uint8_t check_slot_is_esco_retransmit(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs)
{
  esco_info_t *esco_info_p = &connection_info_p->esco_info;
  if (esco_info_p->is_esco == 0) {
    return 0;  // no retransmissions for SCO
  }

  uint8_t *esco_tx_queue_pkt = NULL; //TODO: Update this variable during queue management
  uint32_t added_esco_window_start; //variable used to store added value of window start and w_esco
  added_esco_window_start = BT_ADD_CLOCK(esco_info_p->esco_window_start, esco_info_p->w_esco << 1);
  /*
     this variable is 1 if in case of below conditions
     if ESCO Tx pkt is not ACKED from remote,if ESCO Rx pkt is not received in this window,if ESCO Rx pkt ACK is not sent to remote device
     and if esco window is available or not
   */
  if (IS_BT_CLOCK_LTE(btc_trx_configs->btc_rail_trx_config.clk, added_esco_window_start)) {
    if ((connection_info_p->esco_info.w_esco) && (!connection_info_p->esco_info.crc_status) && ((esco_tx_queue_pkt != NULL) || (!connection_info_p->esco_info.valid_esco_pkt_rcvd) || (connection_info_p->esco_arqn_seqn_info.arqn_status == ACK))) {
      return TRUE;
    }
    return FALSE;
  }
  return FALSE;
}

/**
 * @brief function updates parameters specific to esco during Tx,Rx scheduling.
 * @param connection_info_p, btc_trx_configs pointers
 */
void update_esco_parameters(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs)
{
  /* Update ESCO/SCO params relatd to CRC is needed or not, length of Rx, Tx packet fixed  */
  SET_CONF_FLAG_PERIODIC(btc_trx_configs->flags);
  btc_trx_configs->sm = connection_info_p->esco_sm;
}

/**
 * @brief function updates parameters specific to esco, after first ESCO interval calculation.
 * d_esco(window_start)= (clk)mod(t_esco)
 * @param connection_info_p pointer
 */
void calculate_first_esco_window_start(connection_info_t *connection_info_p)
{
  esco_info_t *esco_info_p = &connection_info_p->esco_info;
  //calculating the current clk using function and storing it.
  uint32_t current_clk = btc_get_current_bd_clk_cnt(&connection_info_p->trx_config.bd_clk_counter_handle);
  uint32_t mod_val = 0;
  current_clk++;
  mod_val = current_clk % (esco_info_p->t_esco << 1); //calculating window start value
  //loop runs until the current clk value mod with t_esco is equal to d_esco(esco_window_start)
  while (mod_val != esco_info_p->d_esco) {
    if (current_clk & BIT(0)) {
      current_clk++;
    } else {
      current_clk += 2; //incrementing the clk value
    }
    if (current_clk & BIT(26)) {
      current_clk &= ~BIT(26);
    }
    mod_val = (current_clk) % (esco_info_p->t_esco << 1);
  }
  esco_info_p->esco_window_start = current_clk; //updating current_clk value to esco window start
}

/**
 * @brief function handles removal of ESCO role .
 * @param connection_info_p pointer
 */
void stop_esco_role_received_from_host(btc_dev_t *btc_dev_p, btc_trx_configs_t *btc_trx_configs)
{
  /* TODO: need to fill the function with other actions needed*/
  stop_periodic_role(btc_dev_p, btc_trx_configs);
}
