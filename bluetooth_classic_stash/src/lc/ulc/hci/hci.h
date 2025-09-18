#ifndef __HCI_H__
#define __HCI_H__

#include "debug.h"
#include "pkb_mgmt.h"
#include <stdint.h>
#include "btc_plt_deps.h"
#define INVALID_INPUT  1
#define HCI_MAX_PAYLOAD_SIZE 255
#define GET_HCI_PKT_TYPE(buf) ((buf)[0] & 0x0F)
#define GET_HCI_CMD_OPCODE(buf) ((buf)[1] | ((buf)[2] << 8))
#define GET_HCI_CMD_OGF(opcode) ((opcode) >> 10)
#define GET_HCI_CMD_OCF(opcode) ((opcode) & 0x03FF)
#define GET_HCI_CMD_PARAM_LEN(buf) ((buf)[3])
#define GET_HCI_OCF_VENDOR_ECHO_PARAMS(buf) ((buf) + 4)

#define GET_HCI_OCF_VENDOR_PAGE_SCAN_PORT_LOW(buf) (buf[4])
#define GET_HCI_OCF_VENDOR_PAGE_SCAN_PORT_HIGH(buf) (buf[5])

#define GET_HCI_OCF_VENDOR_PAGE_IP_0(buf) (buf[4])
#define GET_HCI_OCF_VENDOR_PAGE_IP_1(buf) (buf[5])
#define GET_HCI_OCF_VENDOR_PAGE_IP_2(buf) (buf[6])
#define GET_HCI_OCF_VENDOR_PAGE_IP_3(buf) (buf[7])
#define GET_HCI_OCF_VENDOR_PAGE_PORT_LOW(buf) (buf[8])
#define GET_HCI_OCF_VENDOR_PAGE_PORT_HIGH(buf) (buf[9])

#define GET_HCI_COMMAND_COMPLETE_RESPONE_PARAMS(buf) ((buf) + 6)
#define GET_HCI_EVENT_LEN(buf) ((buf)[2])
#define SET_HCI_CMD_PARAM_LEN(buf, len) ((buf)[2] = (len))
#define HCI_RESET_OPCODE 0x0C03

// Custom macros for SET_AFH_HOST_CHANNEL_CLASSIFICATION command parameters 
#define GET_SET_AFH_HOST_CHANNEL_CLASSIFICATION_MAP_END(buf)  buf[13]
#define AFH_RESERVED_BIT_MASK   BIT(7)
// to check if AFH channel map has reserved bit set
#define IS_AFH_CHANNEL_MAP_RESERVED_BIT_SET(pkb_data) \
    (GET_SET_AFH_HOST_CHANNEL_CLASSIFICATION_MAP_END(pkb_data) & AFH_RESERVED_BIT_MASK)

enum hci_ogf {
  HCI_OGF_LINK_CONTROL = 0x01,
  HCI_OGF_LINK_POLICY = 0x02,
  HCI_OGF_CONTROLLER_BASEBAND = 0x03,
  HCI_OGF_INFORMATIONAL_PARAMETERS = 0x04,
  HCI_OGF_STATUS_PARAMETERS = 0x05,
  HCI_OGF_TESTING_COMMANDS = 0x06,
  HCI_OGF_VENDOR_SPECIFIC = 0x3F
};
enum hci_ocf_vendor_specific {
  HCI_OCF_VENDOR_ECHO = 0X0001,
  HCI_OCF_VENDOR_PAGE_SCAN = 0X0002,
  HCI_OCF_VENDOR_PAGE_SCAN_CANCEL = 0X0003,
  HCI_OCF_VENDOR_PAGE = 0X0004,
  HCI_OCF_VENDOR_PAGE_CANCEL = 0X0005,
};
enum hci_event_code {
  HCI_EVENT_CODE_RECVD_ACL_DATA = 0x7F // TODO This is for testing purposes only
};
enum hci_packet_type {
  HCI_PACKET_TYPE_IGNORE = 0, /* 0 used to trigger wakeup interrupt */
  HCI_PACKET_TYPE_COMMAND = 1,
  HCI_PACKET_TYPE_ACL_DATA = 2,
  HCI_PACKET_TYPE_SYNC_DATA = 3,
  HCI_PACKET_TYPE_EVENT = 4,
  HCI_PACKET_TYPE_ISO_DATA = 5
};

enum hci_packet_state {
  HCI_PACKET_STATE_READ_PACKET_TYPE = 0,
  HCI_PACKET_STATE_READ_HEADER = 1,
  HCI_PACKET_STATE_READ_PAYLOAD = 2
};

typedef uint8_t *hci_packet_t;
typedef void (*hci_handler_t)(void *ctx, pkb_t *pkb_s);

void call_hci_handler(void *ctx, pkb_t *pkb_s);
hci_handler_t *get_ocf_handlers_lut(uint8_t ogf);
uint16_t size_of_recieved_group_lut(uint8_t ogf);
uint16_t size_of_hci_ogf_lut();
void hci_ocf_vendor_echo_handler(void *ctx, pkb_t *pkb);
hci_handler_t get_hci_cmd_handler(void *ctx, uint16_t opcode);
pkb_t *prepare_hci_event(uint8_t event_no, uint8_t length);
pkb_t *prepare_hci_command_status_event(uint16_t opcode, uint8_t status);
pkb_t *prepare_hci_command_complete_event(uint16_t opcode, uint8_t length);
void send_hci_response_packet(pkb_t *pkb);

void vendor_send_handler(void *ctx);
#endif // __HCI_H__
