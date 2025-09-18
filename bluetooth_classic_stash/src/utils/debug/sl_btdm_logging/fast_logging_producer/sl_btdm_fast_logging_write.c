//! file provides fast logging APIs for placing the timestamps, event IDs and upto 3 arguments in the log buffer
//! individual APIs are created for no, 1, 2 or 3 arguments
//! each API should check for ring buffer overflow and return immediately if the buffer is full
//! if full, a flag should be set to indicate that the buffer is full

#include "sl_btdm_fast_logging.h"
#ifndef LINUX_UTILS
#include "em_device.h"
#else
#include "stdio.h"
#endif

//! API to log an event with no arguments
void sl_btdm_fast_log_no_args(uint32_t log_sign)
{
    sl_btdm_fast_log_3_args(log_sign, 0, 0, 0);
}

//! API to log an event with 1 argument
void sl_btdm_fast_log_1_args(uint32_t log_sign, uint32_t arg1)
{
    sl_btdm_fast_log_3_args(log_sign, arg1, 0, 0);
}
//! API to log an event with 2 arguments
void sl_btdm_fast_log_2_args(uint32_t log_sign , uint32_t arg1, uint32_t arg2)
{
    sl_btdm_fast_log_3_args(log_sign, arg1, arg2, 0);
}

#ifndef LINUX_UTILS
typedef uint64_t ProtocolTimerTicks_t;

static ProtocolTimerTicks_t PROTIMER_GetPTicks(void)
{
  // Reading LPRECNT loads the other 2 latched registers.
  (void) PROTIMER->LPRECNTSEQ;
  // A 64-bit read, (LWRAPCNTSEQ << 32U) | LBASECNTSEQ
  return *((volatile ProtocolTimerTicks_t *)&PROTIMER->LBASECNTSEQ);
}
#endif

void sl_btdm_fast_log_3_args(uint32_t log_sign, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
#ifndef LINUX_UTILS
    //GPIO->P_SET[0].DOUT = 0x20;
#else
    printf("write_log_entry start: wr_offset = %d, rd_offset = %d\n", 
        sl_btdm_logging_meta_data->wr_offset,
        sl_btdm_logging_meta_data->rd_offset);
#endif
    uint32_t wr_offset = sl_btdm_logging_meta_data->wr_offset & sl_btdm_logging_meta_data->max_offset_mask;
    //SL_BTDM_LOGGING_ENTER_CRITICAL();
    sl_btdm_fast_logging_buffer[wr_offset].log_sign = log_sign;
#ifndef LINUX_UTILS
    sl_btdm_fast_logging_buffer[wr_offset].log_tsf = (uint32_t)PROTIMER_GetPTicks();
#else
    sl_btdm_fast_logging_buffer[wr_offset].log_tsf = SL_BTDM_LOGGING_GET_TIMESTAMP();
#endif
    sl_btdm_fast_logging_buffer[wr_offset].arg1 = arg1;
    sl_btdm_fast_logging_buffer[wr_offset].arg2 = arg2;
    sl_btdm_fast_logging_buffer[wr_offset].arg3 = arg3;
    sl_btdm_logging_meta_data->wr_offset++;
    //SL_BTDM_LOGGING_EXIT_CRITICAL();
#ifndef LINUX_UTILS
    //GPIO->P_CLR[0].DOUT = 0x20;
#else
    printf("write_log_entry end: wr_offset = %d, rd_offset = %d\n", 
        sl_btdm_logging_meta_data->wr_offset,
        sl_btdm_logging_meta_data->rd_offset);
#endif
}