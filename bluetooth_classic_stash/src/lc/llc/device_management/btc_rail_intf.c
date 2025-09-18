/**
 * @file btc_rail_weak_intf.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-19
 *
 * @copyright Copyright (c) 2023-2025
 *
 */

#include "btc_rail.h"
#include "btc_device.h"
#include "btc_rail_intf.h"
#include "inquiry.h"
#include "message_decode.h"
#include "sl_btc_seq_interface.h"
#include "debug_logs.h"
#include "rail_seq.h"
#include "sl_debug_btc_llc_autogen.h"

/* Global variable for buffer address communication between ULC and LLC */
volatile uint32_t g_tx_acl_buffer_address;

#ifdef RUN_TEST_MODE
  #include "stats.h"
  #define INC(val) val++
stats_t stats;
#else
  #define INC(val)
#endif

btc_dev_t btc_dev;
volatile  btc_rail_seq_intfc_t configc;
RAIL_UserSeqConfigCb_t cbc;
shared_mem_t *ulc_llc_share_mem = NULL;
RAIL_UserCpReq_t cp_req[BTC_MAX_CP_REQUESTS];
uint8_t dma_triggered = 0;
uint8_t *received_rx_pkt = NULL;
uint16_t received_rx_pkt_len = 0;

// Update schedule tx timing information
RAIL_ScheduleTxConfigPtick_t scheduletxconfig_pticks = { 0, RAIL_TIME_ABSOLUTE, RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX };

// Update schedule rx timing information
RAIL_ScheduleRxConfigPtick_t schedulerxconfig_pticks = { 0, RAIL_TIME_ABSOLUTE, 0, RAIL_TIME_ABSOLUTE, HARD_RX_END_DISABLED, RX_TRANSITIONS_END_SCHEDULE_DISABLED };

const uint32_t rx_search_timeout_cb_lookup[4][2] = { { RX_DONE, PTI_RX_ID_PKT }, { RX_ABORT, PTI_RX_ABORT_HEC_ERROR }, { RX_TIMEOUT, PTI_RX_TIMEOUT }, { RX_TIMEOUT, PTI_RX_WINDOW_END } };

void init_btc_device_handler()
{
  // This initializes the entire structure to 0, including all elements of the bdAddr array
  RAIL_SEQ_BtcInitConfig_t btcInitConfig = { 0 };
  RAIL_SEQ_BTC_Init(&btcInitConfig);
}

pkb_t *get_buffer_for_notification(uint8_t **buffer)
{
  pkb_t *pkb = NULL;
  queue_t *rx_precommit_buffers_queue = &ulc_llc_share_mem->rx_precommit_buffers_queue;
  btc_rx_meta_data_t rx_meta_data;

  if ((rx_precommit_buffers_queue != NULL) && ((pkb = (pkb_t *)dequeue(rx_precommit_buffers_queue)) != NULL)) {
    *buffer = ((uint8_t *)((uint8_t *)pkb + ulc_llc_share_mem->metadata_size + BTC_LPW_RX_METADATA_SIZE));
  } else {
    BTC_ASSERT(NO_FREE_BUFFERS_QUEUE_AVAILABLE);
  }
  rx_meta_data.rx_pkt_meta_data.event_type = BTC_LLC_TO_ULC_INTERNAL_EVENT;
  *((uint32_t *)((uint8_t *)pkb + ulc_llc_share_mem->metadata_size)) = rx_meta_data.data;
  pkb->pkt_len = (sizeof(hss_event_t) + BTC_LPW_RX_METADATA_SIZE);

  return pkb;
}

void BTC_post_notification_to_ulc(pkb_t *pkb)
{
  queue_t *rx_event_buffers_queue = &ulc_llc_share_mem->rx_event_buffers_queue;
  if (rx_event_buffers_queue == NULL) {
    BTC_ASSERT(NO_FREE_BUFFERS_QUEUE_AVAILABLE);
    return;
  }
  enqueue(rx_event_buffers_queue, &pkb->queue_node);
  RAIL_SEQ_SendMbox(BTC_LLC_TO_ULC_INTERNAL_EVENT);
}

void BTC_RAIL_SEQ_txCb(RAIL_SeqTxPacketInfo_t *pTxPktInfo)
{
  uint8_t tx_status;

  if (pTxPktInfo->pktStatus == RAIL_SEQ_TX_PACKET_STATUS_SENT) {
    tx_status = TX_DONE;
    debug_log_event(DEBUG_PTI, PTI_TX_DONE);
    sl_btdm_fast_log(btc_log_rail_tx_done_cb, COMPONENT_SEQ_BTC, TRACE, "RAIL TX Done CB Received");
    INC(stats.tx_sent);
  } else {
    tx_status = TX_ABORT;
    debug_log_event(DEBUG_PTI, PTI_TX_ABORT);
    sl_btdm_fast_log(btc_log_rail_tx_abort_cb, COMPONENT_SEQ_BTC, TRACE, "RAIL TX Abort CB Received");
    INC(stats.tx_aborted);
  }

  // TODO: Remove this if RAIL provides a solution for callback being called after stop
  if (btc_dev.l2_scheduled_sm == NULL) {
    sl_btdm_fast_log(btc_log_l2_scheduled_sm_assert, COMPONENT_SEQ_BTC, TRACE, "ASSERT: TX CB: l2_scheduled_sm is NULL");
    return;
  }

  SM_STEP(RAIL_SM, btc_dev.l2_scheduled_sm, tx_status);
}

static void btc_rail_config_dma_descriptors(uint8_t *src, uint8_t *dst, uint32_t len, uint32_t *scatters)
{
  cp_req[*scatters].pSrc = src;
  cp_req[*scatters].pDst = dst;
  cp_req[*scatters].xferSzBytes = len;
  cp_req[*scatters].pNext = &cp_req[(*scatters) + 1];
  cp_req[*scatters].pCpReqCB = NULL;
  (*scatters)++;
}

static uint32_t btc_rail_prepare_dma_descriptors(uint8_t *src, uint32_t PortionBytes, uint32_t *scatters, uint32_t metadata_size)
{
  uint32_t src_len = 0;
  uint32_t pkb_max_size = pkb_chunk_size_medium - ulc_llc_share_mem->metadata_size;
  uint32_t len;
  pkb_t *dst;
  while (PortionBytes) {
    len = PortionBytes > pkb_max_size - metadata_size ? pkb_max_size - metadata_size : PortionBytes;
    if ((dst = (pkb_t *)dequeue(&ulc_llc_share_mem->rx_precommit_buffers_queue)) == NULL) {
      BTC_ASSERT(NO_BUFFERS_AVAILBLE);
      return NO_BUFFERS_AVAILBLE;
    }
    btc_rail_config_dma_descriptors((src + src_len), (uint8_t *)((uint8_t *)dst + ulc_llc_share_mem->metadata_size + metadata_size), len, scatters);
    PortionBytes -= len;;
    dst->pkt_len = len + metadata_size;
    src_len += len;
    metadata_size = 0;
  }
  return 0;
}

uint8_t btc_rail_start_dma_transfer(RAIL_RxPacketInfo_t *pRxPkt, RAIL_RxPacketDetails_t *pRxPktDetails, btc_trx_configs_t *btc_trx_configs)
{
  uint32_t scatters = 0;
  uint32_t status;
  RAIL_UserCpReq_t *head = &cp_req[0];
  RAIL_UserCpReq_t *tail = NULL;

  if (pRxPkt == NULL || pRxPktDetails == NULL || btc_trx_configs == NULL) {
    BTC_ASSERT(NULL_POINTER);
    return NULL_POINTER;
  }

  uint32_t rcvd_pkt_len = pRxPkt->packetBytes;
  uint32_t firstPortionBytes = pRxPkt->firstPortionBytes;
  uint32_t lastPortionBytes = (rcvd_pkt_len - firstPortionBytes);

  if ((status = btc_rail_prepare_dma_descriptors(pRxPkt->firstPortionData, firstPortionBytes, &scatters, BTC_LPW_RX_METADATA_SIZE)) != 0) {
    return status;
  }
  if ((status = btc_rail_prepare_dma_descriptors(pRxPkt->lastPortionData, lastPortionBytes, &scatters, 0)) != 0) {
    return status;
  }
  btc_trx_configs->rx_meta_data.rx_pkt_meta_data.rssi = pRxPktDetails->rssi;
  pkb_t *cur_buff = (pkb_t *)(cp_req[0].pDst - (ulc_llc_share_mem->metadata_size + BTC_LPW_RX_METADATA_SIZE));
  *(uint32_t *)((uint8_t *)cur_buff + (ulc_llc_share_mem->metadata_size)) = btc_trx_configs->rx_meta_data.data;

  for (uint32_t idx = 1; idx < scatters; idx++) {
    cur_buff->scatter_node.next = (node_t *)(cp_req[idx].pDst - ulc_llc_share_mem->metadata_size);
    cur_buff = (pkb_t *)cur_buff->scatter_node.next;
  }
  cur_buff->scatter_node.next =  NULL;
  cp_req[--scatters].pNext = NULL;
  cp_req[scatters].pCpReqCB = btc_rail_dma_done_cb;
  tail = &cp_req[scatters];
  if (RAIL_SEQ_StartCpReq(head, tail) != RAIL_STATUS_NO_ERROR) {
    return DMA_TRIGGER_FAILED;
  }
  return RAIL_STATUS_NO_ERROR;
}

void btc_rail_dma_done_cb(const void *pCpReq, uint32_t reqStatus)
{
  (void)pCpReq;
  if (reqStatus == RAIL_STATUS_NO_ERROR) {
    const RAIL_UserCpReq_t *cpReq = &cp_req[0];
    pkb_t *pkb = (pkb_t *)((uint8_t *)cpReq->pDst - (ulc_llc_share_mem->metadata_size + BTC_LPW_RX_METADATA_SIZE));
    btc_dev.curr_rx_configs->rx_meta_data.rx_pkt_meta_data.event_type = BTC_LLC_TO_ULC_DMA_TRANSFER_DONE;
    *((uint32_t *)((uint8_t *)pkb + ulc_llc_share_mem->metadata_size)) = btc_dev.curr_rx_configs->rx_meta_data.data;
    if (pkb != NULL) {
      enqueue(&ulc_llc_share_mem->rx_event_buffers_queue, &pkb->queue_node);
    }
  } else {
    BTC_ASSERT(DMA_TRANSFER_FAILED);
  }
  RAIL_SEQ_SendMbox(BTC_LLC_TO_ULC_DMA_TRANSFER_DONE);
  dma_triggered = 0;
}

void BTC_RAIL_SEQ_rxCb(RAIL_RxPacketInfo_t *pRxPkt, RAIL_RxPacketDetails_t *pRxPktDetails)
{
  uint8_t rx_status;
  btc_dev_t *btc_dev_ptr = BTC_DEV_PTR;
  btc_trx_configs_t *btc_trx_configs = btc_dev.curr_rx_configs;
  header_t *rx_pkt_hdr;

  if (pRxPkt->firstPortionBytes == 0) {
    btc_dev_ptr->current_slot_info.crc_status = CRC_FAIL;
    INC(stats.rx_packet_aborted);
    debug_log_event(DEBUG_PTI, PTI_RX_ABORT);
    sl_btdm_fast_log(btc_log_rail_rx_abort_cb, COMPONENT_SEQ_BTC, TRACE, "RAIL RX Aborted with status {CRC: %d} and {RSSI: %d}", pRxPkt->packetStatus, pRxPktDetails->rssi);
    SM_STEP(RAIL_SM, btc_dev_ptr->l2_scheduled_sm, RX_ABORT);
    return;
  }

  // Update the Rx packet header
  rx_pkt_hdr = (header_t *)(pRxPkt->firstPortionData);
  memcpy(&btc_dev_ptr->rx_pkt_hdr, rx_pkt_hdr, sizeof(header_t));

  debug_log_event(DEBUG_PTI, PTI_RX_DONE_CB);
  debug_log_event(DEBUG_PTI, pRxPkt->packetStatus);
  INC(stats.rx_packet_received);
  if (pRxPkt->packetStatus == RAIL_RX_PACKET_READY_SUCCESS) {
    rx_status = RX_DONE;
    debug_log_event(DEBUG_PTI, PTI_RX_DONE);
    received_rx_pkt = (pRxPkt->firstPortionData + 3);
    received_rx_pkt_len = pRxPkt->packetBytes - 3;
    INC(stats.rx_packet_done);
    btc_dev_ptr->current_slot_info.crc_status = CRC_PASS;
  } else {
    rx_status = RX_ABORT;
    debug_log_event(DEBUG_PTI, PTI_RX_ABORT);
    btc_dev_ptr->current_slot_info.crc_status = CRC_FAIL;
    INC(stats.rx_packet_aborted);
  }
  sl_btdm_fast_log(btc_log_rail_rx_done_cb, COMPONENT_SEQ_BTC, TRACE, "RAIL RX Done CB Received status {CRC: %d} and {RSSI: %d}", pRxPkt->packetStatus, pRxPktDetails->rssi);

  // TODO: Remove this if RAIL provides a solution for callback being called after stop
  if (btc_dev_ptr->l2_scheduled_sm == NULL) {
    sl_btdm_fast_log(btc_log_l2_scheduled_sm_assert, COMPONENT_SEQ_BTC, TRACE, "RX CB: l2_scheduled_sm is NULL");
    return;
  }

  if (btc_trx_configs->abort_rx_pkt) {
    btc_trx_configs->abort_rx_pkt = 0;
    SM_STEP(RAIL_SM, btc_dev_ptr->l2_scheduled_sm, RX_ABORT);
  } else {
    SM_STEP(RAIL_SM, btc_dev_ptr->l2_scheduled_sm, rx_status);
  }
  // Remove the last Rx packet from the Rx queue. This needs to be done even for the abort case.
  RAIL_SEQ_RemoveLastRxPkt();

  if (pRxPkt->packetStatus == RAIL_RX_PACKET_READY_SUCCESS) {
    if (!btc_trx_configs->ignore_rx_pkt) {
      uint8_t dma_status = btc_rail_start_dma_transfer(pRxPkt, pRxPktDetails, btc_trx_configs);
      dma_triggered = dma_status == 0 ? 1 : 0;
    }
  }
}

void BTC_RAIL_SEQ_rxSearchTimeoutCb(RAIL_SeqRxSearchTimeoutCause_t rxTimeoutCause)
{
  uint8_t rx_status = rx_search_timeout_cb_lookup[rxTimeoutCause][0];
  btc_dev_t *btc_dev_ptr = BTC_DEV_PTR;
  debug_log_event(DEBUG_PTI, PTI_RX_TIMEOUT_CB);
  debug_log_event(DEBUG_PTI, rx_search_timeout_cb_lookup[rxTimeoutCause][1]);
  sl_btdm_fast_log(btc_log_rail_rx_search_timeout_cb, COMPONENT_SEQ_BTC, TRACE, "{Root Cause: %1x} {status: %1x}", rxTimeoutCause, rx_status);

  // Update HEC status
  if (rx_status == RX_ABORT) {
    btc_dev_ptr->current_slot_info.hec_status = HEC_FAIL;
  }
  // TODO: Remove this if RAIL provides a solution for callback being called after stop
  if (btc_dev_ptr->l2_scheduled_sm == NULL) {
    sl_btdm_fast_log(btc_log_l2_scheduled_sm_assert, COMPONENT_SEQ_BTC, TRACE, "ASSERT: RX Search timeout CB: l2_scheduled_sm is NULL");
    return;
  }

  btc_dev_ptr->l2_scheduled_sm->last_rx_slots = 0;

  if (rxTimeoutCause == RAIL_SEQ_RX_TIMEOUT || rxTimeoutCause == RAIL_SEQ_RX_WINDOW_END) {
    INC(stats.rx_timeout);
  } else if (rxTimeoutCause == RAIL_SEQ_BTC_HEC_ERROR_RX_TIMEOUT) {
    INC(stats.hec_error);
    INC(stats.rx_packet_received);
  } else if (rxTimeoutCause == RAIL_SEQ_BTC_ID_PKT_RX_TIMEOUT) {
    INC(stats.id_pkt_rx);
    INC(stats.rx_packet_received);
  }

  if (btc_dev.curr_rx_configs->abort_rx_pkt) {
    btc_dev.curr_rx_configs->abort_rx_pkt = 0;
    SM_STEP(RAIL_SM, btc_dev_ptr->l2_scheduled_sm, RX_ABORT);
  } else {
    SM_STEP(RAIL_SM, btc_dev_ptr->l2_scheduled_sm, rx_status);
  }
}

uint8_t BTC_RAIL_SEQ_RxPktCb(uint8_t *packet, uint32_t *pRxBytes)
{
  uint16_t *pkt_hdr = (uint16_t *)packet;
  uint8_t num_of_slots, pkt_type;
  btc_dev_t *btc_dev_ptr = BTC_DEV_PTR;
  btc_trx_configs_t *btc_trx_configs = btc_dev.curr_rx_configs;
  header_t *rx_pkt_hdr;
  uint32_t rxTimeout;

  debug_log_event(DEBUG_PTI, PTI_PKT_HDR_EVENT_CB);
  rx_pkt_hdr = (header_t *)(pkt_hdr);
  memcpy(&btc_dev_ptr->rx_pkt_hdr, rx_pkt_hdr, sizeof(header_t));
  pkt_type = rx_pkt_hdr->type;
  num_of_slots = packet_type_slot_lookup_table[pkt_type][btc_trx_configs->btc_rail_trx_config.linkConfig.enhancedRate];
  debug_log_event(DEBUG_PTI, pkt_type);
  debug_log_event(DEBUG_PTI, num_of_slots);

  // TODO: Remove this if RAIL provides a solution for callback being called afterg stop
  if (btc_dev_ptr->l2_scheduled_sm == NULL) {
    sl_btdm_fast_log(btc_log_l2_scheduled_sm_assert, COMPONENT_SEQ_BTC, TRACE, "ASSERT: RX pkt CB: l2_scheduled_sm is NULL");
    return 0;
  }
  btc_dev_ptr->l2_scheduled_sm->last_rx_slots = num_of_slots;
  rxTimeout = (num_of_slots * BT_SLOT_WIDTH) - BTC_MIN_RAIL_RX_TIMEOUT;
  // Update the Rx timeout based on the received packet type
  RAIL_BTC_SET_RX_TIMEOUT(rxTimeout);
  // Update the HEC status
  btc_dev_ptr->current_slot_info.hec_status = HEC_PASS;
  *(pRxBytes) = 0; // To inform the rail that the packet header is processed
  if (btc_trx_configs->rx_hdr_needed) {
    btc_trx_configs->rx_hdr_needed = 0;
    SM_STEP(RAIL_SM, btc_dev_ptr->l2_scheduled_sm, RX_HEADER_RECEIVED);
    if (btc_trx_configs->abort_rx_pkt) {
      return ~USER_SEQ_PKT_BUF_CONT; // Abort the Rx if the abort flag is set
    }
  }
  return USER_SEQ_PKT_BUF_CONT; // Continue the Rx if the abort flag is not set
}

uint32_t rail_schedule_id_tx(btc_trx_configs_t *btc_trx_configs)
{
  uint32_t status;
  uint32_t txpktstatus;
  uint8_t ppDst[2] = { 0xFF, 0xFF }; // Dummy bytes for ID packet
  btc_trx_configs->btc_rail_trx_config.txPacketLen = 2;

  // Update schedule tx timing information
  scheduletxconfig_pticks.when = RAIL_SEQ_BTC_ConvertBtcDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk);

  txpktstatus = RAIL_SEQ_SetTxPkt(ppDst, 2);
  if (txpktstatus != RAIL_STATUS_NO_ERROR) {
    sl_btdm_fast_log(btc_log_rail_id_SetTxPkt_failed, COMPONENT_SEQ_BTC, TRACE, "{error: %d}", txpktstatus);
    return txpktstatus;
  }
#ifndef RUN_TEST_MODE
  if (ENABLE_SLOW_HOP) {
    RAIL_SEQ_SetChannel(btc_trx_configs->btcFhResult.channelNum);
  }
#endif
  RAIL_SEQ_BTC_PrepareTxRx(&btc_trx_configs->btc_rail_trx_config);

  debug_log_event(DEBUG_PTI, PTI_SCHEDULED_ID_TX_CALL);
  status = RAIL_SEQ_StartScheduledTxPtick(&scheduletxconfig_pticks);
  sl_btdm_fast_log(btc_log_rail_schedule_id_tx, COMPONENT_SEQ_BTC, TRACE, "ID Packet TX Scheduled at clk id %d BTCDate %d and tsf %d", btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk, scheduletxconfig_pticks.when >> TSF_SCALE_FACTOR);
  INC(stats.schedule_id_tx);
  return status;
}

uint32_t rail_schedule_id_rx(btc_trx_configs_t *btc_trx_configs)
{
  uint32_t status;
  uint32_t end_tsf = btc_trx_configs->rx_window;

  btc_trx_configs->btc_rail_trx_config.rxTimeout = BTC_MIN_RAIL_ID_RX_TIMEOUT;
  // Update start schedule rx values
  schedulerxconfig_pticks.start = RAIL_SEQ_BTC_ConvertBtcDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk);
  schedulerxconfig_pticks.end = schedulerxconfig_pticks.start + (end_tsf << TSF_SCALE_FACTOR);

  debug_log_event(DEBUG_PTI, PTI_SCHEDULED_ID_RX_CALL);
#ifndef RUN_TEST_MODE
  if (ENABLE_SLOW_HOP) {
    RAIL_SEQ_SetChannel(btc_trx_configs->btcFhResult.channelNum);
  }
#endif
  RAIL_SEQ_BTC_PrepareTxRx(&btc_trx_configs->btc_rail_trx_config);
  status = RAIL_SEQ_StartScheduledRxPtick(&schedulerxconfig_pticks);
  sl_btdm_fast_log(btc_log_rail_schedule_id_rx, COMPONENT_SEQ_BTC, TRACE, "ID Packet RX Scheduled at clk id %d BTCDate %d and tsf %d", btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk, schedulerxconfig_pticks.start >> TSF_SCALE_FACTOR);
  btc_dev.curr_rx_configs = btc_trx_configs;
  INC(stats.schedule_id_rx);
  return status;
}

uint32_t rail_schedule_tx(btc_trx_configs_t *btc_trx_configs)
{
  uint32_t status;
  uint32_t txpktstatus;

  txpktstatus = RAIL_SEQ_SetTxPkt(btc_trx_configs->tx_pkt, btc_trx_configs->btc_rail_trx_config.txPacketLen);
  if (txpktstatus != RAIL_STATUS_NO_ERROR) {
    sl_btdm_fast_log(btc_log_rail_SetTxPkt_failed, COMPONENT_SEQ_BTC, TRACE, "{error: %d}", txpktstatus);
    return txpktstatus;
  }  // Update schedule tx timing information
  scheduletxconfig_pticks.when = RAIL_SEQ_BTC_ConvertBtcDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk);
#ifndef RUN_TEST_MODE
  if ((!btc_trx_configs->ignore_whitening)  && (ENABLE_SLOW_HOP)) {
    if (btc_trx_configs->is_connected_procedure) {
      RAIL_SEQ_BTC_setWhitening(((btc_trx_configs->btc_fh_data->masterClk & 0x3F) | 0x40));
    } else {
      RAIL_SEQ_BTC_setWhitening(((btc_trx_configs->btcFhResult.xVal & 0x3F) | 0x60));
    }
    RAIL_SEQ_SetChannel(btc_trx_configs->btcFhResult.channelNum);
  } else {
    RAIL_SEQ_BTC_setWhitening(0);
  }
#endif

  RAIL_SEQ_BTC_PrepareTxRx(&btc_trx_configs->btc_rail_trx_config);
  // Update the encryption parameters based on the encryption mode
  if (btc_trx_configs->btc_rail_trx_config.encryptLink == ENCRYPTION_MODE_E0) {
    RAIL_SEQ_BTC_setE0Encryption(btc_trx_configs->e0EncConfig);
  } else if (btc_trx_configs->btc_rail_trx_config.encryptLink == ENCRYPTION_MODE_AES) {
    RAIL_SEQ_BTC_setAESEncryption(btc_trx_configs->aesEncConfig);
  }

  debug_log_event(DEBUG_PTI, PTI_SCHEDULED_TX_CALL);

  status = RAIL_SEQ_StartScheduledTxPtick(&scheduletxconfig_pticks);
  sl_btdm_fast_log(btc_log_rail_schedule_tx, COMPONENT_SEQ_BTC, TRACE, "Packet TX Scheduled at clk id %d BTCDate %d and tsf %d", btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk, scheduletxconfig_pticks.when >> TSF_SCALE_FACTOR);
  INC(stats.schedule_tx);
  return status;
}

uint32_t rail_schedule_rx(btc_trx_configs_t *btc_trx_configs)
{
  uint32_t status;
  uint32_t end_tsf = btc_trx_configs->rx_window;
  btc_trx_configs->btc_rail_trx_config.rxTimeout = BTC_MIN_RAIL_RX_TIMEOUT;

  // Update start schedule rx values
  schedulerxconfig_pticks.start = RAIL_SEQ_BTC_ConvertBtcDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk);
  schedulerxconfig_pticks.end = schedulerxconfig_pticks.start + (end_tsf << TSF_SCALE_FACTOR);
  debug_log_event(DEBUG_PTI, PTI_SCHEDULED_RX_CALL);

#ifndef RUN_TEST_MODE
  if ((!btc_trx_configs->ignore_whitening)  && (ENABLE_SLOW_HOP)) {
    if (btc_trx_configs->is_connected_procedure) {
      RAIL_SEQ_BTC_setWhitening(((btc_trx_configs->btc_fh_data->masterClk & 0x3F) | 0x40));
    } else {
      RAIL_SEQ_BTC_setWhitening(((btc_trx_configs->btcFhResult.xVal & 0x1F) | 0x60));
    }
    RAIL_SEQ_SetChannel(btc_trx_configs->btcFhResult.channelNum);
  } else {
    RAIL_SEQ_BTC_setWhitening(0);
  }
#endif

  RAIL_SEQ_BTC_PrepareTxRx(&btc_trx_configs->btc_rail_trx_config);
  // Update the encryption parameters based on the encryption mode
  if (btc_trx_configs->btc_rail_trx_config.encryptLink == ENCRYPTION_MODE_E0) {
    RAIL_SEQ_BTC_setE0Encryption(btc_trx_configs->e0EncConfig);
  } else if (btc_trx_configs->btc_rail_trx_config.encryptLink == ENCRYPTION_MODE_AES) {
    RAIL_SEQ_BTC_setAESEncryption(btc_trx_configs->aesEncConfig);
  }
  status = RAIL_SEQ_StartScheduledRxPtick(&schedulerxconfig_pticks);
  sl_btdm_fast_log(btc_log_rail_schedule_rx, COMPONENT_SEQ_BTC, TRACE, "Packet RX Scheduled at clk id %d BTCDate %d and tsf %d", btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk, schedulerxconfig_pticks.start >> TSF_SCALE_FACTOR);
  btc_dev.curr_rx_configs = btc_trx_configs;
  INC(stats.schedule_rx);
  return status;
}

uint32_t get_current_clk()
{
  return RAIL_SEQ_GetTime();
}

uint32_t rail_get_channel(btc_csa_procedure_t csa_procedure, uint32_t bd_clk)
{
  /* TODO: will be fully functional with CSA horizontal integration */
  (void)csa_procedure;
  (void)bd_clk;
  return 0;
}

uint32_t rail_init_bd_clk_counter(uint32_t clk_offset)
{
  (void)clk_offset;
  return 0;
}

buffc_t *trigger_buffc_if_payload_present(buffc_t *buffc, uint16_t length)
{
  (void)buffc;
  (void)length;
  return NULL;
}

uint32_t rail_is_buffc_triggered(buffc_t *buffc)
{
  (void)buffc;
  return 0;
}

void rail_process_arqn_stage1()
{
}

void wait_for_bufc_transfer_status()
{
}

void rail_process_arqn_stage2()
{
}

void rail_wait_for_bufc_transfer_status()
{
}

uint8_t rail_get_hec_status()
{
  return 0;
}

uint8_t rail_get_crc_status()
{
  return 0;
}

uint8_t rail_get_mic_status()
{
  return 0;
}

void rail_process_rx_header_info()
{
}

#ifdef RUN_TEST_MODE
void rail_get_stats()
{
  RAIL_SEQ_SendMbox((uint32_t)&stats);
}
#endif

void rail_btc_mbox(uint32_t msg_data)
{
  // RAIL_SEQ_SetPtiAuxdataOutputMasked(RAIL_PTI_MASK_AUXDATA, 'M');
#ifdef RUN_TEST_MODE
  if (msg_data == TEST_MODE_STATS_MSG) {
    rail_get_stats();
  }
#endif
  ulc_llc_share_mem = (shared_mem_t *)msg_data;
  lpw_mailbox_handler(ulc_llc_share_mem, &btc_dev);
}

sl_btdm_fast_log_entry_t fast_log_entry[32];
sl_btdm_logging_meta_data_t logging_meta_data;

void plt_enter_critical()
{
}

void plt_exit_critical()
{
}
inline uint32_t plt_get_timestamp()
{
  //return (uint32_t)RAIL_SEQ_GetTime();
  return 0;
}

void rail_set_btc_clk_instance(bd_clk_counter_t *bd_clk_counter)
{
  cbc.pCurrentBtcClockInfo = &BtcClkInfo[bd_clk_counter->bd_clk_counter_id];
}

void rail_user_btc_int(void)
{
  RAIL_SEQ_StartPktBuf(0, BTC_RAIL_PKT_CB_THRESHOULD);
  cbc.pMsgCB = &rail_btc_mbox;
  cbc.pRxCB = &BTC_RAIL_SEQ_rxCb;
  cbc.pRxSearchEndNoDataCB = &BTC_RAIL_SEQ_rxSearchTimeoutCb;
  cbc.pTxCB = &BTC_RAIL_SEQ_txCb;
  cbc.pPktCB = &BTC_RAIL_SEQ_RxPktCb;
  // RAIL_SEQ_SetPtiAuxdataOutputMasked(RAIL_PTI_MASK_AUXDATA, 'I');
  configc.status = RAIL_SEQ_RegCB(&cbc);
  sl_btdm_fast_logging_init(fast_log_entry, &logging_meta_data, 32);
}

void  *get_tx_from_fifo_if_available(void)
{
  //Get the address of the fifo_manager from the shared memory location
  if (g_tx_acl_buffer_address == 0) {
    return NULL;
  }
  // Check if FIFO has data available
  buffer_fifo_t *acl_fifo_inst = (buffer_fifo_t *)(g_tx_acl_buffer_address);
  if (((acl_fifo_inst->read_index & HSS_LPWSS_FIFO_MASK) != (acl_fifo_inst->write_index & HSS_LPWSS_FIFO_MASK))) {
    linear_buffer_t *linear_buffer = (linear_buffer_t *)(acl_fifo_inst->data_buffer[acl_fifo_inst->read_index]);
    return linear_buffer;
  }
  return NULL;
}
