/*
   File Header:
   File Name: ulc.h
   File Description: This file contains the starting point for ULC
   implementation.
 */
#ifndef __ULC_H_
#define __ULC_H_
#include "btc_dev.h"
#include "debug.h"
#include "event_loop.h"
#include "host_interface.h"
#include "pkb_mgmt.h"
#include <stddef.h>
#include <stdint.h>
#include "usched_common_role.h"

/*Super structure to encompass all possible ULC parameters*/
typedef struct ulc_s {
  //! master structure for event loop implementation
  event_loop_t event_loop;
  //! OS semaphore to block the event loop till some valid event is posted
  btc_pend_t event_loop_pend;
  //! queue to store the packets from the host interface, packets are posted by
  //! the transport implementation and picked in hci_q_pending_handler
  queue_t hci_q;
  //! queue to store the packets generated across the controller, packets are
  //! picked by the transport implementation and sent to the host. The transport
  //! implementation itself is trggered by the hci_event_handler.
  queue_t hci_event_pending_q;
  //! queue to store the hci responses sent to the host interface and ready to
  //! be freed,
  queue_t hci_event_sent_q;
  // queue to store the packets received from the OTA
  queue_t lpw_rx_done_q;
  // queue to store the packets to be sent OTA
  queue_t tx_send_q;
  // Shared memory queue to send commands to message box
  shared_mem_t tx_mbox_queue;
  //! a common structure for all device related information
  btc_dev_info_t btc_dev_info;
#ifdef SCHED_SIM
  //! command complete handler for virtual link
  void (*cmd_complete_handler)(void *ctx, uint8_t status);
#endif
  // Scheduler info structure
  uscheduler_info_t scheduler_info;
} ulc_t;

extern ulc_t ulc;
// Function Name: ulc_init
// Description: This function is used to initialize the ULC module.
// Parameters: None
// Return: None
void *ulc_init(void *ctx);
void *ulc_loop(void *ctx);
void  btc_hci_init(void *ctx);
void  btc_lmp_init(ulc_t *ulc);
#endif // __ULC_H_
