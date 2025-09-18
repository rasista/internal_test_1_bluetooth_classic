#include "btc_device.h"
#include "string.h"
#include "arqn_seqn_check.h"
#include "acl_common.h"
#include <stdint.h>
#ifdef SIM
#include "btc_rail.h"
#include "rail_simulation.h"
#include "protocol_btc_rail.h"
#include "host_lpw_interface.h"
#else
#include "btc_rail_intf.h"
#endif
#include "lpw_scheduler.h"
#include "lpw_to_host_interface.h"
#include "debug_logs.h"

uint32_t poll_pkt;
uint32_t null_pkt;

// Lookup table for ACL packet types and their corresponding number of slots
const uint8_t packet_type_slot_lookup_table[NUM_PACKET_TYPES][NUM_DATA_RATES] = {
  [BT_NULL_PKT_TYPE]  = { 1, 1 }, // NULL: 1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_POLL_PKT_TYPE]  = { 1, 1 }, // POLL: 1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_FHS_PKT_TYPE]   = { 1, 1 }, // FHS:  1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_DM1_PKT_TYPE]   = { 1, 1 }, // DM1:  1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_DH1_PKT_TYPE]   = { 1, 1 }, // DH1/2DH1: 1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_HV1_PKT_TYPE]   = { 1, 1 }, // HV1:  1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_HV2_PKT_TYPE]   = { 1, 1 }, // HV2/2EV2:  1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_HV3_PKT_TYPE]   = { 1, 1 }, // HV3/EV3/3EV3: 1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_DV_PKT_TYPE]    = { 1, 1 }, // DV/3DH1: 1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_AUX_PKT_TYPE]   = { 1, 1 }, // AUX: 1 slot at Basic Rate, 1 slot at Enhanced Rate
  [BT_DM3_PKT_TYPE]   = { 3, 3 }, // DM3/2DH3: 3 slots at Basic Rate, 3 slots at Enhanced Rate
  [BT_DH3_PKT_TYPE]   = { 3, 3 }, // DH3/3DH3: 3 slots at Basic Rate, 3 slots at Enhanced Rate
  [BT_EV4_PKT_TYPE]   = { 3, 3 }, // EV4/2EV5: 3 slots at Basic Rate, 3 slots at Enhanced Rate
  [BT_EV5_PKT_TYPE]   = { 3, 3 }, // EV5/3EV5: 3 slots at Basic Rate, 3 slots at Enhanced Rate
  [BT_DM5_PKT_TYPE]   = { 5, 5 }, // DM5/2DH5: 5 slots at Basic Rate, 5 slots at Enhanced Rate
  [BT_DH5_PKT_TYPE]   = { 5, 5 }, // DH5/3DH5: 5 slots at Basic Rate, 5 slots at Enhanced Rate
};

extern shared_mem_t *ulc_llc_share_mem;

/**
 * @brief function used to initialize the device.
 *
 * @param btc_dev_p
 * @param bd_addr
 */
void init_btc_device(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  (void) hss_cmd;
  uint8_t ii;
  /* Initialize a new BD Clock counter module.
   * offset from prototimer clk ticks is used to initialise BD CLock counter
   */
  init_btc_device_handler();
  //init_buffer_pool(); //intialize buffer pool
  memcpy(btc_dev_p->bd_addr, HSS_CMD_INIT_BTC_DEVICE_BD_ADDRESS(hss_cmd), BD_ADDR_LEN);
  memcpy(btc_dev_p->sync_word, HSS_CMD_INIT_BTC_DEVICE_SYNC_WORD(hss_cmd), SYNC_WORD_LEN);
  // deriving LAP from first 3 bytes of bd_address
  btc_dev_p->lap = (btc_dev_p->bd_addr[0] | (btc_dev_p->bd_addr[1] << 8) | (btc_dev_p->bd_addr[2] << 16));

  // deriving UAP from 4th byte of bd_address
  btc_dev_p->uap = btc_dev_p->bd_addr[3];

  // deriving NAP from last 2 bytes of bd_address
  btc_dev_p->nap = (btc_dev_p->bd_addr[4] | (btc_dev_p->bd_addr[5] << 8));

  for (ii = 0; ii < MAX_NUMBER_OF_CONNECTIONS; ii++) {
    btc_trx_configs_t *btc_trx_configs = &btc_dev_p->connection_info[ii].btc_trx_configs_esco;
    btc_trx_configs->is_connected_procedure = TRUE;
    btc_trx_configs->btc_fh_data = &btc_dev_p->connection_info[ii].btc_fh_data;
    btc_trx_configs = &btc_dev_p->connection_info[ii].trx_config;
    btc_trx_configs->is_connected_procedure = TRUE;
    btc_trx_configs->btc_fh_data = &btc_dev_p->connection_info[ii].btc_fh_data;
  }
  btc_dev_p->page.trx_config.btc_fh_data = &btc_dev_p->page.btc_fh_data;
  btc_dev_p->inquiry.trx_config.btc_fh_data = &btc_dev_p->inquiry.btc_fh_data;
  btc_dev_p->inquiry_scan.trx_config.btc_fh_data = &btc_dev_p->inquiry_scan.btc_fh_data;
  btc_dev_p->page_scan.trx_config.btc_fh_data = &btc_dev_p->page_scan.btc_fh_data;
  btc_dev_p->bd_clk_counter_handle.bd_clk_counter_id = BTC_NATIVE_CLK;
}

void btc_common_stop(sm_t *sm, void (*activity_complete)(sm_t *), uint32_t idle_state)
{
  SM_STATE(sm, idle_state);
  /*
     TODO: RAIL_LIB-13704 - https://jira.silabs.com/browse/RAIL_LIB-13704
     Once this JIRA issue is fixed, the RAIL_SEQ_Idle behavior will work as expected
     for properly handling callbacks after radio operations are stopped
   */

  // Immediately abort any pending radio operations and prevent callbacks
  // RAIL_IDLE_ABORT ensures transmit/receive operations are stopped
  // The 'true' parameter makes this call blocking until radio is fully idle
  RAIL_SEQ_Idle(RAIL_IDLE_ABORT, true);
  activity_complete(sm);
}

void btc_activity_complete(sm_t *sm, void (*notify_func)(btc_internal_event_status_t, void *), uint32_t debug_event, uint32_t sm_idle_state)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  debug_log_event(DEBUG_PTI, debug_event);
  notify_func(BTC_ACTIVITY_DURATION_COMPLETED, NULL);
  stop_aperiodic_role(btc_dev_p);
  SM_STATE(sm, sm_idle_state);
  rail_configure_idle();
}

/**
 * @brief function is used to set esco device parameters .
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void set_device_parameters_esco(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_CONNECTION_DEVICE_INDEX(hss_cmd)];

  connection_info->peer_role = *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_PEER_ROLE(hss_cmd);
  connection_info->esco_info.t_esco = *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_T_ESCO(hss_cmd);
  connection_info->esco_info.d_esco = *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_D_ESCO(hss_cmd);
  connection_info->esco_info.w_esco = *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_W_ESCO(hss_cmd);
  connection_info->esco_info.is_esco = *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_IS_ESCO(hss_cmd);
  connection_info->esco_info.is_edr = *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_IS_EDR(hss_cmd);
}

/**
 * @brief function is used to set roleswitch device parameters.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void set_device_parameters_roleswitch(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_START_ROLESWITCH_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd)];

  connection_info->peer_role = *HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_PEER_ROLE(hss_cmd);
  connection_info->lt_addr = *HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_LT_ADDR(hss_cmd);
  connection_info->tx_pwr_index = *HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_TX_PWR_INDEX(hss_cmd);
}

/**
 * @brief function is used to set roleswitch parameters.
 * Should be seen as called HCI request
 *
 * @param hss_cmd_t *hss_cmd
 * @param btc_dev_t *btc_dev_p
 */
void set_roleswitch_parameters(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint32_t current_bd_clk = btc_get_current_bd_clk_cnt(&btc_dev_p->connection_info[btc_dev_p->roleswitch_data.old_connection_idx].trx_config.bd_clk_counter_handle);
  roleswitch_data_t *roleswitch_data = &btc_dev_p->roleswitch_data;
  // get the current clock using that index
  roleswitch_data->old_connection_idx =  *HSS_CMD_SET_ROLESWITCH_PARAMETERS_CONNECTION_DEVICE_INDEX(hss_cmd);
  roleswitch_data->new_connection_idx = roleswitch_data->old_connection_idx + 1; // TODO: get the index from the get_index()
  rail_set_btc_clk_instance(&btc_dev_p->bd_clk_counter_handle);
  roleswitch_data->new_clk_roleswitch_instant = (RAIL_SEQ_BTC_GetBtcDate(btc_dev_p->bd_clk_counter_handle.bd_clk_counter_id) >> CLK_SCALE_FACTOR) + ((HALF_SLOT_FACTOR *  *HSS_CMD_SET_ROLESWITCH_PARAMETERS_ROLESWITCH_INSTANT(hss_cmd)) - current_bd_clk);
  roleswitch_data->new_clk_roleswitch_instant &= ~0x03;  // Clear bits 0 and 1
  rail_set_btc_clk_instance(&btc_dev_p->connection_info[btc_dev_p->roleswitch_data.old_connection_idx].trx_config.bd_clk_counter_handle);
  roleswitch_data->roleswitch_instant = (HALF_SLOT_FACTOR *  *HSS_CMD_SET_ROLESWITCH_PARAMETERS_ROLESWITCH_INSTANT(hss_cmd));
  roleswitch_data->roleswitch_instant &= ~0x03;  // Clear bits 0 and 1
  roleswitch_data->roleswitch_slot_offset = *HSS_CMD_SET_ROLESWITCH_PARAMETERS_ROLESWITCH_SLOT_OFFSET(hss_cmd);
  btc_dev_p->connection_info[roleswitch_data->new_connection_idx].lt_addr = *HSS_CMD_SET_ROLESWITCH_PARAMETERS_NEW_LT_ADDR(hss_cmd);
}

/**
 * @brief function is used to set the encryption parameters for a Bluetooth connection.
 * This function configures either E0 or AES encryption parameters based on the encryption mode
 * specified in the HSS command. For AES encryption, it also resets the ACL encryption packet counters.
 * Should be seen as called HCI request.
 *
 * @param hss_cmd - pointer to the HSS command structure containing encryption parameters
 * @param btc_dev_p - pointer to the global btc device structure
 */
void set_encryption_parameters(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  connection_info_t *connection_info = &btc_dev_p->connection_info[*HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_CONNECTION_DEVICE_INDEX(hss_cmd)];
  connection_info->trx_config.btc_rail_trx_config.encryptLink =  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCRYPTION_MODE(hss_cmd);
  if (connection_info->trx_config.btc_rail_trx_config.encryptLink == ENCRYPTION_MODE_E0) {
    RAIL_SEQ_E0EncryptionConfig_t *e0EncConfig = &connection_info->e0EncConfig;
    memcpy(e0EncConfig, HSS_CMD_PAYLOAD_SET_PROCEDURE_PAYLOAD_ENCRYPTION_PARAMETERS_E0(hss_cmd), sizeof(RAIL_SEQ_E0EncryptionConfig_t));
  } else if (connection_info->trx_config.btc_rail_trx_config.encryptLink == ENCRYPTION_MODE_AES) {
    RAIL_SEQ_AESEncryptionConfig_t *aesEncConfig = &connection_info->aesEncConfig;
    /* TODO: Assuming that the set_encryption_parameters is called only when the encryption is started
     * and won't be triggered for pause and resume encryption.
     */
    reset_acl_enc_pkt_count(connection_info);
    memcpy(aesEncConfig, HSS_CMD_PAYLOAD_SET_PROCEDURE_PAYLOAD_ENCRYPTION_PARAMETERS_AES(hss_cmd), sizeof(RAIL_SEQ_AESEncryptionConfig_t));
  }
}

void update_encryption_parameters(connection_info_t *connection_info, btc_trx_configs_t *btc_trx_configs, uint8_t is_tx, uint8_t direction)
{
  // TODO: Update only the required parameters, integrate once the RAIL provides split APIs.
  if (btc_trx_configs->btc_rail_trx_config.encryptLink == ENCRYPTION_MODE_E0) {
    btc_trx_configs->e0EncConfig->masterClock = btc_trx_configs->btc_rail_trx_config.clk;
  } else if (btc_trx_configs->btc_rail_trx_config.encryptLink == ENCRYPTION_MODE_AES) {
    // TODO: Check is the nonce_type is handled by RAIL using trx_config given while prepareTxRx?
    btc_trx_configs->aesEncConfig->dayCountAndDir = setBtcLmacDayCounter(direction, connection_info->day_counter);
    btc_trx_configs->aesEncConfig->zeroLenAclW = connection_info->zero_len_acl_packet;
    if (is_tx) {
      btc_trx_configs->aesEncConfig->aesPldCntr1 = *(uint32_t *)&connection_info->acl_enc_tx_pkt_count[0];
      btc_trx_configs->aesEncConfig->aesPldCntr2 = connection_info->acl_enc_tx_pkt_count[4];
    } else {
      btc_trx_configs->aesEncConfig->aesPldCntr1 = *(uint32_t *)&connection_info->acl_enc_rx_pkt_count[0];
      btc_trx_configs->aesEncConfig->aesPldCntr2 = connection_info->acl_enc_rx_pkt_count[4];
    }
  }
}

/**
 * @brief function is used to increment a 5-byte packet counter by 1.
 * Treats the 5 bytes as a single 40-bit counter with little-endian representation.
 * Handles overflow by wrapping around to 0 when reaching maximum value.
 *
 * @param pkt_count - pointer to the 5-byte counter array (uint8_t[5])
 */
void update_acl_enc_pkt_count(uint8_t encryption_mode, uint8_t *pkt_count)
{
  if (encryption_mode == ENCRYPTION_MODE_AES) {
    // Increment the packet count by 1
    if (*(uint32_t *)&pkt_count[0] == 0xFFFFFFFF) {
      *(uint32_t *)&pkt_count[0] = 0x0;
      pkt_count[4]++;
    } else {
      *(uint32_t *)&pkt_count[0] += 1;
    }
  }
}

const aperiodic_puncture_restore_lut_t aperiodic_puncture_restore_lut[] = {
  { aperiodic_idle }, { aperiodic_restore_tx_abort }, { aperiodic_restore_rx_abort }
};

/**
 * @brief Function is used to update packet header fields related to arqn details in the packet, used before schedule tx.
 * function used for defining packet length.
 *
 * @param bt_tx_pkt_t
 *
 */
void btc_config_tx_pkt(bt_tx_pkt_t *tx_pkt) //  TODO : During Horizontal Integration , this fucntion would be completely addressed
{
  tx_pkt->pkt_len = BTC_TX_PKT_LEN;
  //  TODO: uncomment this part, To use the initialisations of packet header variables during integrations
  /*arqn_seqn_scheme_status_t arqn_update; //  TODO: Temporary Variable, Will be properly addressed in Horizontal integration
     header_t bt_tx_hdr; //  TODO: Structure avariable is temporary, as the header will be from the Rx packet directly
     connection_info_t connection_info;
     bt_tx_hdr.arqn    = arqn_update.arqn_status; //  Arqn bit update
     bt_tx_hdr.seqn    = arqn_update.seqn_status; //  Seqn bit update
     bt_tx_hdr.lt_addr = connection_info.lt_addr;
     bt_tx_hdr.flow    = connection_info.flow; // accessing packet header structure members
     bt_tx_hdr.type    = BT_NULL_PKT_TYPE; //  TODO : Type field needs to be updated based on packet type we are transmitting
   */
}

bt_tx_pkt_t tx_q_pkt;
bt_tx_pkt_t *get_pkt_from_tx_queue()
{
  // definition will be updated after Queue mechanism is finalised,[WIP]
  // return NULL;
  return &tx_q_pkt;
}

/**
 * @brief function handles the scheduling of TxRx
 *
 * @param btc_trx_configs
 *
 * @return uint32_t
 */
uint32_t schedule_tx_rx_activity(btc_trx_configs_t *btc_trx_configs)
{
  uint32_t status = BTC_RAIL_PASS_STATUS;
  btc_trx_configs_t *btc_trx_configs_tmp = btc_trx_configs;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  lpw_scheduler_info_t *lpw_scheduler_info = &btc_dev_p->lpw_scheduler_info;

  /*
   * Aperiodic role will schedule_tx/rx in below cases
   * 1. for the first time from start procedure
   * 2. during continuous tx,rx
   * 3. after restoring from being punctured by periodic roles
   * */
  /*
   * Periodic role will schedule_tx/rx in below cases
   * 1. for the first time from start procedure for first periodic instance
   * 2. during continuous tx,rx within a window
   * 3. for next interval start of window
   * */

  if (btc_dev_p->l2_scheduled_sm == NULL) {
    /* No role is scheduled/configured to RAIL */

    if (IS_CONF_FLAG_PERIODIC(btc_trx_configs->flags)) {
      btc_dev_p->l2_scheduled_sm = btc_trx_configs->sm;
      lpw_scheduler_info->l2_sched_periodic_btc_trx_configs = btc_trx_configs;
    } else {
      btc_dev_p->l2_scheduled_sm = lpw_scheduler_info->l1_scheduled_sm;
      lpw_scheduler_info->l2_sched_aperiodic_btc_trx_configs = btc_trx_configs;
    }
  } else if (btc_dev_p->l2_scheduled_sm != lpw_scheduler_info->l1_scheduled_sm) {
    /* Periodic role is already configured to RAIL*/

    if (IS_CONF_FLAG_PERIODIC(btc_trx_configs->flags)) {
      /* check if configured and requested are same,
         if yes continue with configuration to RAIL,
         else, check if queue iteration is needed based on TSFs (if new is farther, add to queue,else add old to queue, exeute new )  */

      // if btc_dev_p->l2_scheduled_sm == btc_trx_configs->sm,  continue with current request, same periodic role is executed again

      if (btc_dev_p->l2_scheduled_sm == btc_trx_configs->sm) {
        if (IS_CONF_FLAG_NEW_WINDOW(btc_trx_configs->flags)) {
          RESET_CONF_FLAG_NEW_WINDOW(btc_trx_configs->flags);
          add_periodic_role_to_periodic_queue(btc_dev_p, btc_trx_configs);
          //  Check if the queue has a role to schedule
          if (btc_dev_p->lpw_scheduler_info.lpw_sch_periodic_queue.head) {
            lpw_scheduler_info->l2_sched_periodic_btc_trx_configs = btc_dev_p->lpw_scheduler_info.lpw_sch_periodic_queue.head;
          } else {
            //  Fall back to dequeuing if no role is present in the queue
            lpw_scheduler_info->l2_sched_periodic_btc_trx_configs = NULL;
          }
          btc_dev_p->l2_scheduled_sm = lpw_scheduler_info->l2_sched_periodic_btc_trx_configs->sm;
          btc_trx_configs = lpw_scheduler_info->l2_sched_periodic_btc_trx_configs;
        }
      } else { /* Likely that new periodic role is added */
        if ( lpw_scheduler_info->l2_sched_periodic_btc_trx_configs->btc_rail_trx_config.tsf <= btc_trx_configs->btc_rail_trx_config.tsf) {
          // adding new periodic role request to queue, will continue with current active periodic role
          add_periodic_role_to_periodic_queue(btc_dev_p, btc_trx_configs);
          return BTC_RAIL_PASS_STATUS;//  TODO
        } else {
          /* Will proceed below and configure to RAIL */
          // adding old periodic role to queue, will continue with new periodic role RAIL configuration , TODO: priority check
          add_periodic_role_to_periodic_queue(btc_dev_p, lpw_scheduler_info->l2_sched_periodic_btc_trx_configs);
          btc_dev_p->l2_scheduled_sm = btc_trx_configs->sm;
          lpw_scheduler_info->l2_sched_periodic_btc_trx_configs = btc_trx_configs;
        }
      }
    } else { /* Aperiodic role is requesting */
             /* if periodic TSF is farther(TODO: need to define this, best is to check overlapping),
              *     then move periodic to queue, execute aperiodic activity
              * else
              *    send success to Aperiodic, and mark it as punctured.(Aperiodic will be handled after periodic activity)
              * */
      if ((BTC_TSF_DIFF(lpw_scheduler_info->l2_sched_periodic_btc_trx_configs->btc_rail_trx_config.tsf, btc_get_current_clk())) > APERIODIC_TO_PERIODIC_THRESHOLD_US ) { /* Periodic is farther than threshold */                                                                                                                        // adding active periodic role to queue, will continue with Aperiodic role RAIL configuration
        add_periodic_role_to_periodic_queue(btc_dev_p, lpw_scheduler_info->l2_sched_periodic_btc_trx_configs);
        btc_dev_p->l2_scheduled_sm = btc_trx_configs->sm;
        lpw_scheduler_info->l2_sched_aperiodic_btc_trx_configs = btc_trx_configs;
      } else {
        /* Requesting aperiodic is punctured, Periodic configured is left as t is, as the role is active */
        if (IS_CONF_FLAG_TX(btc_trx_configs->btc_rail_trx_config.isRx)) {
          btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured = APERIODIC_TX_PUNCTURED;
        } else {
          btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured = APERIODIC_RX_PUNCTURED;
        }
        return BTC_RAIL_PASS_STATUS;
      }
    }
  } else if (btc_dev_p->l2_scheduled_sm == lpw_scheduler_info->l1_scheduled_sm) {
    /* Aperiodic role is already configured to RAIL */
    /* TODO:future corner case,   there is fair chance, that RAIL was configured with APERIODIC, PERIODIC comes and exeutes,
     * which will cause troublesi from RAIL handling if interrupt already available by now, need brain storming */
    if (IS_CONF_FLAG_PERIODIC(btc_trx_configs->flags)) {
      /* This must be new Periodic activity triggered from HostSS */

      /* Add the new periodic role to queue
       * if periodic queue head TSF is farther(TODO: need to define this, best is to check overlapping),
       *     then move periodic to queue, execute aperiodic activity
       * else
       *     puncture Aperiodic role, execute periodic role
       * */

      // adding new periodic role request to queue, will continue with current active periodic role
      add_periodic_role_to_periodic_queue(btc_dev_p, btc_trx_configs);

      //  if periodic needs immediate execution not waiting for APERIODIC RAIL triggers
      if (btc_trx_configs->btc_rail_trx_config.tsf <= btc_dev_p->lpw_scheduler_info.aperiodic_end_tsf) {
        if (IS_CONF_FLAG_TX(btc_trx_configs->btc_rail_trx_config.isRx)) {
          btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured = APERIODIC_TX_PUNCTURED;
        } else {
          btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured = APERIODIC_RX_PUNCTURED;
        }
        btc_trx_configs = lpw_scheduler_info->l2_sched_periodic_btc_trx_configs = dequeue_periodic_role_from_periodic_queue();
        btc_dev_p->l2_scheduled_sm = lpw_scheduler_info->l2_sched_periodic_btc_trx_configs->sm;
      } else {
        return BTC_RAIL_PASS_STATUS;
      }
    } else {
      // Only aperiodic roles will enter in this condition and execute periodic roles
      /* Check if Aperiodic is overlapping with Periodic activities */
      if (check_aperiodic_role_overlapping_with_periodic_start(btc_dev_p, btc_trx_configs)) {
        if (IS_CONF_FLAG_TX(btc_trx_configs->btc_rail_trx_config.isRx)) {
          btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured = APERIODIC_TX_PUNCTURED;
        } else {
          btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured = APERIODIC_RX_PUNCTURED;
        }
        /* Aperiodic is overlapping with Periodic in queue
         *
         * Puncture Aperiodic, execute Periodic configuration to RAIL */
        btc_trx_configs = lpw_scheduler_info->l2_sched_periodic_btc_trx_configs = dequeue_periodic_role_from_periodic_queue();
        btc_dev_p->l2_scheduled_sm = lpw_scheduler_info->l2_sched_periodic_btc_trx_configs->sm;
      }
    }
  }

  debug_scheduled_tsf_expired_check(btc_trx_configs);
// Below code is to configure rail with current active TX or Rx
  if (IS_CONF_FLAG_TX(btc_trx_configs->btc_rail_trx_config.isRx)) {
    if (IS_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType)) {
      //  Schedule ID packet TX
      status = btc_schedule_id_tx(btc_trx_configs);
    } else {
      //  This function triggers RAIL API
      status = btc_schedule_tx(btc_trx_configs);
    }
    if (IS_CONF_FLAG_APERIODIC(btc_trx_configs->flags)) {
      btc_dev_p->lpw_scheduler_info.aperiodic_end_tsf = (btc_trx_configs->btc_rail_trx_config.tsf) + (MAX_RADIO_USAGE_IN_US);
    }
  } else {
    //  Schedule ID packet TX
    //  scan_window is validated in theupper link layer
    if (IS_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType)) {
      //  Schedule ID RX packet
      status   = btc_schedule_id_rx(btc_trx_configs);
    } else {
      btc_trx_configs->rx_window = BTC_RAIL_RX_TIMEOUT;
      status = btc_schedule_rx(btc_trx_configs);
    }
    if (IS_CONF_FLAG_APERIODIC(btc_trx_configs->flags)) {
      btc_dev_p->lpw_scheduler_info.aperiodic_end_tsf = btc_trx_configs->btc_rail_trx_config.tsf + MAX_RADIO_USAGE_IN_US;
    }
  }

  if (btc_trx_configs_tmp != btc_trx_configs) {
    /* there is a change in role that requested for schedule tx */
    if (status != BTC_RAIL_PASS_STATUS) {
      handle_overriding_role_abort_status(btc_trx_configs);
    }
    status = BTC_RAIL_PASS_STATUS;
  }
  return status;
}

uint32_t schedule_tx(btc_trx_configs_t *btc_trx_configs)
{
  SET_CONF_FLAG_TX(btc_trx_configs->btc_rail_trx_config.isRx);
  return schedule_tx_rx_activity(btc_trx_configs);
}

/**
 * @brief function handles the scheduling of Rx along with prioritizing overlapping roles
 *
 * @param btc_trx_configs
 *
 * @return uint32_t
 */
uint32_t schedule_rx(btc_trx_configs_t *btc_trx_configs)
{
  SET_CONF_FLAG_RX(btc_trx_configs->btc_rail_trx_config.isRx);
  return schedule_tx_rx_activity(btc_trx_configs);
}

/**
 * @brief function to handle events from lpw_ss to host_ss
 *
 * @param buffer
 */
void lpw_procedure_complete_event(uint8_t *buffer, uint16_t buff_len)
{
  (void)buffer;
  (void)buff_len;
  // rail_lpw_procedure_complete_event(buffer, buff_len);
}

/*
 * @brief Configures all slot related parameters that are needed to configure RAIL
 *
 * */
void configure_conn_slot_params(connection_info_t *connection_info_p, btc_bd_clk_identifier_t btc_bd_clk_identifier, btc_trx_configs_t *btc_trx_configs)
{
  //  get physical channel configurations for TXRX activity
  btc_get_txrx_clk_configs(btc_bd_clk_identifier, btc_trx_configs);
  memcpy(connection_info_p->lap_address, btc_trx_configs->lap_addr, LAP_ADDR_LEN);
  // Triggers slot based procedures
  trigger_slot_procedures(connection_info_p, btc_trx_configs);
  //  get target channel on the basis of procedure and target BD clk
  //  FIXME rail_get_channel should require BD address as input parameter
  //  TODO: will be updated during CSA horizontal integration
}

/*
 *
 * @ Brief compare clock values with rollover safety
 * */
uint8_t comp_clock(uint32_t clock1, uint32_t clock2)
{
  uint32_t clk_diff = BT_SUB_CLOCK(clock1, clock2);

  if (clk_diff == 0) {
    /* Both clocks are equal */
    return EQL_CLKS;
  } else if (clk_diff < (BT_CLOCK_MASK >> 1)) {
    /* Clock 1 is greater */
    return CLK1_GT_CLK2;
  } else {
    /* Clock 2 is greater */
    return CLK1_LT_CLK2;
  }
}

/*
 * @brief This function is used to configure the slot parameters of non connected states for TX/RX activities
 *      This function fills the values of the varaiables/members of the structure btc_trx_configs_t
 *
 */
void configure_nonconn_slot_params(btc_trx_configs_t *btc_trx_configs, btc_bd_clk_identifier_t btc_bd_clk_identifier)
{
  //  get physical channel configurations for TXRX activity
  btc_get_txrx_clk_configs(btc_bd_clk_identifier, btc_trx_configs);
}

void configure_nonconn_slot_params_clk(btc_trx_configs_t *btc_trx_configs, btc_bd_clk_identifier_t btc_bd_clk_identifier)
{
  //  get physical channel configurations for TXRX activity
  btc_get_txrx_clk_configs_with_clk(btc_trx_configs->btc_rail_trx_config.clk, btc_bd_clk_identifier, btc_trx_configs);
}

/*
 * @brief This function is used to configure the slot parameters for esco state machine w.r.t to window start
 *      This function fills the values of the varaiables/members of the structure btc_trx_configs_t
 *
 */
void configure_conn_slot_params_for_esco_window_start(connection_info_t *connection_info_p,
                                                      btc_trx_configs_t *btc_trx_configs,
                                                      esco_info_t *esco_info_p)
{
  uint32_t target_time;
  //  Calculate the time using the ESCO interval
  target_time = esco_info_p->esco_window_start;

  //  Get physical channel configurations for TXRX activity using the target time
  btc_get_txrx_clk_configs_with_target_time(btc_trx_configs,
                                            target_time);
  memcpy(connection_info_p->lap_address, btc_trx_configs->lap_addr, LAP_ADDR_LEN);
  //  Triggers slot based procedures
  trigger_slot_procedures(connection_info_p, btc_trx_configs);

  //  Get target channel on the basis of procedure and target BD clk
  //  FIXME rail_get_channel should require BD address as input parameter
  //  TODO: will be updated during CSA horizontal integration
}

void update_channel_info(btc_trx_configs_t *btc_trx_configs, RAIL_SEQ_BtcFhConfig_t *btcFhData)
{
  btcFhData->masterClk = btc_trx_configs->btc_rail_trx_config.clk; // clk updation
  update_btc_channel_number(btc_trx_configs, btcFhData);
}

/*
 * @brief This function is used to configure channel, channel selection function is being called ,which returns channel number
 *
 */
void update_btc_channel_number(btc_trx_configs_t *btc_trx_configs, RAIL_SEQ_BtcFhConfig_t *btcFhData)
{
  RAIL_SEQ_BTC_FhCopro(btcFhData, &btc_trx_configs->btcFhResult);
}

/*
 * @brief This function will update the FHS or EIR packet to Tx packet.
 *  This function is used to form packets for non connected states
 *
 */
void packetization_nonconn(btc_trx_configs_t *btc_trx_configs, uint8_t is_fhs_pkt)
{
  (void)is_fhs_pkt;
  /*TODO: packet handling is handled along with buffer management integration.
     This function will be completed during the integration of buffer management procedure */
  bt_tx_pkt_t *tx_pkt = get_pkt_from_tx_queue();
  btc_config_tx_pkt(tx_pkt); // updating header information
  btc_trx_configs->tx_window = tx_pkt->pkt_len;
  // TODO
  btc_trx_configs->tx_pkt = (uint8_t*) tx_pkt;
}

/*
 *@brief function is used to count the number of set (1) bits, can be reused to check the bits which are set
 */
uint8_t get_no_of_active_channels(uint8_t *addr)
{
  uint8_t active_channel_count = 0;
  uint8_t bit_pos;

  for (bit_pos = 0; bit_pos < BTC_CHANNELS_COUNT; bit_pos++) {
    if (addr[bit_pos >> 3] & BIT(bit_pos & 0x7)) {
      active_channel_count++;
    }
  }
  return active_channel_count;
}

/*
 * @brief This function is to send aperiodic roles aborts that are punctured due to periodic roles
 *
 * */
void restore_aperiodic_role_punctured()
{
/* get BTC DEV from function call and check if aperiodic is punctured, if yes, send ABORT respective to tx, or Rx
 * */
  btc_dev_t *btc_dev_p =  BTC_DEV_PTR;
  aperiodic_puncture_restore_lut[btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured].handler(btc_dev_p);
  btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured = APERIODIC_NOT_PUNCTURED;
}

/*
 * @brief This function is to track configurations with expired TSF
 *
 * */
void debug_scheduled_tsf_expired_check(btc_trx_configs_t *btc_trx_configs)
{
  if (btc_trx_configs->btc_rail_trx_config.tsf < btc_get_current_clk()) {
    //  Configuring Tx,Rx for expired TSF
    // printf("Debug, TSF expired , in  ASSERTION ");
    // TODO: uncomment below
    // BTC_ASSERT("INVALID");
  }
}

/*
 * @brief This function is to handle aborts for overridign roles, as return should be sent PASS for originally triggered handle
 *
 * */
void  handle_overriding_role_abort_status(btc_trx_configs_t *btc_rail_txrx_config)
{
  //  Trigger Abort status to executed scheduled role
  //  need to call STEP for ABORT over here
  SM_STEP(SM, btc_rail_txrx_config->sm, (IS_CONF_FLAG_TX(btc_rail_txrx_config->btc_rail_trx_config.isRx)) ? TX_ABORT : RX_ABORT);
  /* TODO:  Aperiodic role overridden over here, is still in hanging state, now Periodic role if abort state is handled, it might configure for TSF with delayed execution.
   * But, periodic role while configuring for farther in time, it might call Abort from the aperiodic role. All happenns in the same instance where we are supposed to give success for triggering abort later.
   * Solution 1:
   *    Need to ignore the sending of PASS_STATUS in the aperiodic activity initially triggered. No activity after schedule Tx, Rx needs to be happening after handling PASS/FAIL_STATUS.
   * Solution 2:
   *    Need brainstorming
   * */
}

/*
 * @brief This function is to handle IDLE LUT
 *
 * */
void aperiodic_idle(btc_dev_t *btc_dev_p)
{
  (void)btc_dev_p;
  /* NO activity*/
}

/*
 * @brief This function is to handle restoring Tx abort
 *
 * */
void aperiodic_restore_tx_abort(btc_dev_t *btc_dev_p)
{
  btc_dev_p->l2_scheduled_sm = btc_dev_p->lpw_scheduler_info.l1_scheduled_sm;
  SM_STEP(SM, btc_dev_p->lpw_scheduler_info.l1_scheduled_sm, TX_ABORT);
}

/*
 * @brief This function is to handle restoring Rx abort
 *
 * */
void aperiodic_restore_rx_abort(btc_dev_t *btc_dev_p)
{
  btc_dev_p->l2_scheduled_sm = btc_dev_p->lpw_scheduler_info.l1_scheduled_sm;
  SM_STEP(SM, btc_dev_p->lpw_scheduler_info.l1_scheduled_sm, RX_ABORT);
}

/*
 * @brief function to indicate that procedure activity is complete and indicate to ull
 * This function is called by ACL prodedure
 */
void procedure_activity_complete(sm_t *connection_info_sm, uint8_t state)
{
  uint8_t *buffer = NULL;
  pkb_t *pkb;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;

  stop_aperiodic_role(btc_dev_p);
  pkb = get_buffer_for_notification(&buffer);
  prep_hss_internal_event(buffer, BTC_ACTIVITY_COMPLETE, BTC_ACTIVITY_DURATION_COMPLETED);
  btc_lpw_activity_complete_indication_to_ull(pkb);
  SM_STATE(connection_info_sm, state);
  debug_log_event(DEBUG_PTI, PTI_PROCEDURE_ACTIVITY_COMPLETED);
  rail_configure_idle();
}

/*
 * @brief function to check CRC status.
 *
 */
uint8_t is_crc_check_passed()
{
  if (btc_get_crc_status() == TRUE) {
    return TRUE;
  }
  return FALSE;
}

/*
 * @brief function to check HEC status.
 *
 */
uint8_t is_hec_check_passed()
{
  if (btc_get_hec_status() == TRUE) {
    return TRUE;
  }
  return FALSE;
}

void configure_trx_acl_common(btc_trx_configs_t *btc_trx_configs, uint8_t rate)
{
  btc_trx_configs->btc_rail_trx_config.linkConfig.acl = 1;
  btc_trx_configs->btc_rail_trx_config.specialPktType = SL_RAIL_BTC_NORMAL_PKT;
  btc_trx_configs->btc_rail_trx_config.linkConfig.enhancedRate = rate;
}

static void configure_trx_acl(btc_trx_configs_t *btc_trx_configs, uint8_t rate, uint8_t pkt_type, uint16_t pkt_len)
{
  configure_trx_acl_common(btc_trx_configs, rate);
  btc_trx_configs->btc_rail_trx_config.packetConfig.packetType = pkt_type; // Packet type for NULL
  btc_trx_configs->btc_rail_trx_config.txPacketLen = pkt_len; // in bytes
}

void configure_trx_acl_rx(btc_trx_configs_t *btc_trx_configs, uint8_t rate, uint32_t rxTimeout)
{
  configure_trx_acl_common(btc_trx_configs, rate);
  btc_trx_configs->btc_rail_trx_config.rxTimeout = rxTimeout;
}

void form_fhs_pkt(btc_trx_configs_t *btc_trx_configs, uint32_t *fhs_pkt, uint8_t lt_addr, uint8_t old_lt_addr)
{
  btc_dev_t *dev = BTC_DEV_PTR;
  uint16_t *fhs_packet = (uint16_t *)fhs_pkt;
  uint8_t *class_of_dev = dev->class_of_dev;
  uint32_t clock = 0;
  uint8_t eir = 0;
  uint8_t sp = BTC_DEFAULT_SP_IN_FHS; // Protocol Mandatory
  uint8_t sr_mode = 0;
  uint8_t resrved = 0;

  btc_trx_configs->tx_pkt = (uint8_t *)fhs_packet;
  if ((dev->l2_scheduled_sm->current_state_machine == inquiry_scan_sm_context)) {   //inquiry scan state
    lt_addr = 0;
    clock = btc_get_current_clk() + 2; //clock adjustments for extended rx window as packet arrival can be in any of these slots 2np0, 2np1, or 2np2.
  } else if ((dev->l2_scheduled_sm->current_state_machine == page_sm_context)) {
    clock = btc_trx_configs->btc_rail_trx_config.clk;
  }

  // Filling header of FHS Packet
  fhs_packet[0] = FHS_RESERVED_FIELD;      // Storing  (0x290)
  if (dev->l2_scheduled_sm->current_state_machine == roleswitch_peripheral_sm_context) {
    clock = btc_trx_configs->btc_rail_trx_config.clk;
    // Update the header of the FHS packet with old LT address
    fhs_packet[0] = fhs_packet[0] | old_lt_addr;
  }
  *(uint32_t *)&fhs_packet[1] = dev->sync_word[0];
  if ((dev->l2_scheduled_sm->current_state_machine == inquiry_scan_sm_context) && (dev->inquiry_scan.seqSelect == IQRS)) { //inquiry response state
    eir = dev->inquiry_scan.EIR_value ? 1 : 0;
  } else if ((dev->inquiry_scan.seqSelect) == PGRSM) {
    sr_mode = (dev->page.remote_scan_rep_mode);
  }

  *(uint32_t *)&fhs_packet[3] = CONFIGURE_FHS_HEADER(resrved, sr_mode, sp, eir, dev->lap, dev->sync_word[1]);
  *(uint32_t *)&fhs_packet[5] = CONFIGURE_COD_NAP_UAP(class_of_dev[0], dev->nap, dev->uap);
  fhs_packet[7] = ((class_of_dev[2] << 8) | class_of_dev[1]);    // Second & third  byte of class of device
  *(uint32_t *)&fhs_packet[8] = CONFIGURE_MODE_CLOCK_LTADDR(MANDATORY_SCAN_MODE, clock, lt_addr);

  configure_trx_acl(btc_trx_configs, BASIC_RATE, BT_FHS_PKT_TYPE, BT_FHS_PKT_LEN);
}

void update_fhs_pkt_clk(btc_trx_configs_t *btc_trx_configs, uint32_t *fhs_pkt, uint8_t lt_addr, uint32_t clk)
{
  uint16_t *fhs_packet = (uint16_t *)fhs_pkt;
  btc_trx_configs->tx_pkt = (uint8_t *)fhs_packet;
  *(uint32_t *)&fhs_packet[8] = CONFIGURE_MODE_CLOCK_LTADDR(MANDATORY_SCAN_MODE, clk, lt_addr);
  configure_trx_acl(btc_trx_configs, BASIC_RATE, BT_FHS_PKT_TYPE, BT_FHS_PKT_LEN);
}

void llc_process_rx_fhs_pkt(uint32_t *fhs_pkt, btc_fhs_rx_pkt_t *fhs_rx_pkt, uint8_t *remote_bd_address)
{
  uint16_t *fhs_packet = (uint16_t *)fhs_pkt;
  uint32_t clk_in_fhs_pkt = 0;
  uint8_t *fhs_pkt_8 = (uint8_t *)fhs_pkt;
  uint8_t barker_seq;
  uint16_t *access_code = (uint16_t *)fhs_rx_pkt->rx_sync_word;
  // TODO: Update connection_info.
  // Move the complete FHS parsing apart from required fields to ULC. This simplifies LLC.
  // TODO: Handle slave_id properly.

  // Extract Clock in FHS
  clk_in_fhs_pkt = ((fhs_packet[7] >> 1) & MASK_15_BITS); // Lower 15 bits of the clock
  clk_in_fhs_pkt |= ((fhs_packet[8] & MASK_13_BITS) << 15); // Upper 13 bits of the clock
  clk_in_fhs_pkt &= ~0x3;

  // Store the clock from FHS packet
  fhs_rx_pkt->fhs_rx_clk = clk_in_fhs_pkt;

  // Extracting LAP
  fhs_rx_pkt->fhs_rx_lap_addr[0] = EXTRACT_LAP_ADDR_BYTE(*((uint16_t *)(&fhs_pkt_8[4])));
  fhs_rx_pkt->fhs_rx_lap_addr[1] = EXTRACT_LAP_ADDR_BYTE(*((uint16_t *)(&fhs_pkt_8[5])));
  fhs_rx_pkt->fhs_rx_lap_addr[2] = EXTRACT_LAP_ADDR_BYTE(*((uint16_t *)(&fhs_pkt_8[6])));

  // Form complete BD address from LAP, UAP, and NAP
  remote_bd_address[0] = fhs_rx_pkt->fhs_rx_lap_addr[0];
  remote_bd_address[1] = fhs_rx_pkt->fhs_rx_lap_addr[1];
  remote_bd_address[2] = fhs_rx_pkt->fhs_rx_lap_addr[2];
  remote_bd_address[3] = fhs_pkt_8[8];
  remote_bd_address[4] = fhs_packet[4] >> 8;
  remote_bd_address[5] = fhs_packet[5] & 0xFF;

  // Extract LT address
  fhs_rx_pkt->lt_addr = fhs_packet[7] & MASK_3_BITS;

  // // Extract 64-bit sync word
  if (fhs_rx_pkt->fhs_rx_lap_addr[2] & BIT(7)) {
    barker_seq = 0x13;
  } else {
    barker_seq  = 0x2C;
  }
  *(uint32_t *)(access_code)     = *(fhs_pkt);
  *(uint32_t *)(access_code + 2) = *(fhs_pkt + 1);
  access_code[3] &= 0x03FF;
  access_code[3] |= (((uint16_t)barker_seq) << 10);

  // TODO: Based on state handle EIT bit
}

void form_poll_and_null_pkt(btc_trx_configs_t *btc_trx_configs, uint8_t lt_addr, uint8_t pkt_type)
{
  uint32_t *pkt_ptr = NULL;

  if (pkt_type == BT_POLL_PKT_TYPE) {
    pkt_ptr = &poll_pkt;
    *pkt_ptr = FIRST_CONN_POLL_PKT | lt_addr;
  } else {
    pkt_ptr = &null_pkt;
    *pkt_ptr = FIRST_CONN_NULL_PKT | lt_addr;
  }
  btc_trx_configs->tx_pkt = (uint8_t *)pkt_ptr;
  configure_trx_acl(btc_trx_configs, BASIC_RATE, pkt_type, BT_TX_PKT_HDR_LEN);
}

// Common helper functions to reduce code duplication
connection_info_t* get_connection_info(sm_t *connection_info_sm)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  return &btc_dev_p->connection_info[connection_info_sm->current_device_index];
}

void form_null_arqn_flow(btc_trx_configs_t *btc_trx_configs, connection_info_t *connection_info_p)
{
  uint32_t *pkt_ptr = &null_pkt;
  arqn_seqn_scheme_status_t *arqn_seqn_status = &connection_info_p->acl_arqn_seqn_info;
  btc_trx_configs->tx_pkt = (uint8_t *)pkt_ptr;

  *pkt_ptr = CONN_NULL_PKT | connection_info_p->lt_addr | connection_info_p->flow << 7 | arqn_seqn_status->arqn_status << 8;
  configure_trx_acl(btc_trx_configs, BASIC_RATE, BT_NULL_PKT_TYPE, BT_TX_PKT_HDR_LEN);
}

void tx_packetization(connection_info_t *connection_info, btc_trx_configs_t *btc_trx_configs, uint8_t* tx_pkt, uint16_t tx_pkt_len)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  uint16_t* pkt_hdr = (uint16_t*)tx_pkt;
  uint8_t pkt_type = GET_RX_PKT_HEADER_TYPE(*pkt_hdr);
  uint8_t payload_len = 0;

  // Update zero_len_acl_packet flag based on payload length
  if (connection_info->encryption_mode == ENCRYPTION_MODE_AES) {
    uint8_t num_slots = packet_type_slot_lookup_table[pkt_type][connection_info->br_edr_mode];
    uint8_t llid = EXTRACT_LLID(&tx_pkt[2]);
    if (llid == ACL_LLID_CONTINUATION_FRAGMENT) {
      if (num_slots == 1) {
        payload_len = EXTRACT_EIR_SINGLE_SLOT_PAYLOAD_LENGTH(tx_pkt[3]);
      } else {
        payload_len = EXTRACT_EIR_MULTI_SLOT_PAYLOAD_LENGTH(&tx_pkt[3]);
      }
      connection_info->zero_len_acl_packet = (payload_len == 0) ? 1 : 0;
    }
  }
  btc_dev_p->current_slot_info.current_role_lt_addr = connection_info->lt_addr; // TODO: Handle ACL/eSCO/SCO runnning together

  // Config the Tx packet header
  tx_pkt[0] = tx_pkt[0] | (connection_info->flow /*FLOW*/ << 7);
  tx_pkt[1] = tx_pkt[1] | (connection_info->acl_arqn_seqn_info.arqn_status) | (connection_info->acl_arqn_seqn_info.seqn_status << 1);

  // Update the trx config with tx_pkt information
  btc_trx_configs->tx_pkt = tx_pkt;

  configure_trx_acl(btc_trx_configs, connection_info->br_edr_mode, pkt_type, tx_pkt_len);
  // Set the arqn status to ACK for the next packet to convery that the current packet requires an ACK
  connection_info->acl_arqn_seqn_info.arqn_status = ACK;
#ifdef SIM
  btc_trx_configs->tx_window = tx_pkt_len; // TODO: Will be removed
#endif
}

void send_tx_pkt_ack_notification_to_ulc()
{
  uint8_t* buffer = NULL;
  pkb_t *pkb;
  pkb = get_buffer_for_notification(&buffer);
  prep_hss_internal_event(buffer, BTC_TX_PKT_ACK_RECEIVED, 0);
  btc_lpw_activity_complete_indication_to_ull(pkb);
}

void set_fixed_btc_channel(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint8_t channel = *HSS_CMD_SET_FIXED_BTC_CHANNEL_CHANNEL(hss_cmd);
  btc_dev_p->fixed_btc_channel = channel;
  RAIL_SEQ_SetChannel(channel); // Set the channel for the device
  btc_dev_p->disable_channel_hopping = 1; // Disable channel hopping for the device
}

void get_roleswitch_parameter_slot_offset(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  (void)hss_cmd;
  (void)btc_dev_p;
  uint8_t* buffer = NULL;
  hss_event_btc_slot_offset_t roleswitch_slot_offset_event;
  pkb_t *pkb;
  uint8_t current_device_index = *HSS_CMD_SET_ROLESWITCH_PARAMETERS_CONNECTION_DEVICE_INDEX(hss_cmd);
  uint8_t peer_role = btc_dev_p->connection_info[current_device_index].peer_role;
  uint16_t roleswitch_slot_offset = RAIL_SEQ_BTC_SlotOffsetUs(btc_dev_p->connection_info[btc_dev_p->roleswitch_data.old_connection_idx].trx_config.bd_clk_counter_handle.bd_clk_counter_id, btc_dev_p->bd_clk_counter_handle.bd_clk_counter_id);
  roleswitch_slot_offset_event.roleswitch_slot_offset = roleswitch_slot_offset;
  pkb = get_buffer_for_notification(&buffer);
  prep_hss_internal_event(buffer, BTC_CLK_SLOT_OFFSET_NOTIFICATION, BTC_ACTIVITY_DURATION_COMPLETED);
  prep_hss_roleswitch_slot_offset_event(buffer, &roleswitch_slot_offset_event, peer_role);
  btc_lpw_activity_complete_indication_to_ull(pkb);
}
