#ifndef __ACCESS_CODE_GEN_H__
#define __ACCESS_CODE_GEN_H__
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void lc_polynom_modulo(uint32_t *dividend, uint32_t *divisor, uint32_t *remainder);
void access_code_synth(uint32_t *lap_addr, uint32_t *bt_acc_code);

#endif
