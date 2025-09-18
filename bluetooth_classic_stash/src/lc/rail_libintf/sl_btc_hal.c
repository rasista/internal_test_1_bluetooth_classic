#include <string.h>
#include <stdio.h>
#include "sl_btc_hal.h"
#include "data_path.h"

void btc_sequencer_user_init()
{
  btc_rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
  RAIL_Status_t status = sl_rail_btc_init(btc_rail_handle);
  uint32_t rxFifoSize = RX_FIFO_SIZE;

  if (status != RAIL_STATUS_NO_ERROR) {
    // responsePrintError("", status, "Unexpected error in sl_rail_btc_init()");
    //error logs will be added and responseprinterror will be removed
  }

  status = RAIL_ConfigEvents(btc_rail_handle, RAIL_EVENTS_ALL, RAIL_EVENT_USER_MBOX | RAIL_EVENT_TX_PACKET_SENT | RAIL_EVENT_RX_FRAME_ERROR | RAIL_EVENT_RX_PACKET_ABORTED);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError("", 0xA, "unexpected error %d during RAIL_ConfigEvents", status);
    return;
  }

  status = RAIL_ConfigRxOptions(btc_rail_handle, RAIL_RX_OPTIONS_ALL, RAIL_RX_OPTION_IGNORE_CRC_ERRORS);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError("", 0xA, "unexpected error %d during RAIL_ConfigRxOptions", status);
    return;
  }

  //! Setting TxPower
  RAIL_TxPowerConfig_t powerConfig = {
    .mode = RAIL_TX_POWER_MODE_2P4GIG_HP,
    .voltage = 3300,
    .rampTime = 5,
  };
  int ret = RAIL_ConfigTxPower(btc_rail_handle, &powerConfig);

  if (!ret) {
    ret = RAIL_SetTxPowerDbm(btc_rail_handle, 0);
  }

  GPIO->P[0].MODEL = 0x4 << (5 * 4U) | 0x4 << (6 * 4U);   //for initialization
  status = RAIL_LoadUserSeqApp(btc_rail_handle,
                               &riscvseq_user_btc_info);
  if (status == RAIL_STATUS_NO_ERROR) {
    // responsePrint("RAIL_LoadUserSeqApp", "Success:True");
    //error logs will be added and responseprinterror will be removed
  } else {
    // responsePrintError("RAIL_LoadUserSeqApp", status, "Unexpected error in RAIL_LoadUserSeqApp()");
    //error logs will be added and responseprinterror will be removed
  }

  RAIL_StateTransitions_t transitions = {
    .success = RAIL_RF_STATE_IDLE,
    .error = RAIL_RF_STATE_IDLE,
  };
  RAIL_SetTxTransitions(btc_rail_handle, &transitions);
  status = RAIL_SetRxFifo(btc_rail_handle, &rxFifo[0], &rxFifoSize);
  if (status == RAIL_STATUS_INVALID_STATE) {
    // Allow failures due to multiprotocol
    // responsePrintError("RAIL_SetRxFifo", status, "Unexpected error in RAIL_SetRxFifo");
    //error logs will be added and responseprinterror will be removed

    return;
  }

  Ecode_t dmaError = DMADRV_Init();
  if ((dmaError == ECODE_EMDRV_DMADRV_ALREADY_INITIALIZED)
      || (dmaError == ECODE_EMDRV_DMADRV_OK)) {
    uint32_t dmaChannel;
    dmaError = DMADRV_AllocateChannel((unsigned int *)&dmaChannel, NULL);
    if (dmaError == ECODE_EMDRV_DMADRV_OK) {
      status = RAIL_USER_InitCp(btc_rail_handle, dmaChannel);
      if (status == RAIL_STATUS_NO_ERROR) {
        responsePrint("RAIL_USER_InitCp", "Success:True");
      } else {
        // responsePrintError("RAIL_USER_InitCp", status, "Unexpected error in RAIL_USER_InitCp()");
        //error logs will be added and responseprinterror will be removed
      }
    } else {
      // responsePrintError("DMADRV_AllocateChannel", status, "Unexpected error in DMADRV_AllocateChannel()");
      //error logs will be added and responseprinterror will be removed
    }
  }
}

void fifo_manager_init()
{
  init_fifo_manager();
}
