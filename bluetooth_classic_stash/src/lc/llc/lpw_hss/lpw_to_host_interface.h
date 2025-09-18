#ifndef LPW_TO_HOST_INTF_H_
#define LPW_TO_HOST_INTF_H_

#include "sl_btc_seq_interface.h"
#include "stdint.h"
#include "string.h"
#include "state_machine.h"
#include "btc_device.h"

void btc_lpw_activity_complete_indication_to_ull(pkb_t *pkb);
void prep_hss_internal_event(uint8_t *hss_event, btc_internal_event_type_t event_type, btc_internal_event_status_t status);
void prep_hss_page_complete_event(uint8_t *hss_event, hss_event_page_complete_t* page_complete);
void prep_hss_page_scan_complete_event(uint8_t *hss_event, hss_event_page_scan_complete_t* page_scan_complete);
void prep_hss_roleswitch_slot_offset_event(uint8_t *hss_event, hss_event_btc_slot_offset_t* roleswitch_slot_offset, uint8_t peer_role);
#endif
