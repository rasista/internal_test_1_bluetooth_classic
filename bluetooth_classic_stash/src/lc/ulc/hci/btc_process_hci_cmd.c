#include "btc_dev.h"
#include "btc_hci.h"
#include "hci.h"
#include "btc_hci_events.h"
#include "hci_cmd_get_defines_autogen.h"
#include "hci_handlers_autogen.h"
#include "hci_cmd_set_defines_status_event_autogen.h"
#include "hci_cmd_set_defines_complete_event_autogen.h"
#include "hci_cmd_get_defines_autogen.h"
#include "ulc.h"
#include "btc.h"
#include "hci_events_ptl_autogen.h"
#include "btc_lmp.h"
#include "btc_process_hci_cmd.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "usched_pi.h"
#include "btc_lmp_synchronous_conn.h"

void byte_reset_peer_info(acl_peer_info_t *acl_peer_info)
{
  memset(&(acl_peer_info->acl_dev_mgmt_info), 0, sizeof(acl_dev_mgmt_info_t));
  memset(&(acl_peer_info->acl_link_mgmt_info), 0, sizeof(acl_dev_mgmt_info_t));
  memset(&(acl_peer_info->acl_enc_info), 0, sizeof(acl_enc_info_t));
  memset(&(acl_peer_info->conn_pwr_ctrl_info), 0, sizeof(conn_pwr_ctrl_info_t));
  memset(&(acl_peer_info->esco_info), 0, sizeof(esco_info_t));
  memset(&(acl_peer_info->sco_link_params), 0, (sizeof(sco_link_params_t) * MAX_NUM_SCO_HANDLES));
  memset(&(acl_peer_info->esco_link_params), 0, (sizeof(esco_link_params_t) * MAX_NUM_ESCO_HANDLES));
  memset(&(acl_peer_info->sco_update_params), 0, (sizeof(sco_update_params_t) * MAX_NUM_SCO_HANDLES));

  memset(&(acl_peer_info->test_mode_params), 0, sizeof(test_mode_params_t));
  memset(&(acl_peer_info->sniff_params), 0, sizeof(sniff_params_t));

  // memset(&slave_info->tx_acld_q, sizeof(bt_tx_queue_t));
  // memset(&slave_info->tx_aclc_q, sizeof(bt_tx_queue_t));
  return;
}

void bt_reset_peer_info(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info;

  acl_peer_info                              = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info     = &acl_peer_info->acl_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info   = &acl_peer_info->acl_link_mgmt_info;
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info   = &acl_peer_info->conn_pwr_ctrl_info;
  memset(acl_peer_info->acl_dev_mgmt_info.device_name, 0, MAX_BT_DEVICE_NAME_LEN);
  byte_reset_peer_info(acl_peer_info);

  /* Parameters to be retained */
  /* Assigning Default Values */
  *(uint32_t *)&acl_dev_mgmt_info->afh_channel_map_new[0] = 0xFFFFFFFF;
  *(uint32_t *)&acl_dev_mgmt_info->afh_channel_map_new[4] = 0xFFFFFFFF;
  *(uint16_t *)&acl_dev_mgmt_info->afh_channel_map_new[8] = 0x7FFF;

  /* If Tx power is configured from user then it should apply for every slave once
   * it got disconnected until the driver removes */
  if (btc_dev_info->btc_dev_mgmt_info.bt_tx_power_from_host) {
    conn_pwr_ctrl_info->tx_power_index = btc_dev_info->btc_dev_mgmt_info.bt_tx_power_from_host;
  } else {
    /* SIG_REVIEW, Sateesh decreased this to 15 from 18 check with him */
    conn_pwr_ctrl_info->tx_power_index = DEFAULT_EDR_POWER_INDEX;
  }

  conn_pwr_ctrl_info->min_allowed_power_index = MIN_ALLOWED_POWER_INDEX;
  conn_pwr_ctrl_info->max_allowed_power_index = MAX_ALLOWED_POWER_INDEX;
  acl_dev_mgmt_info->lm_connection_status         = LM_LEVEL_CONN_NOT_DONE;
  acl_link_mgmt_info->link_supervision_timeout    = DEFAULT_LINK_SUPERVISION_TIMEOUT;
  acl_link_mgmt_info->link_supervision_enabled    = LINK_SUPERVISION_ENABLED;
  acl_link_mgmt_info->poll_interval               = DEFAULT_POLL_INTERVAL;
  acl_dev_mgmt_info->connection_status            = NOT_CONNECTED;
  acl_dev_mgmt_info->conn_id_occupied             = 0;
  btc_dev_info->btc_dev_mgmt_info.max_acl_pkt_len              = DEFAULT_ACL_PACKET_LENGTH;

  acl_dev_mgmt_info->afh_reporting_mode = AFH_REPORTING_DISABLED;
  acl_dev_mgmt_info->afh_min_interval   = 0xBB78;
  acl_dev_mgmt_info->afh_max_interval   = 0xBB80;

  return;
}

#ifdef SCHED_SIM
void generate_random_device_bd_address(uint8_t *buffer, uint32_t length) {
  // Seed the random number generator once
  srand((uint32_t)time(NULL));

  // Generate random bytes
  for (uint32_t i = 0; i < length; i++) {
  buffer[i] = (uint8_t)(rand() & 0xFF);
  }
}
#endif

void bt_lc_init(ulc_t *ulc)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info     = &btc_dev_info->btc_dev_mgmt_info;
  page_scan_params_t *page_scan_params       = &btc_dev_info->page_scan_params;
  inquiry_scan_params_t *inquiry_scan_params = &btc_dev_info->inquiry_scan_params;

#ifdef SCHED_SIM
  generate_random_device_bd_address(btc_dev_mgmt_info->bt_mac_addr, MAC_ADDR_LEN);
#endif

  btc_dev_mgmt_info->device_name[0]  = 'E';
  btc_dev_mgmt_info->device_name[1]  = 'V';
  btc_dev_mgmt_info->device_name[2]  = 'E';
  btc_dev_mgmt_info->device_name[3]  = 'R';
  btc_dev_mgmt_info->device_name[4]  = 'E';
  btc_dev_mgmt_info->device_name[5]  = 'S';
  btc_dev_mgmt_info->device_name[6]  = 'T';
  btc_dev_mgmt_info->device_name[7]  = 'D';
  btc_dev_mgmt_info->device_name[8]  = 'E';
  btc_dev_mgmt_info->device_name[9]  = 'V';
  btc_dev_mgmt_info->device_name[10] = 'I';
  btc_dev_mgmt_info->device_name[11] = 'C';
  btc_dev_mgmt_info->device_name[12] = 'E';
  btc_dev_mgmt_info->device_name[13] = 0x00;
  btc_dev_mgmt_info->device_name_len = 13;
  btc_dev_mgmt_info->class_of_dev[0] = 0x00;
  btc_dev_mgmt_info->class_of_dev[1] = 0x01;
  btc_dev_mgmt_info->class_of_dev[2] = 0x10;

  btc_dev_mgmt_info->max_sco_pkt_len              = MAX_SCO_PACKET_LENGTH;
  btc_dev_mgmt_info->max_slots                    = FIVE_SLOTS;
  btc_dev_mgmt_info->scan_enable                  = NO_SCAN_EN;
  btc_dev_mgmt_info->link_policy_settings         = 0;
  btc_dev_mgmt_info->fixed_pin                    = VARIABLE_PIN;
  btc_dev_mgmt_info->drift                        = DEFAULT_DRIFT;
  btc_dev_mgmt_info->jitter                       = DEFAULT_JITTER;
  btc_dev_mgmt_info->max_acl_pkt_len              = MAX_ACL_PACKET_LENGTH;
  // btc_dev_mgmt_info->periodic_inq_enable          = PERIODIC_INQUIRY_DISABLED;
  // btc_dev_mgmt_info->pending_states               = 0;
  btc_dev_mgmt_info->tx_non_connected_power_index = DEFAULT_EDR_POWER_INDEX;

  btc_dev_info->inquiry_params.inquiry_mode = INQUIRY_RESULT_FORMAT;

  btc_dev_info->page_params.page_scan_rep_mode = PAGE_SCAN_MODE_R1;
  btc_dev_info->page_params.page_tout          = DEFAULT_PAGE_TOUT;

  page_scan_params->page_scan_interval = DEFAULT_PSCAN_INTERVAL;
  page_scan_params->page_scan_window   = DEFAULT_PSCAN_WINDOW;
  page_scan_params->page_scan_rep_mode = PAGE_SCAN_MODE_R1;
  page_scan_params->page_scan_type     = STANDARD_PAGE_SCAN;

  inquiry_scan_params->inquiry_scan_interval = DEFAULT_ISCAN_INTERVAL;
  inquiry_scan_params->inquiry_scan_window   = DEFAULT_ISCAN_WINDOW;
  inquiry_scan_params->inquiry_scan_type     = STANDARD_INQUIRY_SCAN;

  return;
}

void btc_hci_init(void *ctx)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_hci_params_t *hci_params = &btc_dev_info->btc_hci_params;

  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  /*initialize the version info*/
  btc_dev_mgmt_info->hci_vers_num = BT_SPEC_5_4;
  btc_dev_mgmt_info->hci_revision = EVEREST_SUB_VERSION_NUMBER;
  btc_dev_mgmt_info->lmp_vers_num = BT_SPEC_5_4;
  btc_dev_mgmt_info->comp_id = IOT_COMP_ID;
  btc_dev_mgmt_info->lmp_sub_vers_num = LMP_SUB_VERSION_NUMBER_NUM;

  bt_lc_init(ulc);
  uint8_t peer_id;
  for (peer_id = 0; peer_id < MAX_NUM_ACL_CONNS; peer_id++) {
    bt_reset_peer_info(ulc, peer_id);
  }

  btc_dev_mgmt_info->dev_status_flags |= ROLE_SWITCH_EN;

  btc_dev_mgmt_info->dev_status_flags |= SIMPLE_PAIRING_EN;
  /* Initializing the feature bitmap  */
  btc_dev_mgmt_info->features_bitmap[0] = 0 | LMP_3SLOT | LMP_5SLOT | LMP_ENCRYPT | LMP_SOFFSET | LMP_TACCURACY | LMP_RSWITCH | LMP_SNIFF;
  btc_dev_mgmt_info->features_bitmap[1] = 0 | LMP_PCONTROL_REQ | LMP_CQDDR | LMP_SCO | LMP_HV2 | LMP_HV3 | LMP_ULAW | LMP_ALAW;
  btc_dev_mgmt_info->features_bitmap[2] = 0 | LMP_CVSD | LMP_PCONTROL | LMP_SYNCH_DATA;
  btc_dev_mgmt_info->features_bitmap[3] = LMP_EDR_ACL_2M | LMP_EDR_ACL_3M | LMP_ENHNCD_INQ_SCN | LMP_INTLCD_INQ_SCN | LMP_INTLCD_PG_SCN | LMP_RSSI_INQ | LMP_ESCO;
  btc_dev_mgmt_info->features_bitmap[4] = 0 | LMP_EV4 | LMP_EV5 | LMP_AFH_CAP_SLAVE | LMP_AFH_CLS_SLAVE | LMP_3S_EDR_ACL;
  btc_dev_mgmt_info->features_bitmap[5] = 0 | LMP_5S_EDR_ACL | LMP_SNIFF_SUBR  | LMP_PAUSE_ENC | LMP_AFH_CAP_MASTER  | LMP_AFH_CLS_MASTER | LMP_EDR_ESCO_2M | LMP_EDR_ESCO_3M | LMP_EDR_3S_ESCO;
  btc_dev_mgmt_info->features_bitmap[6] = LMP_EXT_INQ | LMP_NO_FLUSH | LMP_SIMPLE_PAIR | LMP_ENCAP_PDU;
  btc_dev_mgmt_info->features_bitmap[7] = LMP_LSTO_CHANGED | LMP_INQ_TX_PWR | LMP_ENHCED_PWR | LMP_EXTFEATURES;

  btc_dev_mgmt_info->ext_features_bitmap[0] = 0 | LMP_HOST_SSP;
  btc_dev_mgmt_info->ext_features_bitmap_page2[0] = 0 |  LMP_INQUIRY_NOTIFICATION_EVENT | LMP_GENERALIZED_INTERLACED_SCAN;

  /* Initializing the event map */
  *(uint32_t *)&hci_params->event_map_mask[0] = HCI_EVENT_MAP_MASK;
  *(uint32_t *)&hci_params->event_map_mask[4] = HCI_EVENT_MAP_MASK;

  /* Initalizing the Command bit map */
  memset(hci_params->hci_cmd_bitmap, 0, HCI_CMD_MAP_LENGTH);
  hci_params->hci_cmd_bitmap[0] = 0xBF;
  /* 0 Inquiry
   * 1 Inquiry Cancel
   * 2 Periodic Inquiry Mode
   * 3 Exit Periodic Inquiry Mode
   * 4 Create Connection
   * 5 Disconnect
   * 6 Add SCO Connection (deprecated)
   * 7 Cancel Create Connection
   * */

  hci_params->hci_cmd_bitmap[1] = 0xFF;
  /* 0 Accept Connection Request
   * 1 Reject Connection Request
   * 2 Link Key Request Reply
   * 3 Link Key Request Negative Reply
   * 4 PIN Code Request Reply
   * 5 PIN Code Request Negative Reply
   * 6 Change Connection Packet Type
   * 7 Authentication Request
   * */
  hci_params->hci_cmd_bitmap[2] = 0xFB;
  /* 0 Set Connection Encryption
   * 1 Change Connection Link Key
   * 2 Master Link Key
   * 3 Remote Name Request
   * 4 Cancel Remote Name Request
   * 5 Read Remote Supported Features
   * 6 Read Remote Extended Features
   * 7 Read Remote Version Information
   * */
  hci_params->hci_cmd_bitmap[3] = 0x03;
  /* 0 Read Clock Offset
   * 1 Read LMP Handle
   * 2-7 Reserved
   * */
  hci_params->hci_cmd_bitmap[4] = 0xCC;
  /* 0 Reserved
   * 1 Hold Mode
   * 2 Sniff Mode
   * 3 Exit Sniff Mode
   * 4 Park State
   * 5 Exit Park State
   * 6 Qos Setup
   * 7 Role Discovery
   * */
  hci_params->hci_cmd_bitmap[5] = 0xFF;
  /* 0 Switch Role
   * 1 Read Link Policy Settings
   * 2 Write Link Policy Settings
   * 3 Read Default Link Policy Settings
   * 4 Write Default Link Policy Settings
   * 5 Flow Specification
   * 6 Set Event Mask
   * 7 Reset
   * */
  hci_params->hci_cmd_bitmap[6] = 0xEF;
  /* 0 Set Event Filter
   * 1 Flush
   * 2 Read PIN Type
   * 3 Write PIN Type
   * 4 Create New Unit Key
   * 5 Read Stored Link Key
   * 6 Write Stored Link Key
   * 7 Delete Stored Link Key
   * */
  hci_params->hci_cmd_bitmap[7] = 0xFF;
  /* 0 Write Local Name
   * 1 Read Local Name
   * 2 Read Connection Accept Timeout
   * 3 Write Connection ACcept Timeout
   * 4 Read Page Timeout
   * 5 Write Page Timeout
   * 6 Read Scan Enable
   * 7 Write Scan Enable
   * */
  hci_params->hci_cmd_bitmap[8] = 0xFF;
  /* 0 Read Page Scan Activity
   * 1 Write Page Scan Activity
   * 2 Read Inquiry Scan Activity
   * 3 Write Inquiry Scan Activity
   * 4-5 Reserved
   * 6 Read Encryption Mode (deprecated)
   * 7 Write Encryption Mode (deprecated)
   * */
  hci_params->hci_cmd_bitmap[9] = 0x3F;

  /* 0 Read Class of Device
   * 1 Write Class of Device
   * 2 Read Voice Setting
   * 3 Write Voice Setting
   * 4 Read Automatic Flush Timeout
   * 5 Write Automatic Flush Timeout
   * 6 Read Num Broadcast Retransmissions
   * 7 Write Num Broadcast Retransmissions
   * */
  hci_params->hci_cmd_bitmap[10] = 0xC4;
  /* 0 Read Hold Mode Activity
   * 1 Write Hold Mode Activity
   * 2 Read Transmit Power Level
   * 3 Read Synchronous Flow Control Enable
   * 4 Write Synchronous Flow Control Enable
   * 5 Set Host Controller To Host Flow Control
   * 6 Host Buffer Size
   * 7 Host Number of Completed Packets
   * */
  hci_params->hci_cmd_bitmap[11] = 0x1F;
  /* 0 Read Link Supervision Timeout
   * 1 Write Link Supervision Timeout
   * 2 Read Number of Supported IAC
   * 3 Read Current IAC LAP
   * 4 Write Current IAC LAP
   * 5 Read Page Scan Mode Period (deprecated)
   * 6 Write Page Scan Mode Period (deprecated)
   * 7 Read Page Scan Mode (deprecated)
   * */
  hci_params->hci_cmd_bitmap[12] = 0xF2;
  /* 0 Write Page Scan Mode (deprecated)
   * 1 Set AFH Channel Classification
   * 2-3 Reserved
   * 4 Read Inquiry Scan Type
   * 5 Write Inquiry Scan Type
   * 6 Read Inquiry Mode
   * 7 Write Inquiry Mode
   * */
  hci_params->hci_cmd_bitmap[13] = 0x0F;
  /* 0 Read Page Scan Type
   * 1 Write Page Scan Type
   * 2 Read AFH Channel Assessment Mode
   * 3 Write AFH Channel Assessment Mode
   * 4-7 Reserved
   * */
  hci_params->hci_cmd_bitmap[14] = 0xE8;
  /* 0-2 Reserved
   * 3 Read Local Version Information
   * 4 Reserved
   * 5 Read Local Supported Features
   * 6 Read Local Extended Features
   * 7 Read Buffer Size
   * */
  hci_params->hci_cmd_bitmap[15] = 0xFE;
  /* 0 Read Country Code (Deprecated)
   * 1 Read BD ADDR
   * 2 Read Failed Contact Count
   * 3 Reset Failed Contact Count
   * 4 Get Link Quality
   * 5 Read RSSI
   * 6 Read AFH Channel Map
   * 7 Read BD Clock
   * */
  hci_params->hci_cmd_bitmap[16] = 0x3c;
  /* 0 Read Loopback Mode
   * 1 Write Loopback Mode
   * 2 Enable Device Under Test Mode
   * 3 Setup Synchronous Connection
   * 4 Accept Synchronous Connection
   * 5 Reject Synchronous Connection
   * 6-7 Reserved
   * */
  hci_params->hci_cmd_bitmap[17] = 0xF7;
  /* 0 Read Extended Inquiry Response
   * 1 Write Extended Inquiry Response
   * 2 Refresh Encryption Key
   * 3 Reserved
   * 4 Sniff Subrating
   * 5 Read Simple Pairing Mode
   * 6 Write Simple Pairing Mode
   * 7 Read Local OOB Data
   * */
  hci_params->hci_cmd_bitmap[18] = 0x83;
  /* 0 Read Inquiry Response Transmit Power
   * 1 Write Inquiry Transmit Power Level
   * 2 Read Default Erroneous Data Reporting
   * 3 Write Default Erroneous Data Reporting
   * 4-6 Reserved
   * 7 IO Capability Request Reply
   * */
  hci_params->hci_cmd_bitmap[19] = 0xFF;
  /* 0 User Confirmation Request Reply
   * 1 User Confirmation Request Negative Reply
   * 2 User Passkey Request Reply
   * 3 User Passkey Request Negative Reply
   * 4 Remote OOB Data Request Reply
   * 5 Write Simple Pairing Debug Mode
   * 6 Enhanced Flush
   * 7 Remote OOB Data Request Negative Reply
   * */
  hci_params->hci_cmd_bitmap[20] = 0x1C;
  /**
   * Enable this For CIN/BV-03-C
   * hci_params->hci_cmd_bitmap[20] = 0x14;
   */
  /* 0-1 Reserved
   * 2 Send Keypress Notification
   * 3 IO Capability Request Negative Reply
   * 4 Read Encryption Key Size
   * 5-7 Reserved
   * */
  hci_params->hci_cmd_bitmap[21] = 0x00;
  /* 0 Create Physical Link
   * 1 Accept Physical Link
   * 2 Disconnect Physical Link
   * 3 Create Logical Link
   * 4 Accept Logical Link
   * 5 Disconnect Logical Link
   * 6 Logical Link Cancel
   * 7 Flow Spec Modify
   * */
  hci_params->hci_cmd_bitmap[22] = 0x04;
  /* 0 Read Logical Link Accept Timeout
   * 1 Write Logical Link Accept Timeout
   * 2 Set Event Mask Page 2
   * 3 Read Location Data
   * 4 Write Location Data
   * 5 Read Local AMP Info
   * 6 Read Local AMP_ASSOC
   * 7 Write Remote AMP_ASSOC
   * */
  hci_params->hci_cmd_bitmap[23] = 0x07;

  /**
   * Enable this For CIN/BV-03-C
   * hci_params->hci_cmd_bitmap[23] = 0x00;
   */
  /* 0 Read Flow Control Mode  Disabled in PICS, Need to check why HCI case is getting failed
   * 1 Write Flow Control Mode Disabled in PICS, Need to check why HCI case is getting failed
   * 2 Read Data Block Size    Disabled in PICS, Need to check why HCI case is getting failed
   * 3 Reserved
   * 4 Reserved
   * 5 Enable AMP Receiver Reports
   * 6 AMP Test End
   * 7 AMP Test Command
   * */
  hci_params->hci_cmd_bitmap[24] = 0x61;
  /**
   * Enable this For CIN/BV-03-C
   * hci_params->hci_cmd_bitmap[24] = 0x60;
   */
  /* 0 Read Enhanced Transmit Power Level
   * 1 Reserved
   * 2 Read Best Effort Flush Timeout
   * 3 Write Best Effort Flush Timeout
   * 4 Short Range Mode
   * 5 Read LE Host Support
   * 6 Write LE Host Support
   * 7 Reserved
   * */
}

uint16_t lc_cal_acl_max_payload_len(uint8_t data_rate_type, uint16_t pkt_type, uint8_t max_slot)
{
  /*TODO: max_tx_len has to be moved from cslp to slave_info*/
  uint16_t max_tx_len = 0;
  if (data_rate_type == ENHANCED_DATA_RATE) {
    switch (max_slot) {
      case FIVE_SLOT_PACKET:
      {
        if (!(pkt_type & PTYPE_3DH5_MAY_NOT_BE_USED)) {
          max_tx_len = BT_3DH5_PAYLOAD_MAX_LEN;
          break;
        } else if (!(pkt_type & PTYPE_2DH5_MAY_NOT_BE_USED)) {
          max_tx_len = BT_2DH5_PAYLOAD_MAX_LEN;
          break;
        }
        break;
      }
      case THREE_SLOT_PACKET:
      {
        if (!(pkt_type & PTYPE_3DH3_MAY_NOT_BE_USED)) {
          max_tx_len = BT_3DH3_PAYLOAD_MAX_LEN;
          break;
        } else if (!(pkt_type & PTYPE_2DH3_MAY_NOT_BE_USED)) {
          max_tx_len = BT_2DH3_PAYLOAD_MAX_LEN;
          break;
        }
        break;
      }
      case ONE_SLOT_PACKET:
      {
        if (!(pkt_type & PTYPE_3DH1_MAY_NOT_BE_USED)) {
          max_tx_len = BT_3DH1_PAYLOAD_MAX_LEN;
          break;
        } else if (!(pkt_type & PTYPE_2DH1_MAY_NOT_BE_USED)) {
          max_tx_len = BT_2DH1_PAYLOAD_MAX_LEN;
          break;
        }
        break;
      }
      default:
      {
        if (pkt_type & ((BIT(BT_DM1_PKT_TYPE)))) {
          max_tx_len = BT_DM1_PAYLOAD_MAX_LEN;
          break;
        }
        break;
      }
    }
  } else {
    switch (max_slot) {
      case FIVE_SLOT_PACKET:
      {
        if (pkt_type & PTYPE_DH5_MAY_BE_USED) {
          max_tx_len = BT_DH5_PAYLOAD_MAX_LEN;
          break;
        } else if (pkt_type & PTYPE_DM5_MAY_BE_USED) {
          max_tx_len = BT_DM5_PAYLOAD_MAX_LEN;
          break;
        }
        break;
      }
      case THREE_SLOT_PACKET:
      {
        if (pkt_type & PTYPE_DH3_MAY_BE_USED) {
          max_tx_len = BT_DH3_PAYLOAD_MAX_LEN;
          break;
        } else if (pkt_type & PTYPE_DM3_MAY_BE_USED) {
          max_tx_len = BT_DM3_PAYLOAD_MAX_LEN;
          break;
        }
        break;
      }
      case ONE_SLOT_PACKET:
      {
        if (pkt_type & PTYPE_DH1_MAY_BE_USED) {
          max_tx_len = BT_DH1_PAYLOAD_MAX_LEN;
          break;
        } else if (pkt_type & PTYPE_DM1_MAY_BE_USED) {
          max_tx_len = BT_DM1_PAYLOAD_MAX_LEN;
          break;
        }
        break;
      }
    }
  }

  if (max_tx_len == 0) {
    BTC_ASSERT(FRAG_SELECTION_FAILED); // Its gud to have this
  }
  return max_tx_len;
}

uint16_t bt_peer_id_to_conn_handle(ulc_t *ulc, uint8_t slave_id)
{
  (void)slave_id;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(btc_dev_info->btc_dev_mgmt_info);

  uint16_t count = BIT(0);
  // uint32_t ipl_reg;

  while (btc_dev_mgmt_info->conn_handle_bitmap & count) {
    count <<= 1;
    if (count & BIT(3)) {
      BTC_ASSERT(CHECK_BITMAP);
    }
  }

  // ipl_reg = conditional_intr_disable();
  btc_dev_mgmt_info->conn_handle_bitmap |= count;
  // conditional_intr_enable(ipl_reg);
  return count;
}

uint8_t dm_read_rssi_for_conn(ulc_t *ulc, uint16_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;
  uint8_t rssi_val;

  if (conn_pwr_ctrl_info->rssi_average < GOLDEN_RECEIVE_POWER_RANGE_LOWER_LIMIT) {
    rssi_val = conn_pwr_ctrl_info->rssi_average - GOLDEN_RECEIVE_POWER_RANGE_LOWER_LIMIT;
  } else if (conn_pwr_ctrl_info->rssi_average > GOLDEN_RECEIVE_POWER_RANGE_UPPER_LIMIT) {
    rssi_val = conn_pwr_ctrl_info->rssi_average - GOLDEN_RECEIVE_POWER_RANGE_UPPER_LIMIT;
  } else {
    rssi_val = 0;
  }

  return rssi_val;
}

void bt_delete_link_key(ulc_t *ulc, uint8_t *bd_addr)
{
  // uint32_t ipl_reg;
  link_key_info_t *link_key_info;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t btc_dev_mgmt_info = bt_dev_info->btc_dev_mgmt_info;

  link_key_info = bt_find_link_key(ulc, bd_addr);
  if (link_key_info) {
    memcpy(link_key_info,
           &btc_dev_mgmt_info.link_key_info[btc_dev_mgmt_info.no_of_link_key_stored],
           sizeof(link_key_info_t));
    btc_dev_mgmt_info.no_of_link_key_stored--;
  }
}

void bt_store_link_key(ulc_t *ulc, uint8_t *bd_addr, uint8_t *link_key)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  link_key_info_t *link_key_info = NULL;
  link_key_info = bt_find_link_key(ulc, bd_addr);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(bt_dev_info->btc_dev_mgmt_info);

  if (link_key_info) {
    memcpy(link_key_info->link_key, link_key, LINK_KEY_LEN);
  } else {
    if (btc_dev_mgmt_info->no_of_link_key_stored < MAX_NUM_OF_STORED_LINK_KEY) {
      memcpy(btc_dev_mgmt_info->link_key_info[btc_dev_mgmt_info->no_of_link_key_stored].bd_addr, bd_addr, MAC_ADDR_LEN);
      memcpy(btc_dev_mgmt_info->link_key_info[btc_dev_mgmt_info->no_of_link_key_stored].link_key, link_key, LINK_KEY_LEN);
      btc_dev_mgmt_info->no_of_link_key_stored++;
    }
  }
}

link_key_info_t *bt_find_link_key(ulc_t *ulc, uint8_t *bd_addr)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  link_key_info_t *link_key_info = &bt_dev_info->btc_dev_mgmt_info.link_key_info[0];
  uint8_t ii = 0;
  uint8_t no_of_link_key_stored = bt_dev_info->btc_dev_mgmt_info.no_of_link_key_stored;

  for (ii = 0; ii < no_of_link_key_stored; ii++) {
    if (!memcmp(bd_addr, link_key_info[ii].bd_addr, BD_ADDR_LEN)) {
      return &link_key_info[ii];
    }
  }
  return NULL;
}

void bt_clear_security_flags(ulc_t *ulc, uint8_t *bd_addr, uint8_t delete_all_flag)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_dev_mgmt_info_t *acl_dev_mgmt_info   = NULL;
  acl_link_mgmt_info_t *acl_link_mgmt_info = NULL;
  uint8_t peer_id                    = 0;
  acl_peer_info_t *acl_peer_info           = NULL;

  for (peer_id = 0; peer_id < MAX_NUM_ACL_CONNS; peer_id++) {
    acl_peer_info      = bt_dev_info->acl_peer_info[peer_id];
    acl_dev_mgmt_info  = &acl_peer_info->acl_dev_mgmt_info;
    acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
    if (acl_dev_mgmt_info->conn_id_occupied
      && ((!memcmp(bd_addr, acl_dev_mgmt_info->bd_addr, BD_ADDR_LEN)) || delete_all_flag)) {
      acl_link_mgmt_info->device_status_flags &= ~(DEVICE_LINK_KEY_EXISTS | PUBLIC_KEY_SENT | PUBLIC_KEY_RCVD);
      break;
    }
  }
}

void generate_rand_num(uint8_t rand_num_bytes, uint8_t *rand_num, uint8_t random_num_type)
{
  (void)random_num_type;
  (void)rand_num_bytes;
  (void)rand_num;
#ifdef SCHED_SIM
  if (rand_num != NULL && rand_num_bytes > 0) {
    static uint8_t seed_initialized = 0;
    if (!seed_initialized) {
      srand((uint32_t)time(NULL));
    }
    seed_initialized = 1;
      // Generate pseudo-random numbers using standard rand()
    for (uint8_t i = 0; i < rand_num_bytes; i++) {
        rand_num[i] = (uint8_t)(rand() & 0xFF);
    }
  }
#endif
}

void allocate_resp_addr_list(btc_dev_info_t *btc_dev_info_p)
{
  if (btc_dev_info_p->inquiry_params.resp_addr_list == NULL) {
    btc_dev_info_p->inquiry_params.resp_addr_list = (uint8_t *)pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
    if (!(btc_dev_info_p->inquiry_params.resp_addr_list)) {
      BTC_ASSERT(NULL_POINTER);
      return;
    }
  }
}

void free_resp_addr_list(btc_dev_info_t *btc_dev_info_p)
{
  if (btc_dev_info_p->inquiry_params.resp_addr_list) {
    pkb_free((pkb_t *)btc_dev_info_p->inquiry_params.resp_addr_list, &btc.pkb_pool[pkb_chunk_type_medium]);
    btc_dev_info_p->inquiry_params.resp_addr_list = NULL;
  }
}

uint8_t bt_remote_name_req_cancel(ulc_t *ulc, uint8_t *bd_addr)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_link_mgmt_info_t *acl_link_mgmt_info = NULL;
  uint8_t peer_id = 0;

  /* Getting Slave id */
  for (peer_id = 0; peer_id < MAX_NUM_ACL_CONNS; peer_id++) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

    if (acl_peer_info->acl_dev_mgmt_info.conn_id_occupied && !memcmp(acl_peer_info->acl_dev_mgmt_info.bd_addr, bd_addr, BD_ADDR_LEN)) {
      break;
    }
  }

  /* Returning error code in case of unexpected reception of command */
  if (peer_id == MAX_NUM_ACL_CONNS) {
    return INVALID_HCI_COMMAND_PARAMETERS;
  }

  acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  if ((!(acl_link_mgmt_info->check_pending_req_from_hci & HCI_NAME_REQ_SENT)) && (acl_link_mgmt_info->conn_purpose != TEMPORARY_CONN_FOR_REMOTE_NAME_RES)) {
    return INVALID_HCI_COMMAND_PARAMETERS;
  }

  /* Calling a function to cancel ongoing connection process */
  if (acl_link_mgmt_info->conn_purpose == TEMPORARY_CONN_FOR_REMOTE_NAME_RES) {
    acl_link_mgmt_info->check_pending_req_from_hci |= REMOTE_NAME_REQ_CAN_EVENT_PENDING;
    usched_pi_stop_handler(NULL, &ulc->scheduler_info.page_role_cb.task);
  }

  acl_link_mgmt_info->check_pending_req_from_hci &= ~(HCI_NAME_REQ_SENT);
  return BT_SUCCESS_STATUS;
}

uint8_t dm_remote_dev_name_req_from_hci(ulc_t *ulc,
                                        uint8_t *bd_addr,
                                        uint8_t page_scan_rep_mode,
                                        uint16_t clk_offset)
{
  conn_params_t conn_params;
  uint8_t peer_id;
  uint8_t status;
  acl_peer_info_t *acl_peer_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);

  memcpy(conn_params.bd_addr, bd_addr, MAC_ADDR_LEN);
  conn_params.page_scan_rep_mode = page_scan_rep_mode;
  conn_params.clk_offset = clk_offset;
  conn_params.allow_role_switch = NO_ROLE_SWITCH;
  conn_params.pkt_type = ALL_ACL_DATA_PKTS_VALID;
  /* If a connection doesn't exist create a temporary connection */
  status = bt_check_conn(ulc, &conn_params, (uint8_t *)&peer_id);

  acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);

  if (status == ACL_CONNECTION_ALREADY_EXISTS) {
    {
      acl_link_mgmt_info->check_pending_req_from_hci |= HCI_NAME_REQ_SENT;
      lmp_remote_dev_name_req_from_hci(ulc, peer_id);
    }
    status = BT_SUCCESS_STATUS;
  } else if (status == BT_SUCCESS_STATUS) {
    memcpy(acl_dev_mgmt_info->bd_addr, bd_addr, MAC_ADDR_LEN);
    acl_link_mgmt_info->check_pending_req_from_hci |= HCI_NAME_REQ_SENT;
    acl_link_mgmt_info->conn_purpose = TEMPORARY_CONN_FOR_REMOTE_NAME_RES;
    usched_pi_parameters_init(&(ulc->scheduler_info), 0, PAGE_ROLE);
  }
  return status;
}

uint8_t bt_allow_conn_req_from_host(ulc_t *ulc)
{
  (void)ulc;
  uint8_t status = CONN_REJECTED_DUE_TO_LIMITED_RESOURCES;
  status = BT_STATUS_SUCCESS;
  // #ifdef BT_2_SLAVE_SUPPORT
  //   bt_ram_info_t *bt_ram_info = (bt_ram_info_t *)bt_info->dummy_bt_info_p;
  // #endif
  // btc_dev_info_t *bt_dev_info =  &(ulc->btc_dev_info);
  //   //! Allowing Connection requests only when scatternet is not established
  //   if (!(bt_dev_info->bt_context_info_p->bt_roles & (BT_ROLE_PAGING | BT_ROLE_LC_START_PAGING)) &&
  //   /* SCATTER_FIXME: Check Why this One BoxEmbedded define is present */
  // #if 1 //ndef ONEBOXE
  //       ((bt_info->bt_context_info_p->no_of_slaves_connected == 0) ||
  //        /* slave role is already there or PSCAN is running then not allowed the connection */
  //        (!(bt_info->bt_context_info_p->bt_roles & (BT_SLAVE_ROLE | BT_ROLE_PSCAN)))
  // #ifdef BT_2_SLAVE_SUPPORT
  //        || (bt_ram_info->no_of_masters_connected < (BT_MAX_SUPPORTED_ACL_LINKS))
  // #endif
  //          )
  // #else
  //       (!(bt_info->bt_context_info_p->bt_roles & BT_SLAVE_ROLE))
  // #endif
  //   ) {
  status = BT_STATUS_SUCCESS;

  return status;
}

/**
 * @brief   This function changes the pkt type for the SCO connection
 * @param   bt info structure pointer
 * @param   slave_id
 * @param   sco_handle
 * @param   pkt_type
 * @return  status
 */
uint8_t bt_change_sco_conn(ulc_t *ulc, uint8_t peer_id, uint8_t sco_handle, uint16_t pkt_type)
{
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info_p->acl_peer_info[peer_id];
  sco_update_params_t *sco_update_params = NULL;
  // uint32_t ipl_reg;
  // ipl_reg = conditional_intr_disable();
  if (acl_peer_info->acl_dev_mgmt_info.connection_status == ALREADY_CONNECTED) {
    sco_update_params = &acl_peer_info->sco_update_params[sco_handle];
    if (pkt_type & ADD_SCO_HV1) {
      sco_update_params->sco_pkt_type = SCO_HV1;
    }
    if (pkt_type & ADD_SCO_HV2) {
      sco_update_params->sco_pkt_type = SCO_HV2;
    }
    if (pkt_type & ADD_SCO_HV3) {
      sco_update_params->sco_pkt_type = SCO_HV3;
    }
    /* Calling a function in LM to update the SCO connection */
    // lm_create_sco_conn(ulc, peer_id, UPDATE_SYNC_CONN); //TODO
  } else {
    /*not in connected state*/
    // BT_RAM_ASSERT(CHANGE_SCO_CONNECTION);
  }
  /// conditional_intr_enable(ipl_reg);
  return BT_STATUS_SUCCESS;
}

uint8_t bt_conn_handle_to_peer_id(ulc_t *ulc, uint16_t conn_handle)
{
  uint8_t peer_id = INVALID_PEER_ID;
  uint16_t esco_handle, sco_handle;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;

  for (peer_id = 0; peer_id < MAX_NUM_ACL_CONNS; peer_id++) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    if (acl_peer_info->acl_dev_mgmt_info.connection_status == ALREADY_CONNECTED && acl_peer_info->acl_dev_mgmt_info.conn_id_occupied) {
      if (acl_peer_info->acl_dev_mgmt_info.conn_handle == conn_handle) {
        return peer_id;
      } else if (conn_handle & SCO_LT) {
        sco_handle = SCO_CONN_HANDLE_TO_SCO_HANDLE(conn_handle);
        if (acl_peer_info->sco_link_params[sco_handle].sco_handle_occupied == SCO_HANDLE_OCCUPIED) {
          return peer_id;
        }
      } else if (conn_handle & ESCO_LT) {
        esco_handle = ESCO_CONN_HANDLE_TO_ESCO_HANDLE(conn_handle);
        if (acl_peer_info->esco_link_params[esco_handle].esco_handle_occupied == ESCO_HANDLE_OCCUPIED) {
          return peer_id;
        }
      }
    }
  }

  return INVALID_PEER_ID;
}

uint8_t bt_mac_addr_to_peer_id(ulc_t *ulc, uint8_t *bd_addr)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = NULL;
  uint8_t peer_id = 0;

  for (peer_id = 0; peer_id < MAX_NUM_ACL_CONNS; peer_id++) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);

    if (acl_peer_info->acl_dev_mgmt_info.conn_id_occupied && acl_peer_info->acl_dev_mgmt_info.connection_status == ALREADY_CONNECTED && !memcmp(bd_addr, acl_dev_mgmt_info->bd_addr, BD_ADDR_LEN)) {
      return peer_id;
    }
  }
  return INVALID_PEER_ID;
}

uint8_t bt_disconnect(ulc_t *ulc, uint8_t peer_id, uint8_t reason)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_peer_info = bt_dev_info->acl_peer_info[peer_id];

  if (acl_peer_info->acl_dev_mgmt_info.connection_status == ALREADY_CONNECTED) {
    lmp_send_detach_frame(ulc, peer_id, reason);
  } else {
    return INVALID_HCI_COMMAND_PARAMETERS;
  }
  return BT_STATUS_SUCCESS;
}

void dm_acl_disconn_ind(ulc_t *ulc, uint8_t peer_id, uint8_t reason_code)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  esco_link_params_t *esco_link_params = NULL;
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(bt_dev_info->btc_dev_mgmt_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  uint16_t conn_handle;
  conn_handle = acl_dev_mgmt_info->conn_handle;
  esco_link_params = &acl_peer_info->esco_link_params[acl_peer_info->esco_info.esco_handle];
  if (acl_link_mgmt_info->host_event_notification_flags & DISCONNNECTION_COMPLETE) {
    acl_link_mgmt_info->host_event_notification_flags &= ~DISCONNNECTION_COMPLETE;
    if (esco_link_params->esco_handle_occupied == ESCO_HANDLE_OCCUPIED) {
      hci_disconn_complete_indication(ulc, esco_link_params->esco_conn_handle, BT_STATUS_SUCCESS, reason_code);
    }
    hci_disconn_complete_indication(ulc, conn_handle, BT_STATUS_SUCCESS, reason_code);

  } else if (acl_link_mgmt_info->host_event_notification_flags & CONNECTION_COMPLETE) {
    /* If LMP_Disconnect_req comes before connection complete event then conn_complete_event with reason code should be sent*/
    acl_link_mgmt_info->host_event_notification_flags &= ~CONNECTION_COMPLETE;
    hci_conn_complete_indication(ulc, peer_id, reason_code, BT_ACL);
  } else if (acl_link_mgmt_info->host_event_notification_flags & DISCONNECT_COMPLETE_EVENT_GIVEN_TO_HOST) {
    return;
  }
  btc_dev_mgmt_info->testmode_block_host_pkts = 0;
}

void dm_reject_connection_from_hci(ulc_t *ulc, uint8_t *remote_dev_bd_addr, uint8_t role)
{
  (void)role;
  (void)remote_dev_bd_addr;
  (void)ulc;
  // TO DO
}

void dm_accept_connection_from_hci(ulc_t *ulc, uint8_t *remote_dev_bd_addr, uint8_t role)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_link_mgmt_info_t *acl_link_mgmt_info = NULL;
  uint8_t peer_id = 0;

  for (peer_id = 0; peer_id < MAX_NUM_ACL_CONNS; peer_id++) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

    if (acl_peer_info->acl_dev_mgmt_info.conn_id_occupied && !memcmp(acl_peer_info->acl_dev_mgmt_info.bd_addr, remote_dev_bd_addr, BD_ADDR_LEN)) {
      break;
    }
  }

  if (peer_id >= MAX_NUM_ACL_CONNS) {
    return;
  }

  if (acl_link_mgmt_info->received_req_flag & ACL_CONNECTION_REQ) {
    acl_peer_info->acl_dev_mgmt_info.link_policy_settings = bt_dev_info->btc_dev_mgmt_info.link_policy_settings;
    lmp_accept_connection_req_from_dm(ulc, peer_id, role);
  } else if (acl_link_mgmt_info->received_req_flag & SCO_CONNECTION_REQ) {
    /* Setting pending request flag */
    acl_link_mgmt_info->check_pending_req_from_hci |= NORMAL_CONN_REQ;
    lmp_accept_sync_conn_req_from_dm(ulc, peer_id, 0x00); // BT_COMMON_ACCEPT_CMD         0x0 TODO
  }
  return;
}

uint8_t bt_create_connection_cancel(ulc_t *ulc, uint8_t *bd_addr)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = NULL;
  acl_link_mgmt_info_t *acl_link_mgmt_info = NULL;
  uint8_t peer_id = 0;
  // uint32_t ipl_reg;

  // ipl_reg = conditional_intr_disable();
  for (peer_id = 0; peer_id < MAX_NUM_ACL_CONNS; peer_id++) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
    acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

    if (acl_peer_info->acl_dev_mgmt_info.conn_id_occupied && !memcmp(acl_peer_info->acl_dev_mgmt_info.bd_addr, bd_addr, BD_ADDR_LEN)) {
      break;
    }
  }
  // conditional_intr_enable(ipl_reg);

  if (peer_id >= MAX_NUM_ACL_CONNS) {
    return UNKNOWN_CONNECTION_IDENTIFIER;
  }

  if (acl_dev_mgmt_info->lm_connection_status == LM_LEVEL_CONN_DONE) {
    return ACL_CONNECTION_ALREADY_EXISTS;
  } else {
    acl_link_mgmt_info->check_pending_req_from_hci |= CREATE_CON_CAN_EVENT_PEDNING;
    usched_pi_stop_handler(NULL, &ulc->scheduler_info.page_role_cb.task);
    acl_dev_mgmt_info->connection_status = NOT_CONNECTED;
  }

  return BT_SUCCESS_STATUS;
}

uint8_t bt_check_conn(ulc_t *ulc, conn_params_t *conn_params, uint8_t *peer_id)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t i;
  uint8_t curr_peer_id = MAX_NUM_ACL_CONNS;
  acl_peer_info_t *acl_peer_info;

  for (i = 0; i < MAX_NUM_ACL_CONNS; i++) {
    acl_peer_info = bt_dev_info->acl_peer_info[i];
    if (acl_peer_info->acl_dev_mgmt_info.conn_id_occupied && !memcmp(acl_peer_info->acl_dev_mgmt_info.bd_addr, conn_params->bd_addr, BD_ADDR_LEN)) {
      if (bt_dev_info->acl_peer_info[i]->acl_dev_mgmt_info.connection_status == ALREADY_CONNECTED) {
        *peer_id = i;
        return ACL_CONNECTION_ALREADY_EXISTS;
      } else if (i == 0) {
        continue; // Reserved for remote master when local device is slave
      } else {
        curr_peer_id = i;
      }
    }

    /* Allocate one empty peer_id */
    if ((i != 0) && (curr_peer_id == MAX_NUM_ACL_CONNS) && !acl_peer_info->acl_dev_mgmt_info.conn_id_occupied) {
      curr_peer_id = i;
    }
  }

  if (curr_peer_id == MAX_NUM_ACL_CONNS) {
    return CONN_REJECTED_DUE_TO_LIMITED_RESOURCES;
  }

  /* Resetting peer_info parameters */
  bt_reset_peer_info(ulc, curr_peer_id);
  acl_peer_info = bt_dev_info->acl_peer_info[curr_peer_id];

  acl_peer_info->acl_dev_mgmt_info.conn_id_occupied = 1;
  memcpy(acl_peer_info->acl_dev_mgmt_info.bd_addr, conn_params->bd_addr, MAC_ADDR_LEN);
  acl_peer_info->acl_link_mgmt_info.acl_pkt_type = conn_params->pkt_type;
  acl_peer_info->acl_dev_mgmt_info.page_scan_rep_mode = conn_params->page_scan_rep_mode;
  acl_peer_info->acl_dev_mgmt_info.clk_offset = (conn_params->clk_offset & BT_CLOCK_OFFSET_VALID) ? ((conn_params->clk_offset) & ~(BT_CLOCK_OFFSET_VALID)) : 0;
  acl_peer_info->acl_link_mgmt_info.allow_role_switch = conn_params->allow_role_switch;
  acl_peer_info->acl_dev_mgmt_info.link_policy_settings = bt_dev_info->btc_dev_mgmt_info.link_policy_settings;
  *peer_id = curr_peer_id;
  return BT_STATUS_SUCCESS;
}

uint8_t bt_create_conn(ulc_t *ulc, conn_params_t *conn_params, pkb_t *pkb)
{
  (void)pkb;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t peer_id;
  uint8_t status;
  status = bt_check_conn(ulc, conn_params, (uint8_t *)&peer_id);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  if (status == ACL_CONNECTION_ALREADY_EXISTS) {
    if (acl_dev_mgmt_info->lm_connection_status == LM_LEVEL_CONN_DONE) {
      hci_conn_complete_indication(ulc, peer_id, ACL_CONNECTION_ALREADY_EXISTS, BT_ACL); 
    } else {
      acl_link_mgmt_info->host_event_notification_flags |= CONNECTION_COMPLETE;
      if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
        lmp_start_connection(ulc, peer_id);
      } else {
        // TODO
      }
    }
    return ACL_CONNECTION_ALREADY_EXISTS;
  } else if (status == CONN_REJECTED_DUE_TO_LIMITED_RESOURCES) {
    return CONN_REJECTED_DUE_TO_LIMITED_RESOURCES;
  } else {
    acl_link_mgmt_info->host_event_notification_flags |= CONNECTION_COMPLETE;
    acl_link_mgmt_info->conn_purpose = CONN_FOR_DATA_PKTS_EXCHANGE;
    usched_pi_parameters_init(&(ulc->scheduler_info), 0, PAGE_ROLE);
  }
  return BT_STATUS_SUCCESS;
}
