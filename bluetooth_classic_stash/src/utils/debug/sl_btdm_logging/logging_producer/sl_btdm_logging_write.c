#include "sl_btdm_logging.h"

extern sl_btdm_log_db_t sl_btdm_log_db;


static inline generate_external_log_header(sl_btdm_log_info_ext_t *log_info_ext, uint8_t *log_write_pointer)
{
    uint32_t current_tsf = sl_sleeptimer_get_tick_count();
    uint32_t log_info_ext_bytes = sizeof(sl_btdm_log_info_ext_t);
    
    uint32_t ext_did_present = !!log_info_ext->ext_did;
    uint32_t hdid_present = !!log_info_ext->hdid;
    uint32_t tsf_present;
    uint32_t tsf_format;
    log_info_ext_bytes -= !ext_did_present - !hdid_present;

    uint32_t tsf_bytes;
    // add a new TSF only if the tsf_diff is equal/more than granularity configured
    uint32_t tsf_diff = current_tsf - sl_btdm_log_db.last_msg_sent_tsf;
    tsf_present = (tsf_diff >= sl_btdm_log_db.log_tsf_granularity);
    tsf_format = ((tsf_diff > MAX_LTSF) || (sl_btdm_log_db.last_msg_sent_tsf == 0));
    tsf_bytes = ((tsf_present << 1) << tsf_format);

*(uint32_t *)log_write_pointer = (tsf_format << 6) | (tsf_present << 7) |
            (hdid_present << 15) | (ext_did_present << 23) |
            (log_info_ext->ldid << 8) |
            (log_info_ext->hdid << 16) | 
            (log_info_ext->ext_did << 24);
    *(uint32_t *)(log_write_pointer + log_info_ext_bytes) = tsf_diff;
    log_info_ext_bytes += tsf_bytes;
    if (tsf_present)
    {
        sl_btdm_log_db.last_msg_sent_tsf = current_tsf;
    }
    return log_info_ext_bytes;
}

void sl_btdm_log_no_args(uint32_t log_sign)
{
    uint8_t log_entry[MAX_LOG_MSG_SIZE];
    sl_btdm_log_info_ext_t *log_info_ext = (sl_btdm_log_info_ext_t *)log_entry;
    sl_btdm_log_signature_t log_signature;
    uint32_t log_info_ext_bytes = 0;
    
    log_signature.signature = log_sign;
    *log_info_ext = log_signature.log_info_ext;
    log_info_ext_bytes = generate_external_log_header(log_info_ext, log_entry);
    log_info_ext_bytes += generate_time_header(log_info_ext, &log_entry[log_info_ext_bytes]);
    sl_btdm_logging_emit_log_entry(log_entry, log_info_ext_bytes);
}

void sl_btdm_log_1_arg(uint32_t log_sign, uint32_t arg1)
{
    uint8_t log_entry[MAX_LOG_MSG_SIZE];
    sl_btdm_log_info_ext_t *log_info_ext = (sl_btdm_log_info_ext_t *)log_entry;
    sl_btdm_log_signature_t log_signature;
    uint32_t log_info_ext_bytes;
    uint32_t arg1_len = 0;

    
    log_signature.signature = log_sign;
    arg1_len = log_signature.log_info_int.arg1_len;
    if (arg1_len == 3)
    {
        arg1_len = 4;
    }
    *log_info_ext = log_signature.log_info_ext;
    log_info_ext_bytes = generate_external_log_header(log_info_ext, log_entry);
    *(uint32_t *)&log_entry[log_info_ext_bytes] = arg1;
    log_info_ext_bytes += arg1_len;
    sl_btdm_logging_emit_log_entry(log_entry, log_info_ext_bytes);
}

void sl_btdm_log_2_args(uint32_t log_sign, uint32_t arg1, uint32_t arg2)
{
    uint8_t log_entry[MAX_LOG_MSG_SIZE];
    sl_btdm_log_info_ext_t *log_info_ext = (sl_btdm_log_info_ext_t *)log_entry;
    sl_btdm_log_signature_t log_signature;
    uint32_t log_info_ext_bytes;
    uint32_t arg1_len = 0;
    uint32_t arg2_len = 0;

    log_signature.signature = log_sign;
    arg1_len = log_signature.log_info_int.arg1_len;
    if (arg1_len == 3)
    {
        arg1_len = 4;
    }
    arg2_len = log_signature.log_info_int.arg2_len;
    if (arg2_len == 3)
    {
        arg2_len = 4;
    }
    *log_info_ext = log_signature.log_info_ext;
    log_info_ext_bytes = generate_external_log_header(log_info_ext, log_entry);
    *(uint32_t *)&log_entry[log_info_ext_bytes] = arg1;
    log_info_ext_bytes += arg1_len;
    *(uint32_t *)&log_entry[log_info_ext_bytes] = arg2;
    log_info_ext_bytes += arg2_len;
    sl_btdm_logging_emit_log_entry(log_entry, log_info_ext_bytes);


}

void sl_btdm_log_3_args(uint32_t log_sign, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    uint8_t log_entry[MAX_LOG_MSG_SIZE];
    sl_btdm_log_info_ext_t *log_info_ext = (sl_btdm_log_info_ext_t *)log_entry;
    sl_btdm_log_signature_t log_signature;
    uint32_t log_info_ext_bytes = sizeof(sl_btdm_log_info_ext_t);
    uint32_t arg1_len = 0;
    uint32_t arg2_len = 0;
    uint32_t arg3_len = 0;

    log_signature.signature = log_sign;
    arg1_len = log_signature.log_info_int.arg1_len;
    if (arg1_len == 3)
    {
        arg1_len = 4;
    }
    arg2_len = log_signature.log_info_int.arg2_len;
    if (arg2_len == 3)
    {
        arg2_len = 4;
    }
    arg3_len = log_signature.log_info_int.arg3_len1 |
               (log_signature.log_info_int.arg3_len2 << 1);
    if (arg3_len == 3)
    {
        arg3_len = 4;
    }
    *log_info_ext = log_signature.log_info_ext;
    // log_info_ext_bytes = generate_external_log_header(log_info_ext, log_entry);
    uint32_t current_tsf = sl_sleeptimer_get_tick_count();

    uint32_t ext_did_present = !!log_info_ext->ext_did;
    uint32_t hdid_present = !!log_info_ext->hdid;
    uint32_t tsf_present;
    uint32_t tsf_format;
    log_info_ext_bytes -= !ext_did_present - !hdid_present;

    uint32_t tsf_bytes;
    // add a new TSF only if the tsf_diff is equal/more than granularity configured
    uint32_t tsf_diff = current_tsf - sl_btdm_log_db.last_msg_sent_tsf;
    tsf_present = (tsf_diff >= sl_btdm_log_db.log_tsf_granularity);
    tsf_format = ((tsf_diff > MAX_LTSF) || (sl_btdm_log_db.last_msg_sent_tsf == 0));
    tsf_bytes = ((tsf_present << 1) << tsf_format);
    uint8_t *log_write_pointer = log_entry;

    *(uint32_t *)log_write_pointer = (tsf_format << 6) | (tsf_present << 7) |
                                     (hdid_present << 15) | (ext_did_present << 23) |
                                     (log_info_ext->ldid << 8) |
                                     (log_info_ext->hdid << 16) |
                                     (log_info_ext->ext_did << 24);
    *(uint32_t *)(log_write_pointer + log_info_ext_bytes) = tsf_diff;
    log_info_ext_bytes += tsf_bytes;
    if (tsf_present)
    {
        sl_btdm_log_db.last_msg_sent_tsf = current_tsf;
    }
    *(uint32_t *)&log_entry[log_info_ext_bytes] = arg1;
    log_info_ext_bytes += arg1_len;

    *(uint32_t *)&log_entry[log_info_ext_bytes] = arg2;
    log_info_ext_bytes += arg2_len;

    *(uint32_t *)&log_entry[log_info_ext_bytes] = arg3;
    log_info_ext_bytes += arg3_len;

    sl_btdm_logging_emit_log_entry(log_entry, log_info_ext_bytes);
}