#ifndef MSG_DECODE_H
#define MSG_DECODE_H

#include "btc_device.h"
#include "sl_btc_seq_interface.h"
#include "stdint.h"
#include "string.h"
#include "pkb_mgmt_struct.h"

typedef void (*hss_cmd_handler_t)(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);

typedef struct hss_cmd_start_procedure_lut_s {
  hss_cmd_handler_t handler;
} hss_cmd_start_procedure_lut_t;

typedef struct hss_cmd_set_procedure_lut_s {
  hss_cmd_handler_t handler;
} hss_cmd_set_procedure_lut_t;

typedef struct hss_cmd_stop_procedure_lut_s {
  hss_cmd_handler_t handler;
} hss_cmd_stop_procedure_lut_t;

typedef struct hss_cmd_type_lut_s {
  hss_cmd_handler_t handler;
} hss_cmd_type_lut_t;

typedef struct hss_cmd_get_procedure_lut_s {
  hss_cmd_handler_t handler;
} hss_cmd_get_procedure_lut_t;

void handle_hss_cmd_start_procedure(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void handle_hss_cmd_stop_procedure(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void handle_hss_cmd_set_procedure(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void handler_hss_cmd_get_procedure(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void hss_msg_decode(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void lpw_mailbox_handler(shared_mem_t *queue, btc_dev_t *btc_dev_p);

#endif // MSG_DECODE_H
