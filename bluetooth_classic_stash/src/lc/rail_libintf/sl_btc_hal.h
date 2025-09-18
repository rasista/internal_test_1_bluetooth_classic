#ifndef SL_BTC_HAL_H_
#define SL_BTC_HAL_H_

#include "dmadrv.h"
#include "rail.h"
#include "rail_seq.h"
#include "rail_seq_user_app_info.h"
#include "sl_rail_util_init.h"
#include "sl_event_handler.h"

extern const RAIL_SeqUserAppInfo_t riscvseq_user_btc_info;

extern RAIL_Handle_t btc_rail_handle;

#define RX_FIFO_SIZE 2056 // Size of the RX FIFO to hold atleast two BTC data packets with maximum size of 1021 bytes payload
extern uint32_t rxFifo[RX_FIFO_SIZE / 4]; // to make address 32 bit aligned
void btc_sequencer_user_init();
void fifo_manager_init();

#endif
