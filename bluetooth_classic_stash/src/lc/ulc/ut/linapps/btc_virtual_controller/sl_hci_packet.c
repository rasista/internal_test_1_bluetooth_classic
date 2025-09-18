#include "sl_hci_packet.h"
#include "uart.h"
#include "debug.h"
#include "ulc.h"
#include "btc.h"
#include "time.h"
#include "btc_plt_deps.h"
btc_pend_t hci_write_pend;

#define RX_BUFFER_LEN 1024
static uint8_t hci_rx_buffer[RX_BUFFER_LEN];
/* buffer pointer for transferring bytes to hci_rx_buffer */
static uint8_t *buf_byte_ptr;
static enum hci_packet_state state;
static uint16_t byte_consumed;
static uint16_t total_bytes_read;
static uint16_t total_bytes_to_read;

static void sl_hci_read_reset(void)
{
  memset(hci_rx_buffer, 0, RX_BUFFER_LEN);
  state = HCI_PACKET_STATE_READ_PACKET_TYPE;
  buf_byte_ptr = hci_rx_buffer;
  total_bytes_read = 0;
  total_bytes_to_read = 0;
}

// static void reception_failure(void)
// {
//   sl_hci_read_reset();
//   return;
// }

//! function returns minimum value between two integers
static inline uint32_t min(uint32_t a, uint32_t b)
{
  return a < b ? a : b;
}

void hci_uart_receive(uint8_t *data, uint32_t len, void *ctx)
{
  BTC_HEX_DUMP("HCI RX", data, len);
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  //! allocate a packet from mid size pool
  pkb_t *pkb             = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  uint32_t bytes_to_copy = len;
  uint32_t bytes_copied  = 0;

  //! start filling the pkb with data. if length exceeds the pkb size, create a scatter list
  while (bytes_to_copy > 0) {
    uint32_t bytes_to_copy_in_this_pkb =
      min(pkb_get_max_user_data_len(&btc.pkb_pool[pkb_chunk_type_medium]), bytes_to_copy);
    memcpy(pkb->data + bytes_copied, data + bytes_copied, bytes_to_copy_in_this_pkb);
    bytes_copied += bytes_to_copy_in_this_pkb;
    bytes_to_copy -= bytes_to_copy_in_this_pkb;
    if (bytes_to_copy > 0) {
      pkb_t *scatter_pkb             = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
      pkb->scatter_node.next         = &scatter_pkb->scatter_node;
      scatter_pkb->scatter_node.next = NULL;
      pkb                            = scatter_pkb;
    }
  }
  pkb->pkt_len = len;
  //! post packet to ulc_ptr->host_q
  enqueue(&ulc_ptr->hci_q, &pkb->queue_node);
  //! set host_tx_pending event in ulc_ptr->event_loop
  btc_event_loop_set_event(&ulc_ptr->event_loop, BTC_EVENT_HCI_FROM_HOST_PENDING);
  //! post to ulc_ptr->event_loop_pend
  btc_plt_remove_from_pend(&ulc_ptr->event_loop_pend);
}

//! Function to process the received data over UART and parse it as an HCI packet
//! Function makes a blocking call to uart_read passing hci_rx_buffer as the buffer
//! function returns the length of the data read.
//! the number of bytes read needs to be parsed as an HCI packet.
//! The HCI packet comprise 1 byte of header which contains the packet type
//! if packet type is command or event, the packet will have 3 bytes of header
//! if packet type is ACL data, the packet will have 4 bytes of header
//! the header is followed by the payload
//! the payload length is present in the header

void * hci_read(void *arg)
{
  uint32_t len = 0;
  while (1) {
    uint16_t force_uart_read = false;
    uint16_t bytes_read = 0;
    bytes_read = uart_read(hci_rx_buffer);
    total_bytes_read += bytes_read;
    buf_byte_ptr += bytes_read;

    while (force_uart_read == false) {
      switch (state) {
        case HCI_PACKET_STATE_READ_PACKET_TYPE:
          if (total_bytes_read >= 1) {
            state = HCI_PACKET_STATE_READ_HEADER;
            byte_consumed = 1;
          }
          break;
        case HCI_PACKET_STATE_READ_HEADER:
          if (GET_HCI_PKT_TYPE(hci_rx_buffer) == HCI_PACKET_TYPE_ACL_DATA) {
            if (total_bytes_read >= 5) {
              state = HCI_PACKET_STATE_READ_PAYLOAD;
              //! calculate the total bytes to read
              total_bytes_to_read = (hci_rx_buffer[3] | (hci_rx_buffer[4] << 8)) + HCI_ACL_DATA_HEADER_SIZE + 1;//1 for packet type, 2 for handle, 2 for length            } else {
            } else {
              BTC_PRINT("Insufficient bytes read, force read from UART\n");
              //! insufficient bytes read, force read from UART
              force_uart_read = true;
              continue;
            }
          } else if (GET_HCI_PKT_TYPE(hci_rx_buffer) == HCI_PACKET_TYPE_COMMAND || GET_HCI_PKT_TYPE(hci_rx_buffer) == HCI_PACKET_TYPE_EVENT) {
            if (total_bytes_read >= 4) {
              state = HCI_PACKET_STATE_READ_PAYLOAD;
              //! calculate the total bytes to read
              total_bytes_to_read = hci_rx_buffer[3] + HCI_COMMAND_HEADER_SIZE + 1;//1 for packet type, 2 for opcode, 1 for length
            } else {
              BTC_PRINT("Insufficient bytes read, force read from UART\n");
              //! insufficient bytes read, force read from UART
              force_uart_read = true;
              continue;
            }
          } else {
            BTC_ASSERT(INCORRECT_HCI_PACKET_TYPE);
          }
          break;
        case HCI_PACKET_STATE_READ_PAYLOAD:
          if (total_bytes_read == total_bytes_to_read) {
            len = total_bytes_read;
            for (int i = 0; i < len; i++) {
              BTC_PRINT("-0x%02x -", hci_rx_buffer[i]);
            }
            hci_uart_receive(hci_rx_buffer, len, arg);
            sl_hci_read_reset();
          } else {
            BTC_PRINT("Insufficient bytes read, force read from UART\n");
            BTC_PRINT("total_bytes_read: %d\n", total_bytes_read);
            BTC_PRINT("total_bytes_to_read: %d\n", total_bytes_to_read);
            //! insufficient bytes read, force read from UART
          }
          force_uart_read = true;
          break;
      }
    }
  }
}

void *hci_write(void *arg)
{
  if (btc_plt_create_pend(&hci_write_pend) != 0)
  {
    BTC_PRINT(" hci_write pending mechanism init Failed");
    return NULL;
  }

  while (1) {
    btc_plt_add_to_pend(&hci_write_pend);
    //! for each packet in ulc.hci_event_pending_q, send the packet over UART
    printf("queue_size %d \n", ulc.hci_event_pending_q.size);
    while (ulc.hci_event_pending_q.size > 0) {
      pkb_t *pkb = (pkb_t *)dequeue(&ulc.hci_event_pending_q);
      BTC_PRINT("Sending packet to host\n");
      BTC_HEX_DUMP("uart_write", pkb->data, GET_HCI_EVENT_LEN(pkb->data) + 3);
      time_t current_time = time(NULL);
      printf("System time: %s", ctime(&current_time));
      if (uart_write(pkb->data, GET_HCI_EVENT_LEN(pkb->data) + 3) == GET_HCI_EVENT_LEN(pkb->data) + 3)
      {
        BTC_PRINT("UART TX DONE \n");
      }
      else {
        BTC_PRINT("UART TX FAILED \n");
      }
      enqueue(&ulc.hci_event_sent_q, &pkb->queue_node);
    }
    btc_event_loop_set_event(&ulc.event_loop, BTC_EVENT_HCI_EVENT_TO_HOST_SENT);
    btc_plt_remove_from_pend(&ulc.event_loop_pend);
  }
}

void schedule_hci_events_to_host()
{
  BTC_PRINT("Calling remove hci_write_pend\n");
  btc_plt_remove_from_pend(&hci_write_pend);
}
