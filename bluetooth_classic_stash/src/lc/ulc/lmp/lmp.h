#ifndef __LMP_H__
#define __LMP_H__

#include "pkb_mgmt.h"
#include "btc_lmp.h"
typedef void (*lmp_command_handler_t)(void * ctx, pkb_t *pkb_s);

typedef struct lmp_input_params_s{
  uint8_t opcode;
  uint8_t exopcode;
  uint8_t peer_id;
  uint8_t tid;
  uint8_t reason_code;
  uint8_t input_params[MAX_SIZE_OF_INPUT_PARAMS];
}lmp_input_params_t;

#define ENCODE_LMP_CONTROL_WORD(opcode, exopcode, peer_id, tid) \
  ((uint32_t)(((uint32_t)(tid) << 24)                           \
              | ((uint32_t)(peer_id) << 16)                     \
              | ((uint32_t)(exopcode) << 8)                     \
              | ((uint32_t)(opcode))))

#define DECODE_LMP_OPCODE(lmp_control_word) ((uint8_t)((lmp_control_word) & 0xFF))
#define DECODE_LMP_EXOPCODE(lmp_control_word) ((uint8_t)(((lmp_control_word) >> 8) & 0xFF))
#define DECODE_LMP_PEER_ID(lmp_control_word) ((uint8_t)(((lmp_control_word) >> 16) & 0xFF))
#define DECODE_LMP_TID(lmp_control_word) ((uint8_t)(((lmp_control_word) >> 24) & 0xFF))

typedef void (*lmp_tx_handler_t)(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
typedef void (*lmp_rx_handler_t)(ulc_t *ulc, pkb_t *pkb);
typedef void (*lmp_accepted_rx_handler_t)(ulc_t *ulc, pkb_t *pkb);
typedef void (*lmp_accepted_ext_rx_handler_t)(ulc_t *ulc, pkb_t *pkb);
typedef void (*lmp_not_accepted_rx_handler_t)(ulc_t *ulc, pkb_t *pkb);
typedef void (*lmp_not_accepted_ext_rx_handler_t)(ulc_t *ulc, pkb_t *pkb);

void add_pkt_to_aclc_q(ulc_t *ulc, pkb_t *lmp_tx_pkt, uint8_t peer_id);
void add_tx_pkt_to_queue(queue_t *tx_q, pkb_t *pkb);
pkb_t *prepare_lmp_command_tx_pkt(uint8_t len);
void call_lmp_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
lmp_tx_handler_t get_lmp_tx_handler(uint32_t lmp_control_word);
void call_lmp_rx_handler(ulc_t *ulc, pkb_t *pkb);
lmp_rx_handler_t get_lmp_rx_handler(pkb_t *pkb);
void call_lmp_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb);
void call_lmp_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb);
void call_lmp_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb);
void call_lmp_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb);

#endif
