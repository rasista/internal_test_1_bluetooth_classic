/***************************************************************************//**
 * @file
 * @brief RAILtest application
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif
#include "sl_system_init.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  #include "sl_power_manager.h"
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
  #include "sl_system_kernel.h"
#else // SL_CATALOG_KERNEL_PRESENT
#include "sl_system_process_action.h"
#endif // SL_CATALOG_KERNEL_PRESENT

#include "sl_rail_util_init.h"
#include "response_print.h"
#include "sl_cli_handles.h"
#include "sl_cli_instances.h"
#include "sl_cli_storage_ram_instances.h"
#include "sl_event_handler.h"
#include "sl_iostream_handles.h"
#include "sl_iostream_init_eusart_instances.h"
#include "sl_iostream_init_instances.h"
#include "dmadrv.h"
#include "sl_host_to_lpw_interface.h"
#include "sl_rail_util_pti.h"
#include "rail.h"
#include "rail_seq.h"
#include "host_to_lpw_interface.h"
#include "lpw_to_host_mbox_interface.h"
#include "pkb_mgmt.h"
#include "sl_power_manager.h"
#include "stats.h"

extern bool stats_check;
extern shared_mem_t queue;
uint8_t test_mode_buffer[256];
extern pkb_pool_t pkb_pool_g[num_supported_pkb_chunk_type];

void app_init(void);
void app_process_action(void);

int main(void)
{
  // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
  // Note that if the kernel is present, processing task(s) will be created by
  // this call.
  sl_system_init();

  // Initialize the application. For example, create periodic timer(s) or
  // task(s) if the kernel is present.
  app_init();

#if defined(SL_CATALOG_KERNEL_PRESENT)
  // Start the kernel. Task(s) created in app_init() will start running.
  sl_system_kernel_start();
#else // SL_CATALOG_KERNEL_PRESENT
  while (1) {
    // Do not remove this call: Silicon Labs components process action routine
    // must be called from the super loop.
    sl_system_process_action();

    // Application process.

    app_process_action();

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#endif
  }
#endif // SL_CATALOG_KERNEL_PRESENT
}

void app_init(void)
{
}

void app_process_action(void)
{
  sl_btdm_fast_log_consumer_app_process_action();
}

static void print_llc_stats(stats_t *seq_stats)
{
  responsePrint("\n                     LLC:Tx Stats          \n",
                "schedule_id_tx:%u\n,"
                "schedule_tx:%u\n,"
                "tx_sent_llc:%u\n,"
                "tx_aborted_llc:%u\n",
                seq_stats->schedule_id_tx,
                seq_stats->schedule_tx,
                seq_stats->tx_sent,
                seq_stats->tx_aborted);

  responsePrint("\n                     LLC:Rx Stats          \n",
                "schedule_id_rx:%u\n,"
                "schedule_rx:%u\n,"
                "rx_packet_done_llc:%u\n,"
                "rx_packet_received_llc:%u\n,"
                "rx_packet_aborted_llc:%u\n,"
                "rx_timeout_llc:%u\n,"
                "id_pkt_rx_timeout:%u\n,"
                "hec_error:%u\n,"
                "id_pkt_rx:%u\n",
                seq_stats->schedule_id_rx,
                seq_stats->schedule_rx,
                seq_stats->rx_packet_done,
                seq_stats->rx_packet_received,
                seq_stats->rx_packet_aborted,
                seq_stats->rx_timeout,
                seq_stats->id_pkt_rx,
                seq_stats->hec_error,
                seq_stats->id_pkt_rx);
}

void sl_rail_util_on_event(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  if (events & RAIL_EVENT_USER_MBOX) {
    uint32_t msg = 0;
    RAIL_Status_t status = RAIL_USER_GetMboxMsg(railHandle, &msg);
    if (status != RAIL_STATUS_NO_ERROR) {
      responsePrintError("sl_rail_util_on_event", 0xA, "Could not get message %d", status);
      return;
    } else {
      if (stats_check) {
        stats_check = false;
        stats_t *seq_stats = (stats_t *)msg;
        print_llc_stats(seq_stats);
      } else {
        process_mbox_msg(&queue);
      }
    }
  }
  return;
}

#ifdef _SILICON_LABS_IP_PROJ_IS_LPWH74000
// Not 100% sure why this works on no 74000 builds.  It has to do with us forcing SL_POWER_MANAGER_DEBUG to 1 in
// rail_power_manager.c, but not having SL_POWER_MANAGER_DEBUG set to 1 in the fpga power_manager component.  Maybe.
#undef sli_power_manager_debug_log_em_requirement
void sli_power_manager_debug_log_em_requirement(sl_power_manager_em_t em, bool add, const char *name)
{
  (void) em;
  (void) add;
  (void) name;
}
#endif
