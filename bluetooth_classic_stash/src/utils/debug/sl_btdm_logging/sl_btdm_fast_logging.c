#include "stdint.h"
#include "sl_btdm_logging.h"

sl_btdm_fast_log_entry_t *sl_btdm_fast_logging_buffer;
sl_btdm_logging_meta_data_t *sl_btdm_logging_meta_data;

uint32_t get_power_of_two_floor(uint32_t num)
{
    num |= (num >> 1);
    num |= (num >> 2);
    num |= (num >> 4);
    num |= (num >> 8);
    num |= (num >> 16);

    return ((num + 1) >> 1);
}
void sl_btdm_fast_logging_init(sl_btdm_fast_log_entry_t *fast_log_entries, sl_btdm_logging_meta_data_t * log_meta_data, uint32_t max_log_entries)
{
    sl_btdm_fast_logging_buffer = fast_log_entries;
    sl_btdm_logging_meta_data = log_meta_data;
    sl_btdm_logging_meta_data->buffer_size = get_power_of_two_floor(max_log_entries);
    sl_btdm_logging_meta_data->max_offset_mask = sl_btdm_logging_meta_data->buffer_size - 1;
    sl_btdm_logging_meta_data->wr_offset = 0;
    sl_btdm_logging_meta_data->rd_offset = 0;
}