#ifndef __BTC_HCI_RX_PKT_HANDLERS_H__
#define __BTC_HCI_RX_PKT_HANDLERS_H__
#include "hci.h"
#include "btc.h"
#include "btc_hci.h"
#include "hci_cmd_set_defines_complete_event_autogen.h"
#include "ulc.h"

#define BD_ADDR_LEN 6
#define MAX_INQUIRY_RESPONSES 53
#define FHS_PKT_START_OFFSET 7 //FHS pkt starts after 2 bytes of Meta data and 3 bytes of header
#define FHS_EIR_BIT        BIT(10)

#define PACKET_TYPE_SHIFT 3
#define PACKET_TYPE_MASK 0x7F
#define TXID_SHIFT 7
#define TID_MASK 0x01
#define LMP_OPCODE_MASK 0x7F
#define LLID_MASK 0x03
#define META_DATA_LENGTH 4
#define LMP_PAYLOAD_HDR_LEN 1
#define GET_PKT_TYPE(pkb) (pkb->data[META_DATA_LENGTH] & PACKET_TYPE_MASK) >> PACKET_TYPE_SHIFT;
#ifdef SCHED_SIM
#define BTC_RX_PKT_HDR_LEN 2
#else
#define BTC_RX_PKT_HDR_LEN 3
#endif
#define LMP_PAYLOAD_START_OFFSET (META_DATA_LENGTH + BTC_RX_PKT_HDR_LEN + LMP_PAYLOAD_HDR_LEN)
#define LMP_LLID_OFFSET (META_DATA_LENGTH + BTC_RX_PKT_HDR_LEN)
#define GET_OPCODE(pkb) (pkb->data[LMP_PAYLOAD_START_OFFSET] & LMP_OPCODE_MASK)
#define GET_TID(pkb)    (pkb->data[LMP_PAYLOAD_START_OFFSET] >> TID_MASK) & TID_MASK
#define GET_EXT_OPCODE(pkb) (pkb->data[LMP_PAYLOAD_START_OFFSET + 1])
#define GET_LLID(pkb) ((pkb->data[LMP_LLID_OFFSET]) & LLID_MASK)

#define LPW_EVENT_TYPE_OFFSET(pkb) pkb->data[0]
#define GET_LPW_RX_NOTIFICATION_TYPE(pkb) pkb->data[META_DATA_LENGTH]

/*FHS PKT BYTE INFORMATION MACROS*/

/*AVAILABILITY OF LAP INFORMATION IN FHS PACKET */
#define LAP_START_BYTE 4
#define LAP_MID_BYTE_1 5
#define LAP_MID_BYTE_2 6
#define LAP_FINAL_BYTE 7

/*AVAILABILITY OF EIR INFORMATION IN FHS PACKET */
#define EIR_BYTE 7

/*AVAILABILITY OF PSR INFORMATION IN FHS PACKET */
#define PSR_BYTE 7

/*AVAILABILITY OF UAP AND NAP INFORMATION IN FHS PACKET */
#define UAP_BYTE 8
#define NAP_START_BYTE 9
#define NAP_END_BYTE 10

/*AVAILABILITY OF CLASS OF DEVICE INFORMATION IN FHS PACKET */
#define CLASS_OF_DEVICE_START_BYTE 11
#define CLASS_OF_DEVICE_MID_BYTE 12
#define CLASS_OF_DEVICE_END_BYTE 13

/* RX FHS Packet Extraction Macros */
#define RX_FHS_PKT_LAP(fhs_pkt) ((fhs_pkt[LAP_FINAL_BYTE] & 0x03) << 22 | (fhs_pkt[LAP_MID_BYTE_2] << 14) | ((fhs_pkt[LAP_MID_BYTE_1]) << 6) | ((fhs_pkt[LAP_START_BYTE] >> 2) & 0x3f))
#define RX_FHS_PKT_EIR(fhs_pkt) ((fhs_pkt[EIR_BYTE] >> 5) & 0x01)
#define RX_FHS_PKT_UAP(fhs_pkt) fhs_pkt[UAP_BYTE]
#define RX_FHS_PKT_NAP(fhs_pkt) ((uint16_t)((fhs_pkt[NAP_END_BYTE] << 8) | fhs_pkt[NAP_START_BYTE]))
#define RX_FHS_PKT_PSR(fhs_pkt) ((fhs_pkt[PSR_BYTE] >> 2) & 0x03)
#define RX_FHS_PKT_CLASS_OF_DEVICE(fhs_pkt) ((uint32_t)(fhs_pkt[CLASS_OF_DEVICE_START_BYTE] | fhs_pkt[CLASS_OF_DEVICE_MID_BYTE] << 8 | fhs_pkt[CLASS_OF_DEVICE_END_BYTE] << 16))
/* Extraction Macros for class of device */
#define RX_FHS_PKT_CLASS_OF_DEVICE_0(fhs_pkt) (RX_FHS_PKT_CLASS_OF_DEVICE(fhs_pkt) & 0xFF)
#define RX_FHS_PKT_CLASS_OF_DEVICE_1(fhs_pkt) ((RX_FHS_PKT_CLASS_OF_DEVICE(fhs_pkt) & 0xFF00) >> 8)
#define RX_FHS_PKT_CLASS_OF_DEVICE_2(fhs_pkt) ((RX_FHS_PKT_CLASS_OF_DEVICE(fhs_pkt) & 0xFF0000) >> 16)
/* ToDo calculate the below 3 Macros in the future*/
#define RX_FHS_PKT_RSSI 68
#define RX_FHS_PKT_EIR_LENGTH 17 // EIR length
// #define RX_FHS_PKT_EIR_BUFFER(fhs_pkt) fhs_pkt[FHS_PKT_EIR_BUFFER_OFFSET] // EIR buffer

#define LPW_RX_NOTIFICATION_TYPE_SIZE 1 
#define GET_LPW_RX_RSSI_SAMPLE_COUNT(pkb) pkb->data[META_DATA_LENGTH + LPW_RX_NOTIFICATION_TYPE_SIZE + BD_ADDR_LEN]   // gives the count value of RSSI samples in the packet
#define GET_LPW_RX_RSSI_INPUT_START_INDEX (META_DATA_LENGTH + LPW_RX_NOTIFICATION_TYPE_SIZE + BD_ADDR_LEN + 1)     // points to the first RSSI sample location in the packet
#define GET_LPW_EVENT_TYPE(ulc_ptr, pkb) (IS_TEST_MODE_ENABLED((ulc_ptr), (pkb)) ? TEST_MODE_HANDLER_INDEX : LPW_EVENT_TYPE_OFFSET(pkb))

void btc_process_rx_internal_event_packet_handler(void *ctx, pkb_t *pkb);
void btc_process_rx_on_air_event_packet_handler(void *ctx, pkb_t *pkb);
void btc_process_rx_fhs_packet_handler(void *ctx, pkb_t *pkb);
void lpw_rx_done_handler(void *ctx);
void btc_handle_received_lmp_packet(void *ctx, pkb_t *pkb);
void page_conn_done_from_lpw(void *ctx, pkb_t *pkb);
void page_scan_conn_done_from_lpw(void *ctx, pkb_t *pkb);
void acl_conn_done_from_lpw(void *ctx, pkb_t *pkb);
void inquiry_completion_indication(void *ctx);
#endif
