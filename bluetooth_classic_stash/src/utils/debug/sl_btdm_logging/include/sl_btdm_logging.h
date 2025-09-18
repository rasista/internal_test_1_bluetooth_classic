#ifndef __SL_BTDM_LOGGING_H__
#define __SL_BTDM_LOGGING_H__
#include "stdint.h"

extern void plt_enter_critical();
extern void plt_exit_critical();
extern uint32_t plt_get_timestamp();

#define SL_BTDM_LOGGING_ENTER_CRITICAL() plt_enter_critical()
#define SL_BTDM_LOGGING_EXIT_CRITICAL() plt_exit_critical()
#define SL_BTDM_LOGGING_GET_TIMESTAMP() plt_get_timestamp()

// LTSF is 16 bits wide
#define MAX_LTSF ((1 << sizeof(sl_btdm_log_short_tsf_t)*8)  - 1)
// Maximum number of arguments
#define MAX_NUM_ARGS 3

#define MAX_LOG_MSG_SIZE sizeof(sl_btdm_log_info_ext_t) + sizeof(sl_btdm_log_tsf_t) + MAX_NUM_ARGS * sizeof(uint32_t)
typedef uint32_t sl_btdm_log_tsf_t;
typedef uint16_t sl_btdm_log_short_tsf_t;

typedef struct sl_btdm_fast_log_entry_s {
  uint32_t log_sign;
  uint32_t log_tsf;
  uint32_t arg1;
  uint32_t arg2;
  uint32_t arg3;
} sl_btdm_fast_log_entry_t;

typedef struct sl_btdm_logging_meta_data_s {
  uint32_t rd_offset;
  uint32_t wr_offset;
  uint32_t buffer_size;
  uint32_t max_offset_mask;
} sl_btdm_logging_meta_data_t;

typedef enum btc_log_levels_e{
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARN = 3,
  ERROR = 4,
  FATAL = 5,
} btc_log_levels_t;


typedef struct sl_btdm_log_info_ext_s {
  //! FIrst 2 bytes are mandatorily required on the wire
  //!!!!!!!!!!!!!!!!First Byte
  uint32_t component : 6;
  uint32_t tsf_format : 1;
  uint32_t tsf_present : 1;
  //!!!!!!!!!!!!!!!!Second Byte
  //! optional 1 byte if HDID is required
  uint32_t ldid : 7;
  uint32_t hdid_present : 1;
  //!!!!!!!!!!!!!!!!Third Byte
  uint32_t hdid : 7;
  uint32_t ext_did_present : 1;
  //!!!!!!!!!!!!!!!!Fourth Byte
  //! optional 1 byte if ext_did is required
  uint32_t ext_did : 6;
  uint32_t rfu : 2;
} sl_btdm_log_info_ext_t;

typedef struct sl_btdm_log_info_int_s {
  //!!!!!!!!!!!!!!!!First Byte
  uint32_t component : 6;
  uint32_t arg2_len : 2;
  //!!!!!!!!!!!!!!!!Second Byte
  uint32_t ldid : 7;
  uint32_t arg3_len1 : 1;
  //!!!!!!!!!!!!!!!!Third Byte
  uint32_t hdid : 7;
  uint32_t arg3_len2 : 1;
  //!!!!!!!!!!!!!!!!Fourth Byte
  uint32_t ext_did : 6;
  uint32_t arg1_len : 2;
} sl_btdm_log_info_int_t;

typedef union sl_btdm_log_signature_u {
  sl_btdm_log_info_int_t log_info_int;
  sl_btdm_log_info_ext_t log_info_ext;
  uint32_t signature;
} sl_btdm_log_signature_t;

typedef struct sl_btdm_log_db_s {
  uint32_t last_msg_sent_tsf;
  uint32_t log_tsf_granularity;
  uint8_t *buffer;
  uint32_t buffer_size;
  uint32_t rd_offset;
  uint32_t wr_offset;
} sl_btdm_log_db_t;

extern sl_btdm_fast_log_entry_t *sl_btdm_fast_logging_buffer;
extern sl_btdm_logging_meta_data_t *sl_btdm_logging_meta_data;

void sl_btdm_fast_log_no_args(uint32_t log_signature);
void sl_btdm_fast_log_1_args(uint32_t log_signature, uint32_t arg1);
void sl_btdm_fast_log_2_args(uint32_t log_signature, uint32_t arg1, uint32_t arg2);
void sl_btdm_fast_log_3_args(uint32_t log_signature, uint32_t arg1, uint32_t arg2, uint32_t arg3);
void sl_btdm_fast_logging_backend();;
void sl_btdm_logging_emit_log_entry(uint8_t *log_entry, uint32_t log_entry_size);
void sl_btdm_fast_logging_init(sl_btdm_fast_log_entry_t *fast_log_entries, sl_btdm_logging_meta_data_t * log_meta_data, uint32_t max_log_entries);
#endif