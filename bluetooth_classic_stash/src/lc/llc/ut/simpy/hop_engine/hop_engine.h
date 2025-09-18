#include "/usr/include/x86_64-linux-gnu/bits/stdint-uintn.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifndef HOP_ENGINE_H
#define HOP_ENGINE_H

typedef struct __attribute__((packed)) frequency_hop_io_s {
  uint8_t n;
  uint8_t k_offset;
  uint8_t k_nudge;
  uint8_t sequence_selection;
  uint8_t interlace_offset;
  uint16_t *afh_map_ptr;
  uint32_t clk;
  uint32_t ulap;
  uint32_t frozen_clk;
} frequency_hop_io_t;

extern frequency_hop_io_t *current_frequency_hop_params;

uint8_t **tokenize(int argc, uint8_t *line);
uint32_t get_num_bits(uint32_t num, uint8_t num_bits, uint8_t start);
uint32_t get_indv_bits(uint32_t num, uint8_t argc, uint8_t *var_args);
uint32_t get_Xp();
uint32_t get_Xprp();
uint32_t get_Xprc();
uint32_t get_Xi();
uint32_t get_Xir();
uint8_t Adder_2(uint8_t arg1, uint8_t arg2);
uint32_t Adder_3(uint8_t arg1, uint8_t arg2, uint8_t arg3);
uint32_t Adder_4(uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t arg4);
uint32_t XOR(uint8_t arg1, uint8_t arg2);
void swap_bits(uint8_t *z_val, uint8_t bit_1, uint8_t bit_2);
void butterfly(uint8_t *z_val, uint8_t bit_1, uint8_t bit_2, uint8_t P_val);
uint8_t PERM_5(uint8_t xor1_out_local, uint8_t xor2_out_local,
               uint16_t D_local);
uint32_t hopEngine();
uint32_t remapping_func(uint32_t channel);
void init_hop_params_memory();
uint32_t get_chn_number(frequency_hop_io_t *frequency_hop_params);
#endif
