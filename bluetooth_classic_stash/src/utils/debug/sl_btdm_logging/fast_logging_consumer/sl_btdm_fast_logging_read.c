#include "sl_btdm_logging.h"
#include <string.h>
#ifndef LINUX
#include "em_device.h"
#else
#include <stdio.h>
#endif
extern sl_btdm_log_db_t sl_btdm_log_db;


uint32_t handle_lost_signals()
{
        uint32_t wr_offset = sl_btdm_logging_meta_data->wr_offset;
        uint32_t rd_offset = sl_btdm_logging_meta_data->rd_offset;
        uint32_t signal_lost_count = 0;
        if (wr_offset > rd_offset)
        {
            uint32_t log_count = wr_offset - rd_offset;
            if ((log_count > (sl_btdm_logging_meta_data->buffer_size - 1)))
            {
                uint32_t valid_log_count = sl_btdm_logging_meta_data->buffer_size - 1;
                signal_lost_count = log_count - valid_log_count;
                sl_btdm_logging_meta_data->rd_offset = wr_offset - valid_log_count;
            }
        }
        else if (wr_offset < rd_offset)
        {
            uint32_t log_count = wr_offset + sl_btdm_logging_meta_data->buffer_size - rd_offset;
            if (log_count > (sl_btdm_logging_meta_data->buffer_size - 1))
            {
                uint32_t valid_log_count = sl_btdm_logging_meta_data->buffer_size - 1;
                signal_lost_count = log_count - valid_log_count;
                sl_btdm_logging_meta_data->rd_offset = wr_offset - valid_log_count;
            }
        }
        return signal_lost_count;

}
//! function to read the fast loggin buffer
//! function assumes that sl_btdm_log_info is present at the read offset
//! function starts preparing the sl_btdm_log_flags accordingly
//! fincally the function increments the read pointer to point to the next log
//! if the read pointer reaches buffer full offset, it resets the buffer full and inserts a special log entry
typedef uint32_t RAIL_Time_t;
typedef uint64_t ProtocolTimerTicks_t;
#define PTICKS_PER_US_ACCURATE           8
static RAIL_Time_t PROTIMER_PrecntOverflowToUs(ProtocolTimerTicks_t timerTicks)
{
  return (RAIL_Time_t)(timerTicks / PTICKS_PER_US_ACCURATE);
}
void sl_btdm_fast_logging_backend()
{
    while (sl_btdm_logging_meta_data->rd_offset != sl_btdm_logging_meta_data->wr_offset)
    {
#ifndef LINUX
        GPIO->P_SET[0].DOUT = 0x40;
#else
        printf("read_log_entry start: wr_offset = %d, rd_offset = %d\n",
               sl_btdm_logging_meta_data->wr_offset,
               sl_btdm_logging_meta_data->rd_offset);
#endif
        SL_BTDM_LOGGING_ENTER_CRITICAL();
        uint32_t signal_lost_count = handle_lost_signals();

        if (signal_lost_count)
        {
            uint8_t signal_lost_entry[8] = {0x0};

            signal_lost_entry[0] = 0x1;

            *(uint32_t *)(&signal_lost_entry[2]) = signal_lost_count;
            sl_btdm_logging_emit_log_entry(signal_lost_entry, 6);
#ifdef LINUX
            printf("signal lost count = %d\n", signal_lost_count);
#endif
        }

        uint32_t rd_offset = sl_btdm_logging_meta_data->rd_offset & sl_btdm_logging_meta_data->max_offset_mask;
        sl_btdm_fast_log_entry_t *log_entry = (sl_btdm_fast_log_entry_t *)(&sl_btdm_fast_logging_buffer[rd_offset]);
        sl_btdm_log_info_int_t *log_info_int = (sl_btdm_log_info_int_t *)&(log_entry->log_sign);
        uint32_t log_info_ext_bytes = sizeof(sl_btdm_log_info_ext_t);

        //! print all fields of log_info
        uint32_t arg1_len = log_info_int->arg1_len;
        uint32_t arg2_len = log_info_int->arg2_len;
        uint32_t arg3_len = log_info_int->arg3_len1 | (log_info_int->arg3_len2 << 1);
        //! the TSF corresponds to the time stamp when the message was placed in the
        //! fast logging buffer and is placed after the sl_btdm_log_info_int_t   
        uint32_t current_tsf = PROTIMER_PrecntOverflowToUs(log_entry->log_tsf);
        uint32_t tsf_diff = current_tsf - sl_btdm_log_db.last_msg_sent_tsf;
        uint8_t log_entry_ext[MAX_LOG_MSG_SIZE];
        uint8_t *log_write_pointer = log_entry_ext;
        sl_btdm_log_info_ext_t *log_info_ext = (sl_btdm_log_info_ext_t *)log_write_pointer;
        
        if (arg1_len == 3) {
            arg1_len = 4;
        }
        if (arg2_len == 3) {
            arg2_len = 4;
        }
        if (arg3_len == 3) {
            arg3_len = 4;
        }
        log_info_ext->ext_did_present = 0;
        log_info_ext->hdid_present = 0;
        log_info_ext->tsf_format = 0;
        log_info_ext->tsf_present = 0;

        log_info_ext->ldid = log_info_int->ldid;
        log_info_ext->component = log_info_int->component;
        if (log_info_int->ext_did)
        {
            //! Extended DID 20-bits
            log_info_ext->ext_did = log_info_int->ext_did;
            log_info_ext->hdid = log_info_int->hdid;
            log_info_ext->ext_did_present = 1;
            log_info_ext->hdid_present = 1;
        }
        else if (log_info_int->hdid)
        {
            //! HDID 12-bits
            log_info_ext->hdid = log_info_int->hdid;
            log_info_ext->ext_did_present = 0;
            log_info_ext->hdid_present = 1;
            log_info_ext_bytes -= 1;
        }
        else
        {
            //! LDID 5-bits
            log_info_ext->ext_did_present = 0;
            log_info_ext->hdid_present = 0;
            // HDID is not required, tsf can start a byte earlier
            log_info_ext_bytes -= 2;
        }

        log_write_pointer += log_info_ext_bytes;
        // add a new TSF only if the tsf_diff is equal/more than granularity configured
        if (tsf_diff < sl_btdm_log_db.log_tsf_granularity)
        {
            log_info_ext->tsf_present = 0;
        }
        else
        {
            log_info_ext->tsf_present = 1;
            if ((tsf_diff > MAX_LTSF) || (sl_btdm_log_db.last_msg_sent_tsf == 0))
            {
                sl_btdm_log_tsf_t *log_tsf = (sl_btdm_log_tsf_t *)log_write_pointer;
                log_info_ext->tsf_format = 1;
                *log_tsf = current_tsf;
                log_write_pointer += sizeof(sl_btdm_log_tsf_t);
            }
            else
            {
                sl_btdm_log_short_tsf_t *log_tsf = (sl_btdm_log_short_tsf_t *)log_write_pointer;
                log_info_ext->tsf_format = 0;
                *log_tsf = tsf_diff;
                log_write_pointer += sizeof(sl_btdm_log_short_tsf_t);
            }
            sl_btdm_log_db.last_msg_sent_tsf = current_tsf;
        }
        
        uint32_t log_entry_size = log_write_pointer - log_entry_ext + arg1_len + arg2_len + arg3_len;
        //! Copy arguiments to the buffer
        memcpy(log_write_pointer, &log_entry->arg1, arg1_len);
        log_write_pointer += arg1_len;
        memcpy(log_write_pointer, &log_entry->arg2, arg2_len);
        log_write_pointer += arg2_len;
        memcpy(log_write_pointer, &log_entry->arg3, arg3_len);
        log_write_pointer += arg3_len;

        sl_btdm_logging_emit_log_entry(log_entry_ext, log_entry_size);
        //! Update RD pointer for fast logging buffer
        
        sl_btdm_logging_meta_data->rd_offset++;
        SL_BTDM_LOGGING_EXIT_CRITICAL();
#ifndef LINUX
        GPIO->P_CLR[0].DOUT = 0x40;
#else
        printf("read_log_entry end: wr_offset = %d, rd_offset = %d\n",
               sl_btdm_logging_meta_data->wr_offset,
               sl_btdm_logging_meta_data->rd_offset);
#endif
    }
}