/***************************************************************************//**
 * @file
 * @brief Sleeptimer examples functions
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

#include <stdio.h>
#include <string.h>
#include "sl_sleeptimer.h"
#include "sl_btdm_logging.h"
#include "btc_riscvseq_address_map.h"
#include "em_device.h"
#include "SEGGER_RTT.h"

#define TIMEOUT_MS 1
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static sl_sleeptimer_timer_handle_t periodic_timer;

/*******************************************************************************
 ************************   LOCAL FUNCTIONS ************************************
 ******************************************************************************/

// Periodic timer callback
static void on_periodic_timeout(sl_sleeptimer_timer_handle_t *handle,
                                void *data)
{
  (void)&handle;
  (void)&data;
}


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
sl_btdm_log_db_t sl_btdm_log_db;
uint32_t rtt_buffer[2048];

void plt_enter_critical()
{
}

void plt_exit_critical()
{
}
static bool seq_init_done = false;
void sl_btdm_fast_log_consumer_app_init(void)
{
  seq_init_done = true;
  // Initialize RTT
  SEGGER_RTT_Init();

  // Configure the up buffer
  SEGGER_RTT_ConfigUpBuffer(5, "BTDM_LOG_BUFFER", rtt_buffer, sizeof(rtt_buffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);

  sl_btdm_fast_logging_init(BTC_RISCVSEQ_FAST_LOG_ENTRY, BTC_RISCVSEQ_LOGGING_META_DATA, 32);
  // Create timer for waking up the system periodically.
  sl_sleeptimer_start_periodic_timer_ms(&periodic_timer,
                                        TIMEOUT_MS,
                                        on_periodic_timeout, NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);

}

void sl_btdm_fast_log_consumer_app_process_action()
{
  if (!seq_init_done) {
    return;
  }
  sl_btdm_fast_logging_backend();
}