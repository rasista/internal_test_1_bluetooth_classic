/**
 * @file acl_common.c
 * @author  (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#include "btc_device.h"
#include "btc_rail.h"
#include "acl_common.h"
#include "string.h"
#include "lpw_scheduler.h"
#include "lpw_to_host_interface.h"

/**
 * @brief this function will descide which packet to transfer next
 *    and will update the packet headers, function will also update Tx details to RAIL structure
 */
void packetization_acl(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs)
{
  /*TODO: function needs to be updated during horizontal integration with buffer management */
  //  checking queues
  //  updating headers for NEW and RETRANSMIT packets
  //  POLL NULL initiation
  //  adding packet to TXRX config structure

  bt_tx_pkt_t *tx_pkt = get_pkt_from_tx_queue();
  if (tx_pkt == NULL) {
    // 2 bytes data will be updated in structure sent by Rail
    btc_config_tx_pkt(tx_pkt);  // updating header information
  } else {
    btc_config_tx_pkt(tx_pkt);  // updating header information
  }

  btc_trx_configs->tx_pkt = (uint8_t *) tx_pkt;
  btc_trx_configs->tx_window = tx_pkt->pkt_len; //updating packet length
  btc_trx_configs->btc_rail_trx_config.txPower = connection_info_p->tx_pwr_index; //  Updating the tx_power value obtained from host to rail for connected state
}

/**
 * @brief This function will trigger procedures that are based on instances
 *   AFH instant is handled here
 */
void trigger_slot_procedures(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs)
{
  if (connection_info_p->procedure_instant_bitmap) {
    if (IS_BIT_SET(connection_info_p->procedure_instant_bitmap, AFH_INSTANT_PENDING_BIT)) {
      if ((IS_BT_CLOCK_LTE(connection_info_p->afh_new_channel_map_instant, btc_trx_configs->btc_rail_trx_config.clk)) && (!((btc_trx_configs->btc_rail_trx_config.clk) & MASK_2NP0))) {
        // TODO: handle Special Cases, if periodic role is active and is executing before aperiodic, there is a chance for wrong map being used by aperiodic role
        //  copy New AFH channel map to current channel map
        memcpy(connection_info_p->channel_map_in_use, connection_info_p->afh_new_channel_map, CHANNEL_MAP_SIZE);
        connection_info_p->no_of_valid_afh_channels = get_no_of_active_channels((uint8_t *)connection_info_p->afh_new_channel_map);
        // For ADAPT piconet update N to reflect new active channel count immediately
        if (connection_info_p->piconet_type == ADAPTED_PICONET) {
          connection_info_p->btc_fh_data.kosAndN = setBtcFhKosAndN(ADAPT, connection_info_p->no_of_valid_afh_channels, 0);
        }
        CLEAR_BIT(connection_info_p->procedure_instant_bitmap, AFH_INSTANT_PENDING_BIT);
      }
    }
  }
}

/**
 * @brief function handles removal of aperiodic role .
 * @param connection_info_p pointer
 */
void stop_aperiodic_role_received_from_host(btc_dev_t *btc_dev_p)
{
  /* TODO: need to fill the function with other actions needed*/
  stop_aperiodic_role(btc_dev_p);
}

/**
 * @brief function is used to set acl device parameters .
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void set_device_parameters_acl(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_SET_DEVICE_PARAMETERS_ACL_CONNECTION_DEVICE_INDEX(hss_cmd)];

  connection_info->peer_role = *HSS_CMD_SET_DEVICE_PARAMETERS_ACL_PEER_ROLE(hss_cmd);
  connection_info->tx_pwr_index = *HSS_CMD_SET_DEVICE_PARAMETERS_ACL_TX_PWR_INDEX(hss_cmd);
  connection_info->br_edr_mode = *HSS_CMD_SET_DEVICE_PARAMETERS_ACL_BR_EDR_MODE(hss_cmd);
  connection_info->flow = *HSS_CMD_SET_DEVICE_PARAMETERS_ACL_FLOW(hss_cmd);
}

/**
 * @brief function is used to set AFH parameters .
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void set_afh_parameters_acl(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_SET_AFH_PARAMETERS_ACL_CONNECTION_DEVICE_INDEX(hss_cmd)];

  connection_info->afh_new_channel_map_instant = *HSS_CMD_SET_AFH_PARAMETERS_ACL_AFH_NEW_CHANNEL_MAP_INSTANT(hss_cmd);
  connection_info->no_of_valid_afh_channels = *HSS_CMD_SET_AFH_PARAMETERS_ACL_NO_OF_VALID_AFH_CHANNELS(hss_cmd);
  connection_info->piconet_type = *HSS_CMD_SET_AFH_PARAMETERS_ACL_PICONET_TYPE(hss_cmd);
  memcpy(connection_info->channel_map_in_use, HSS_CMD_SET_AFH_PARAMETERS_ACL_CHANNEL_MAP_IN_USE(hss_cmd), CHANNEL_MAP_SIZE);
  memcpy(connection_info->afh_new_channel_map, HSS_CMD_SET_AFH_PARAMETERS_ACL_AFH_NEW_CHANNEL_MAP(hss_cmd), CHANNEL_MAP_SIZE);

  if (connection_info->afh_new_channel_map_instant) {
    SET_BIT(connection_info->procedure_instant_bitmap, AFH_INSTANT_PENDING_BIT);
  }
}
/**
 * @brief function is used to set acl sniff parameters .
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void set_sniff_parameters_acl(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_SET_SNIFF_PARAMETERS_ACL_CONNECTION_DEVICE_INDEX(hss_cmd)];
  connection_info->sniff_info.sniff_interval = *HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_INTERVAL(hss_cmd);
  connection_info->sniff_info.sniff_instant = *HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_INSTANT(hss_cmd);
  connection_info->sniff_info.sniff_attempt = *HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_ATTEMPT(hss_cmd);
  connection_info->sniff_info.sniff_timeout = *HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_TIMEOUT(hss_cmd);
}

/**
 * @brief function is used to update channel related information, selection box input updations
 *
 * @param connection_info-structure of connection_info_sm
 */
void update_channel_info_connected_state(sm_t *connection_info_sm_p)
{
  connection_info_t *connection_info_p = get_connection_info(connection_info_sm_p);
  BtcSeqSelect_t seqSelect;
  uint8_t NVal = 0;
  //Updating sequence selection based on piconet type(BASIC or ADAPT)
  if (connection_info_p->piconet_type == BASIC_PICONET) {
    seqSelect = BASIC;
  } else {
    seqSelect = ADAPT;
    connection_info_p->btc_fh_data.chlMap0Init = *(uint32_t *)(connection_info_p->channel_map_in_use); //10 Bytes being written
    connection_info_p->btc_fh_data.chlMap1Init = *(uint32_t *)(connection_info_p->channel_map_in_use + 4);
    connection_info_p->btc_fh_data.chlMap2Init = *(uint32_t *)(connection_info_p->channel_map_in_use + 8) & 0x7FFF;
    NVal = connection_info_p->no_of_valid_afh_channels;
  }
  connection_info_p->btc_fh_data.kosAndN = setBtcFhKosAndN(seqSelect, NVal, 0);
}

/**
 * @brief function is used to reset the ACL encryption packet counts.
 *
 * @param connection_info - pointer to the connection info structure
 */
void reset_acl_enc_pkt_count(connection_info_t *connection_info)
{
  for (uint32_t i = 0; i < 5; i++) {
    connection_info->acl_enc_tx_pkt_count[i] = 0;
    connection_info->acl_enc_rx_pkt_count[i] = 0;
  }
  // TODO: This will be incremented at every clock round off only in case of eSCO.
  connection_info->day_counter = 0;
  connection_info->zero_len_acl_packet = 0;
}
