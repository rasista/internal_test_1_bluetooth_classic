/**
 * @file lpw_to_host_interface.c
 * @brief Handles LPW procedure completion indications to ULL.
 * @version 0.1
 * @date 2023-01-18
 */

#include "btc_device.h"
#ifdef SIM
#include "rail_simulation.h"
#else
#include "btc_rail_intf.h"
#endif
#include "lpw_to_host_interface.h"

void btc_lpw_activity_complete_indication_to_ull(pkb_t *pkb)
{
  // Call BTC_post_notification_to_ulc
  BTC_post_notification_to_ulc(pkb);
}

void prep_hss_internal_event(uint8_t *hss_event, btc_internal_event_type_t event_type, btc_internal_event_status_t status)
{
  *HSS_EVENT_EVENT_TYPE(hss_event) = event_type;
  *HSS_EVENT_EVENT_STATUS(hss_event) = status;
}

void prep_hss_page_complete_event(uint8_t *hss_event, hss_event_page_complete_t* page_complete)
{
  *HSS_EVENT_PAYLOAD_PAGE_COMPLETE_CONNECTION_INFO_IDX(hss_event) = page_complete->connection_info_idx;
}

void prep_hss_page_scan_complete_event(uint8_t *hss_event, hss_event_page_scan_complete_t* page_scan_complete)
{
  *HSS_EVENT_PAYLOAD_PAGE_SCAN_COMPLETE_CONNECTION_INFO_IDX(hss_event) = page_scan_complete->connection_info_idx;
  memcpy(HSS_EVENT_PAYLOAD_PAGE_SCAN_COMPLETE_REMOTE_BD_ADDRESS(hss_event), page_scan_complete->remote_bd_address, BD_ADDR_LEN);
}

void prep_hss_roleswitch_slot_offset_event(uint8_t *hss_event, hss_event_btc_slot_offset_t* roleswitch_slot_offset, uint8_t peer_role)
{
  if (peer_role == BTC_CENTRAL) {
    *HSS_EVENT_BTC_SLOT_OFFSET_ROLESWITCH_SLOT_OFFSET(hss_event) = 0;
  } else {
    *HSS_EVENT_BTC_SLOT_OFFSET_ROLESWITCH_SLOT_OFFSET(hss_event) = roleswitch_slot_offset->roleswitch_slot_offset;
  }
}
