//! File implements HCI handlers and other HCi related functions

#include "hci.h"
#include "btc.h"
#include "btc_hci.h"
#include "hci_cmd_set_defines_complete_event_autogen.h"
#include "ulc.h"
#include "usched_vendor_specific.h"
#include "btc_lmp_test_mode.h"


pkb_t *prepare_hci_event(uint8_t event_no, uint8_t length)
{
  pkb_t *pkb =
    pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  pkb->data[0] = HCI_PACKET_TYPE_EVENT;
  pkb->data[1] = event_no;
  pkb->data[2] = length;
  return pkb;
}

//! function to allocate a packet from the pool and prepare hci_command_complete
//! event header
pkb_t *prepare_hci_command_complete_event(uint16_t opcode, uint8_t length)
{
  pkb_t *pkb =
    pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  pkb->data[0] = HCI_PACKET_TYPE_EVENT;
  pkb->data[1] = HCI_CMD_COMPLETE_EVENT;
  pkb->data[2] = length + 3; // parameter total length
  pkb->data[3] = 0x01;       // number of commands
  pkb->data[4] = opcode & 0xFF;
  pkb->data[5] = opcode >> 8;
  return pkb;
}

//! function to allocate a packet from the pool and prepare hci_command_status
//! event header
pkb_t *prepare_hci_command_status_event(uint16_t opcode, uint8_t status)
{
  pkb_t *pkb =
    pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  pkb->data[0] = HCI_PACKET_TYPE_EVENT;
  pkb->data[1] = HCI_CMD_STATUS_EVENT;
  pkb->data[2] = 0x04; // parameter total length
  pkb->data[3] = status;
  pkb->data[4] = 0x01;
  pkb->data[5] = opcode & 0xFF;
  pkb->data[6] = opcode >> 8;
  return pkb;
}

void send_hci_response_packet(pkb_t *pkb)
{
  enqueue(&ulc.hci_event_pending_q, &pkb->queue_node);
  btc_event_loop_set_event(&ulc.event_loop,
                           BTC_EVENT_HCI_EVENT_TO_HOST_PENDING);
  btc_plt_remove_from_pend(&ulc.event_loop_pend);
}

hci_handler_t get_hci_cmd_handler(void *ctx, uint16_t opcode)
{
  (void)ctx;
  uint8_t ogf = GET_HCI_CMD_OGF(opcode);
  BTC_PRINT("%d\n", ogf);
  uint16_t ocf = GET_HCI_CMD_OCF(opcode);
  BTC_PRINT("%d\n", ocf);
  // ogf == HCI_OGF_VENDOR_SPECIFIC || //TODO this check
  // statement to be included after further implementation
  if ((ogf < size_of_hci_ogf_lut() / sizeof(hci_handler_t))) {
    hci_handler_t *ocf_handlers_lut = get_ocf_handlers_lut(ogf);
    if (ocf < size_of_recieved_group_lut(ogf)) {
      return ocf_handlers_lut[ocf];
    } else {
      BTC_ASSERT(INVALID_INPUT);
      BTC_PRINT("Invalid OCF\n");
    }
  } else {
    BTC_ASSERT(INVALID_INPUT);
    BTC_PRINT("Invalid OGF");
  }
  return NULL;
}

void call_hci_handler(void *ctx, pkb_t *pkb)
{
  uint8_t hci_pkt_type = GET_HCI_PKT_TYPE(pkb->data);
  // Check if test mode is blocking host packets
  ulc_t *ulc = (ulc_t *)ctx;
  if (IS_TEST_MODE_BLOCKING_HOST_PKTS(ulc)) {
    // Only allow HCI Reset command when test mode is blocking
    uint16_t opcode = (uint16_t)GET_HCI_CMD_OPCODE(pkb->data); 
    if (opcode != HCI_RESET_OPCODE) {
      // Send command disallowed status for all other commands
      pkb_t *response = prepare_hci_command_status_event(opcode, COMMAND_DISALLOWED);
      send_hci_response_packet(response);
      pkb_free(pkb, &btc.pkb_pool[pkb->pkb_chunk_type]);
      return;
    }
  }
  switch (hci_pkt_type) {
    case HCI_PACKET_TYPE_COMMAND:
      BTC_PRINT("HCI Command\n");
      hci_handler_t handler = get_hci_cmd_handler(ctx, (uint16_t)GET_HCI_CMD_OPCODE(pkb->data));
      if (handler != NULL) {
        handler(ctx, pkb);
      } else {
        pkb_t *response_to_unsupported_cmd = prepare_hci_command_status_event(
          (uint16_t)GET_HCI_CMD_OPCODE(pkb->data), UNKNOWN_HCI_COMMAND);
        send_hci_response_packet(response_to_unsupported_cmd);
      }
      pkb_free(pkb, &btc.pkb_pool[pkb->pkb_chunk_type]); // freeing the received
                                                         // HCI command packet
      break;
    case HCI_PACKET_TYPE_ACL_DATA:
      // TODO
      return;
    case HCI_PACKET_TYPE_SYNC_DATA:
      // TODO
      return;
    default:
      return;
  }
}
