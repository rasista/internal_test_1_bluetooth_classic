#ifndef ARQN_SEQN_CHECK_H
#define ARQN_SEQN_CHECK_H
#include "btc_device.h"
#include "stdint.h"
#include "esco.h"
#include "debug_logs.h"

uint8_t arqn_stage_1_acl(connection_info_t *connection_info_p);
void arqn_stage_2_acl(connection_info_t *connection_info_p);
void update_seqn(arqn_seqn_scheme_status_t *arqn_seqn_status);
uint8_t arqn_lt_addr_check(uint8_t rcvd_lt_address, current_slot_info_t *current_slot_info);
#endif
