#ifndef _BTC_HCI_EVENTS_H
#define _BTC_HCI_EVENTS_H

#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"

void hci_inquiry_complete_indication(ulc_t *ulc, uint8_t event_map_mask, int8_t status);
void hci_inquiry_result(ulc_t *ulc, hci_inq_event_params_t *hci_inq_event_params);
void hci_conn_request_indication(ulc_t *ulc, uint8_t peer_id, uint8_t link_type);
void hci_auth_complete(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void hci_remote_dev_name_recvd_indication(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void hci_change_conn_link_key_complete(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void hci_remote_dev_features_recvd(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void hci_qos_setup_complete_indication(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void hci_return_link_keys_event(ulc_t *ulc, uint8_t no_of_link_keys, uint8_t *bd_addrs);
void hci_pin_code_req(ulc_t *ulc, uint8_t peer_id);
void hci_link_key_req(ulc_t *ulc, uint8_t peer_id);
void hci_link_key_notification(ulc_t *ulc, uint8_t peer_id, uint8_t link_key_type);
void hci_max_slots_changed_indication(ulc_t *ulc, uint8_t peer_id);
void hci_read_clk_offset_complete(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void hci_inquiry_result_with_rssi(ulc_t *ulc, hci_inq_event_params_t *hci_inq_event_params);
void hci_read_remote_ext_features_complete(ulc_t *ulc, uint8_t status, uint8_t peer_id, uint8_t page_num);
void hci_sniff_subrating_event(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void hci_extended_inquiry_result(ulc_t *ulc, hci_inq_event_params_t *hci_inq_event_params);
void hci_io_cap_req_event(ulc_t *ulc, uint8_t peer_id);
void hci_io_cap_response_event(ulc_t *ulc, uint8_t peer_id);
void hci_user_confirmation_req_event(ulc_t *ulc, uint8_t peer_id, uint32_t confirmation_val);
void hci_user_passkey_request(ulc_t *ulc, uint8_t peer_id);
void hci_remote_oob_data_request(ulc_t *ulc, uint8_t peer_id);
void hci_simple_pairing_complete(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void hci_link_supervision_timeout(ulc_t *ulc, uint8_t peer_id);
void hci_user_passkey_notification(ulc_t *ulc, uint8_t peer_id);
void hci_keypress_notification_event(ulc_t *ulc, uint8_t peer_id, uint8_t notification_type);
void hci_remote_host_supp_features_notification_event(ulc_t *ulc, uint8_t peer_id);
void hci_role_change_event(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void hci_mode_change_event(ulc_t *ulc, uint16_t conn_handle, uint8_t status, uint8_t mode, uint16_t interval);
void hci_tx_pkts_flush_occurred_event(ulc_t *ulc, uint8_t peer_id, uint8_t event_type);
void hci_conn_complete_indication(ulc_t *ulc, uint8_t slave_id, uint8_t status, uint8_t conn_type);
void hci_conn_pkt_type_changed_event(ulc_t *ulc, uint8_t peer_id, uint8_t link_type, uint8_t status);
void hci_sync_conn_complete(ulc_t *ulc, uint8_t peer_id, uint8_t link_type, uint8_t status);
void hci_sync_conn_changed(ulc_t *ulc, uint8_t peer_id, uint8_t link_type, uint8_t status);
void hci_enc_changed_event(ulc_t *ulc, uint16_t conn_handle, uint8_t status, uint8_t enc_enabled);
void hci_disconn_complete_indication(ulc_t *ulc, uint16_t conn_handle, uint8_t status, uint8_t reason);
void hci_read_remote_dev_vers_info_complete(ulc_t *ulc, uint16_t conn_handle, uint8_t peer_id, uint8_t status);
void hci_vendor_event(uint8_t *data);
#endif
