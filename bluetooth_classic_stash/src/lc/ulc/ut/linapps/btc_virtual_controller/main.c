#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "uart.h"
#include "thread.h"
#include "sl_hci_packet.h"
#include "ulc.h"
#include "btc.h"
#include "virtual_link.h"
#include "usched_vendor_specific.h"
#include "btc_dev.h"
#include "usched_hci_cmd_event_handlers.h"
#include "tx_rx_handling_for_virtual_link.h"
#include "signal.h"

extern btc_t btc;
uint8_t medium_pkt_heap[48][pkb_chunk_size_medium];
uint8_t short_pkt_heap[8][pkb_chunk_size_small];
acl_peer_info_t glbl_acl_peer_info[MAX_NUM_ACL_CONNS];

btc_pend_t threadLock; // lock to protect the queue
pthread_t hci_uart_receive_thread;
pthread_t hci_uart_send_thread;
pthread_t ulc_thread;
pthread_t virtual_link_thread;

//register signal
void signal_handler(int signum)
{
  printf("Caught signal %d\n", signum);
  uart_close();
  // platform_deinit();
  exit(0);
}


/**
 * @brief Function to register termination signals to this process
 *
*/
void registering_signals()
{
  for (int i = 1; i < 32; i++) {
    signal(i, signal_handler);
  }
  return;
}

int main(int argc, char *argv[])
{
  //! thread for hci read over uart


  //pthread_t virtual_link_data_transfer_thread;

  // Check if UART port was passed as a command-line argument
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <UART port>\n", argv[0]);
    return -1;
  }
  debug_init();
  registering_signals();
  uart_open(argv[1], 115200);

  btc.ulc = &ulc;
  for (int i = 0; i < MAX_NUM_ACL_CONNS; i++) {
    ulc.btc_dev_info.acl_peer_info[i] = &glbl_acl_peer_info[i];
  }

  init_btc_pool(&btc.pkb_pool[pkb_chunk_type_small], pkb_chunk_size_small, pkb_chunk_type_small, 8, 3);
  init_btc_pool(&btc.pkb_pool[pkb_chunk_type_medium], pkb_chunk_size_medium, pkb_chunk_type_medium, 48, 5);
  for (int i = 0; i < btc.pkb_pool[pkb_chunk_type_small].pkb_num_max; i++) {
    add_buffer_to_btc_pool(&btc.pkb_pool[pkb_chunk_type_small], (pkb_t *)short_pkt_heap[i]);
  }
  for (int i = 0; i < btc.pkb_pool[pkb_chunk_type_medium].pkb_num_max; i++) {
    add_buffer_to_btc_pool(&btc.pkb_pool[pkb_chunk_type_medium], (pkb_t *)medium_pkt_heap[i]);
  }
  ulc.hci_event_pending_q.plt_lock.acquire_lock = btc_plt_acquire_lock;
  ulc.hci_event_pending_q.plt_lock.release_lock = btc_plt_release_lock;
  ulc.hci_event_sent_q.plt_lock.acquire_lock = btc_plt_acquire_lock;
  ulc.hci_event_sent_q.plt_lock.release_lock = btc_plt_release_lock;
  ulc.hci_q.plt_lock.acquire_lock = btc_plt_acquire_lock;
  ulc.hci_q.plt_lock.release_lock = btc_plt_release_lock;
  ulc.lpw_rx_done_q.plt_lock.acquire_lock = btc_plt_acquire_lock;
  ulc.lpw_rx_done_q.plt_lock.release_lock = btc_plt_release_lock;
  ulc.tx_send_q.plt_lock.acquire_lock = btc_plt_acquire_lock;
  ulc.tx_send_q.plt_lock.release_lock = btc_plt_release_lock;
  for (int i = 0; i < MAX_NUM_ACL_CONNS; i++) {
    ulc.btc_dev_info.acl_peer_info[i]->tx_aclc_q.plt_lock.acquire_lock = btc_plt_acquire_lock;
    ulc.btc_dev_info.acl_peer_info[i]->tx_aclc_q.plt_lock.release_lock = btc_plt_release_lock;
  }
  ulc_init(&ulc);
  virtual_link_register_receive_cb(&receive_done_callback);
  btc_plt_create_pend(&threadLock);
  thread_create(&ulc_thread, ulc_loop, &ulc);
  thread_create(&hci_uart_receive_thread, hci_read, &ulc);
  thread_create(&hci_uart_send_thread, hci_write, &ulc);
  thread_create(&virtual_link_thread, virtual_link_loop, &ulc);
  btc_plt_add_to_pend(&threadLock);
}

void usched_api_start_vendor_page_scan(uint16_t port)
{
  virtual_link_api_listen_connection(port);
  BTC_PRINT("usched_api_start_vendor_page_scan\n");
}
void usched_api_cancel_vendor_page_scan()
{
  virtual_link_api_cancel_listen();
  BTC_PRINT("usched_api_cancel_vendor_page_scan\n");
}
void usched_api_start_vendor_page(uint8_t ip_0, uint8_t ip_1, uint8_t ip_2, uint8_t ip_3, uint16_t port)
{
  virtual_link_api_connect(ip_0, ip_1, ip_2, ip_3, port);
  BTC_PRINT("usched_api_start_vendor_page\n");
}
void usched_api_cancel_vendor_page()
{
  virtual_link_api_cancel_connect();
  BTC_PRINT("usched_api_cancel_vendor_page\n");
}
void ulc_send_acl_tx(uint8_t *data_pkt, uint8_t data_pkt_len)
{
  virtual_link_api_send(data_pkt, data_pkt_len);
  BTC_PRINT("ulc_send_acl_tx\n");
}

void usched_api_start_inquiry_periodic_inq()
{
  //virtual_link_api_connect(ip_0, ip_1, ip_2, ip_3, port);
  BTC_PRINT("usched_api_start_inquiry_periodic_inq\n");
}

void usched_api_start_inquiry_scan()
{
  BTC_PRINT("usched_api_start_inquiry_scan\n");
  uint8_t fhs_data[25] = { 0x00, 0x00, 0x00, 0x00, 0x10, 0x9C, 0x01, 0x4D, 0x4D, 0x2C, 0xC5, 0x47, 0x5D, 0xF9, 0x95, 0xC3, 0x72, 0x68, 0x3C, 0x04, 0x08, 0x90, 0xFF, 0x29, 0x01 };
  usched_tx_pkt_handler(fhs_data, 25);
}

void usched_tx_pkt_handler(uint8_t *data, uint8_t length)
{
  int status = 0;
  while (status != length)
  {
    status = virtual_link_api_send(data, length);
    if (status != length) {
      BTC_PRINT("usched_tx_pkt_handler: Error sending data\n");
      usleep(400000); // Sleep for a short duration before retrying
    } else {
      BTC_PRINT("usched_tx_pkt_handler: Data sent successfully\n");
      break;
    }
  }
}

void usched_api_cancel_inquiry_periodic_inq()
{
  BTC_PRINT("usched_api_cancel_inquiry_periodic_inq\n");
}

void usched_api_stop_periodic_inquiry()
{
  BTC_PRINT("usched_api_cancel_PERIODIC_inquiry\n");
}

void killFirmware()
{
  btc_plt_remove_from_pend(&threadLock);
}