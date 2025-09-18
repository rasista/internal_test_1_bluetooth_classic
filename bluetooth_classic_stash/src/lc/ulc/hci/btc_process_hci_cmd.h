#ifndef _BT_PROCESS_HCI_CMD_H
#define _BT_PROCESS_HCI_CMD_H

#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"

uint8_t bt_change_sco_conn(ulc_t *ulc, uint8_t peer_id, uint8_t sco_handle, uint16_t pkt_type);
void bt_store_link_key(ulc_t *ulc, uint8_t *bd_addr, uint8_t *link_key);
uint8_t bt_mac_addr_to_peer_id(ulc_t *ulc, uint8_t *bd_addr);
void dm_reject_connection_from_hci(ulc_t *ulc, uint8_t *remote_dev_bd_addr, uint8_t role);
void dm_accept_connection_from_hci(ulc_t *ulc, uint8_t *remote_dev_bd_addr, uint8_t role);
uint8_t bt_create_connection_cancel(ulc_t *ulc, uint8_t *bd_addr);
uint8_t bt_check_conn(ulc_t *ulc, conn_params_t *conn_params, uint8_t *peer_id);
uint8_t bt_create_conn(ulc_t *ulc, conn_params_t *conn_params, pkb_t *pkb);
uint8_t bt_conn_handle_to_peer_id(ulc_t *ulc, uint16_t conn_handle);
uint8_t bt_remote_name_req_cancel(ulc_t *ulc, uint8_t *bd_addr);
uint8_t dm_remote_dev_name_req_from_hci(ulc_t *ulc, uint8_t *bd_addr, uint8_t page_scan_rep_mode, uint16_t clk_offset);
uint8_t bt_allow_conn_req_from_host(ulc_t *ulc);
link_key_info_t *bt_find_link_key(ulc_t *ulc, uint8_t *bd_addr);
void generate_rand_num(uint8_t rand_num_bytes, uint8_t *rand_num, uint8_t random_num_type);
uint8_t dm_enable_sniff_mode_from_hci(ulc_t *ulc, uint8_t peer_id, pkb_t *pkb);
void bt_delete_link_key(ulc_t *ulc, uint8_t *bd_addr);
uint8_t dm_read_rssi_for_conn(ulc_t *ulc, uint16_t peer_id);
uint16_t bt_peer_id_to_conn_handle(ulc_t *ulc, uint8_t slave_id);
uint16_t lc_cal_acl_max_payload_len(uint8_t data_rate_type, uint16_t pkt_type, uint8_t max_slot);
void allocate_resp_addr_list(btc_dev_info_t *btc_dev_info_p);
void free_resp_addr_list(btc_dev_info_t *btc_dev_info_p);
void bt_delete_link_key(ulc_t *ulc, uint8_t *bd_addr);
uint8_t bt_disconnect(ulc_t *ulc, uint8_t peer_id, uint8_t reason);
void dm_acl_disconn_ind(ulc_t *ulc, uint8_t peer_id, uint8_t reason_code);
void bt_clear_security_flags(ulc_t *ulc, uint8_t *bd_addr, uint8_t delete_all_flag);
void bt_reset_peer_info(ulc_t *ulc, uint8_t peer_id);

#endif
