#include "arqn_seqn_check.h"
#include "btc_common_defines.h"
#include "btc_rail.h"
#include "stdint.h"
#include "btc_device.h"
#ifndef SIM
#include "btc_rail_intf.h"
extern shared_mem_t *ulc_llc_share_mem;
#else
#include "rail_simulation.h"
#endif

/**
 *  @brief Function to perform stage 1 operation(ARQN/SEQN assignment process)
 * for ACL packets recieved
 *
 * @param
 */
uint8_t arqn_stage_1_acl(connection_info_t *connection_info_p)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  arqn_seqn_scheme_status_t *arqn_seqn_status = &connection_info_p->acl_arqn_seqn_info;
  header_t *pkt_header_p = &btc_dev_p->rx_pkt_hdr;

  if (arqn_lt_addr_check(pkt_header_p->lt_addr, &btc_dev_p->current_slot_info)) {
    btc_trx_configs->abort_rx_pkt = 0; // To keep the ongoing Rx as the LT address is matching
    arqn_seqn_status->remote_flow = pkt_header_p->flow;
    if ((pkt_header_p->type != BT_POLL_PKT_TYPE) && (pkt_header_p->arqn == ACK) && (arqn_seqn_status->arqn_status == ACK)) {
      // Update the SEQN
      update_seqn(&connection_info_p->acl_arqn_seqn_info);
      update_acl_enc_pkt_count(connection_info_p->encryption_mode, connection_info_p->acl_enc_tx_pkt_count);
#ifndef SIM
      buffer_fifo_t *acl_fifo_inst = (buffer_fifo_t *)(g_tx_acl_buffer_address);
      acl_fifo_inst->read_index = (acl_fifo_inst->read_index + 1) & HSS_LPWSS_FIFO_MASK;
      send_tx_pkt_ack_notification_to_ulc();
#endif
    }
    return 1; // Return 1 to indicate that the LT address check is passed
  } else {
    debug_log_event(DEBUG_PTI, PTI_ACL_LT_ADDR_MISMATCH);
    arqn_seqn_status->arqn_status = NAK;

    // Cancel the ongoing Rx as the LT address is not matching
    btc_trx_configs->ignore_rx_pkt = 1; // To avoid DMA transfer
    // TODO: Check if can we use any RAIL API to abort the ongoing Rx
    btc_trx_configs->abort_rx_pkt = 1; // To abort the ongoing Rx as the LT address is not matching
    return 0; // Return 0 to indicate that the LT address check is failed
  }
}

/**
 *  @brief Function to perform stage 2 operation(ARQN/SEQN assignment process)
 * for ACL packets recieved
 *
 * @param
 */
void arqn_stage_2_acl(connection_info_t *connection_info_p)
{
  btc_dev_t* btc_dev_p = BTC_DEV_PTR;
  btc_trx_configs_t *btc_trx_configs = &connection_info_p->trx_config;
  header_t *pkt_header_p = &btc_dev_p->rx_pkt_hdr;
  arqn_seqn_scheme_status_t *arqn_seqn_status = &connection_info_p->acl_arqn_seqn_info;

  if ((btc_dev_p->rx_pkt_hdr.type == BT_POLL_PKT_TYPE) || (btc_dev_p->rx_pkt_hdr.type == BT_NULL_PKT_TYPE)) {
    btc_trx_configs->ignore_rx_pkt = 1; // To avoid DMA transfer for the received POLL/NULL packet
    if (btc_dev_p->rx_pkt_hdr.type == BT_NULL_PKT_TYPE) {
      arqn_seqn_status->arqn_status = NAK; // Avoid sending response to NULL packet
    }
  } else {
    arqn_seqn_status->arqn_status = ACK;
    btc_trx_configs->ignore_rx_pkt = 0; // To allow DMA transfer for the received packet
  }

  if (pkt_header_p->type == BT_DV_PKT_TYPE) {
    // TODO, Would be Handled in Horizontal Integration
    // Split DV into SCO and ACL
    // Add SCO packet to SCO queue
    // Update current packet start to DV ACL packet header
    // Will be moving to a seperate Handle where the above mentioned process would be taken care of
  }
  if (pkt_header_p->type != BT_POLL_PKT_TYPE && pkt_header_p->type != BT_NULL_PKT_TYPE) {
    if (pkt_header_p->seqn == arqn_seqn_status->remote_seqn) {
      btc_trx_configs->ignore_rx_pkt = 1; // To avoid DMA transfer for the duplicate payload
    } else {
      arqn_seqn_status->remote_seqn = pkt_header_p->seqn;
      btc_trx_configs->ignore_rx_pkt = 0; // To allow DMA transfer for the new payload
      update_acl_enc_pkt_count(connection_info_p->encryption_mode, connection_info_p->acl_enc_rx_pkt_count);
    }
  }
}

/**
 *  @brief Function to perform in the TX side. This function determins the
 * payload to be sent with the updated SEQN status
 *
 * @param
 */
void update_seqn(arqn_seqn_scheme_status_t *arqn_seqn_status)
{
  arqn_seqn_status->seqn_status = ~arqn_seqn_status->seqn_status;
}

/**
 * @brief Function to check the validity of LT_Address being passed
 *        If this function returns 1 -  The ARQN LT Address check is passed
 *        If this function returns 0 -  The ARQN LT Address check is failed
 * @param arqn_seqn_status_t
 */
uint8_t arqn_lt_addr_check(uint8_t rcvd_lt_address, current_slot_info_t *current_slot_info)
{
  uint8_t esco_retransmit_slot = 0; //  TODO: This Variable would be used temporarily, and during the Horizontal Code Integration we would be making use of ESCO specific Variables
  uint8_t status = 0;
  if (current_slot_info->current_role_lt_addr == rcvd_lt_address) { //  Check whether the LT address received is the same w.r.t the current type of LT addr, on which the ARQN/SEQN technique is being processed.
    status = TRUE;
  } else {
    if (esco_retransmit_slot & (rcvd_lt_address == current_slot_info->current_role_acl_lt_addr)) { //  Postive, Only during Retransmission slots of Esco Window and the LT address received is ACL LT_addr
      status = TRUE;
    } else {
      status = FALSE;
    }
  }
  return status;
}
