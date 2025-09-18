#include "hop_engine.h"
#include "stdint.h"
// #include "btc_device.h"
// #include "rail_simulation.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define NO_SYNC_LINK

#ifdef NO_SYNC_LINK
#define R0_N_PAGE 1
#define R1_N_PAGE 128
#define R2_N_PAGE 256
#else
#ifdef ONE_SYNC_LINK
#define R0_N_PAGE 2
#define R1_N_PAGE 256
#define R2_N_PAGE 512
#else
#define R0_N_PAGE 3
#define R1_N_PAGE 384
#define R2_N_PAGE 768
#endif
#endif

// List of selection box inputs

#define PAGE_SCAN_SEQ 0
#define INQ_SCAN_SEQ 1
#define PAGE_SEQ 2
#define INQ_SEQ 3
#define CENTRAL_PAGE_RESPONSE 4
#define PERIPHERAL_PAGE_RESPONSE 5
#define INQ_RESP 6
#define BASIC_CHNL 7
#define ADAPT_CHNL 8

frequency_hop_io_t *current_frequency_hop_params;

uint8_t used_channel_map[80] = { 0 };
// Sequence Selection States

uint8_t remap_arr[] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26,
                        28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54,
                        56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 1, 3,
                        5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31,
                        33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59,
                        61, 63, 65, 67, 69, 71, 73, 75, 77 };

// Frequency Hop Engine inputs
uint8_t n;
uint8_t k_offset;
uint8_t k_nudge;
uint8_t sequence_selection;
uint8_t interlace_offset;
uint16_t *afh_map_ptr;
uint32_t clk;
uint32_t ulap;
uint32_t frozen_clk;
uint16_t RF_channel_index;

uint8_t Nmin = 79;

// Basic Hop engine inputs
uint32_t X_val;
uint32_t A_val;
uint32_t B_val;
uint32_t C_val;
uint16_t D_val;
uint32_t E_val;
uint32_t F_val;
uint32_t Y1_val;
uint32_t Y2_val;
uint32_t Perm_val[14];
uint32_t Perm_Z[14][2] = { { 0, 1 }, { 2, 3 }, { 1, 2 }, { 3, 4 }, { 0, 4 }, { 1, 3 }, { 0, 2 },
                           { 3, 4 }, { 0, 4 }, { 0, 3 }, { 2, 4 }, { 1, 3 }, { 0, 3 }, { 1, 2 } };

// uint32_t Perm_Z[14][2] = {
//   // P0…P6
//   {0,1},  {2,3},  {1,2},  {3,4},  {0,4},  {1,3},  {0,2},
//   // P7…P13  (repeat!)
//   {0,1},  {2,3},  {1,2},  {3,4},  {0,4},  {1,3},  {0,2}
// };
uint8_t Perm5_out;

uint8_t **tokenize(int argc, uint8_t *line)
{
  uint8_t *line_loc;
  line_loc = (uint8_t *)malloc(sizeof(uint8_t) * strlen(line));
  if (line_loc == NULL) {
    printf("NULL Pointer Exception");
  }
  strcpy(line_loc, line);

  uint8_t *cmd = strtok(line_loc, " ");

  uint8_t **argv = (uint8_t **)malloc(argc * sizeof(uint8_t *));
  for (int i = 0; i < argc; i++) {
    argv[i] = (uint8_t *)malloc(10 * sizeof(uint8_t));
  }
  int i = 0;

  while (cmd != NULL) {
    strcpy(argv[i], cmd);
    cmd = strtok(NULL, " ");
    i++;
  }
  free(line_loc);
  return argv;
}

uint32_t get_num_bits(uint32_t num, uint8_t num_bits, uint8_t start)
{
  uint32_t result_bits = 0;
  uint32_t mask = 0;
  for (uint8_t i = 0; i < num_bits; i++) {
    mask = (mask << 1) | 1;
  }
  result_bits = ((num & (mask << (start))) >> start);
  return result_bits;
}

uint32_t get_indv_bits(uint32_t num, uint8_t argc, uint8_t *var_args)
{
  uint32_t result_bits = 0;
  uint8_t **argv;
  argv = tokenize(argc, var_args);
  for (int i = 0; i < argc; i++) {
    // printf("%u %s ", clk, argv[i]);
  }
  for (uint8_t i = 0; i < argc; i++) {
    result_bits = (result_bits << 1) + get_num_bits(num, 1, atoi(argv[i]));
  }
  free(argv);
  return result_bits;
}

uint32_t get_Xp()
{
  return (get_num_bits(clk, 5, 12) + k_nudge + k_offset + (get_indv_bits(clk, 4, (uint8_t *)"4 3 2 0") - get_num_bits(clk, 5, 12) + 32) % 16) % 32;
}

uint32_t get_Xprp()
{
  return ((get_num_bits(frozen_clk, 5, 12) + n) % 32);
}

uint32_t get_Xprc()
{
  return (get_num_bits(frozen_clk, 5, 12) + k_offset + k_nudge
          + (get_indv_bits(frozen_clk, 4, (uint8_t *)"4 3 2 0")
             - get_indv_bits(frozen_clk, 5, (uint8_t *)"16 15 14 13 12"))
          % 16
          + n)
         % 32;
}

uint32_t get_Xi()
{
  return (get_num_bits(clk, 5, 12) + k_offset + k_nudge
          + (get_indv_bits(clk, 4, (uint8_t *)"4 3 2 0")
             - get_num_bits(clk, 5, 12))
          % 16)
         % 32;
}

uint32_t get_Xir()
{
  return (get_num_bits(clk, 5, 12) + n) % 32;
}

uint8_t Adder_2(uint8_t arg1, uint8_t arg2)
{
  // sim_printf("Arg1 : %x Arg2 : %x sum : %x", arg1, arg2, (arg1 + arg2))
  return get_num_bits((arg1 + arg2), 5, 0);
}

uint32_t Adder_3(uint8_t arg1, uint8_t arg2, uint8_t arg3)
{
  return (arg1 + arg2 + arg3) % 32;
}

uint32_t Adder_4(uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t arg4)
{
  return (arg1 + arg2 + arg3 + arg4) % 79;
}

uint32_t XOR(uint8_t arg1, uint8_t arg2)
{
  return arg1 ^ arg2;
}

void swap_bits(uint8_t *z_val, uint8_t bit_1, uint8_t bit_2)
{
  uint8_t z_val_loc;
  z_val_loc = *z_val;
  if ((((*z_val) & (1 << bit_1)) >> bit_1)
      != (((*z_val) & (1 << bit_2)) >> bit_2)) {
    *z_val = ((*z_val) ^ (1 << bit_1 | 1 << bit_2));
  }
}

void butterfly(uint8_t *z_val, uint8_t bit_1, uint8_t bit_2, uint8_t P_val)
{
  if (P_val == 1) {
    // sim_printf("bit1 %u bit2 %u", bit_1, bit_2);
    swap_bits(z_val, bit_1, bit_2);
  }
}

uint8_t PERM_5(uint8_t xor1_out_local, uint8_t xor2_out_local,
               uint16_t D_local)
{
  uint8_t temp = xor1_out_local;
  for (uint8_t i = 0; i < 9; i++) {
    Perm_val[i] = D_local & 1;
    D_local >>= 1;
  }

  for (uint8_t i = 9; i < 14; i++) {
    Perm_val[i] = ((xor2_out_local & 1));
    xor2_out_local >>= 1;
  }

  for (int i = 13; i >= 0; i -= 2) {
    butterfly(&temp, Perm_Z[i][0], Perm_Z[i][1], Perm_val[i]);
    butterfly(&temp, Perm_Z[i - 1][0], Perm_Z[i - 1][1], Perm_val[i - 1]);
  }

  return temp & 0x1F;
}

uint32_t hopEngine()
{
  switch (sequence_selection) {
    case PAGE_SCAN_SEQ:
      X_val = get_num_bits(clk, 5, 12);
      Y1_val = 0;
      Y2_val = 0;
      A_val = get_num_bits(ulap, 5, 23);
      B_val = get_num_bits(ulap, 4, 19);
      C_val = get_num_bits(get_indv_bits(clk, 5, (uint8_t *)"8 6 4 2 0"), 5, 0);
      D_val = get_num_bits(ulap, 9, 10);
      E_val = get_num_bits(get_indv_bits(ulap, 7, (uint8_t *)"13 11 9 7 5 3 1"),
                           7, 0);
      F_val = 0;
      break;
    case INQ_SCAN_SEQ:
      X_val = get_num_bits(get_Xir(), 5, 0);
      Y1_val = 0;
      Y2_val = 0;
      A_val = get_num_bits(ulap, 5, 23);
      B_val = get_num_bits(ulap, 4, 19);
      C_val = get_num_bits(get_indv_bits(ulap, 5, (uint8_t *)"8 6 4 2 0"), 5, 0);
      D_val = get_num_bits(ulap, 9, 10);
      E_val = get_num_bits(get_indv_bits(ulap, 7, (uint8_t *)"13 11 9 7 5 3 1"),
                           7, 0);
      F_val = 0;
      break;
    case PAGE_SEQ:
      X_val = get_num_bits(get_Xp(), 5, 0);
      Y1_val = get_num_bits(clk, 1, 1);
      Y2_val = 32 * get_num_bits(clk, 1, 1);
      A_val = get_num_bits(ulap, 5, 23);
      B_val = get_num_bits(ulap, 4, 19);
      C_val = get_num_bits(get_indv_bits(ulap, 5, (uint8_t *)"8 6 4 2 0"), 5, 0);
      D_val = get_num_bits(ulap, 9, 10);
      E_val = get_num_bits(get_indv_bits(ulap, 7, (uint8_t *)"13 11 9 7 5 3 1"),
                           7, 0);
      F_val = 0;
      break;
    case INQ_SEQ:
      X_val = get_num_bits(get_Xi(), 5, 0);
      Y1_val = get_indv_bits(clk, 1, (uint8_t *)"1");
      Y2_val = 32 * get_indv_bits(clk, 1, (uint8_t *)"1");
      A_val = get_num_bits(ulap, 5, 23);
      B_val = get_num_bits(ulap, 4, 19);
      C_val = get_indv_bits(ulap, 5, (uint8_t *)"8 6 4 2 0");
      D_val = get_num_bits(ulap, 9, 10);
      E_val = get_indv_bits(ulap, 7, (uint8_t *)"13 11 9 7 5 3 1");
      F_val = 0;
      break;
    case CENTRAL_PAGE_RESPONSE:
      X_val = get_num_bits(get_Xprc(), 5, 0);
      Y1_val = get_indv_bits(clk, 1, (uint8_t *)"1");
      Y2_val = 32 * get_indv_bits(clk, 1, (uint8_t *)"1");
      A_val = get_num_bits(ulap, 5, 23);
      B_val = get_num_bits(ulap, 4, 19);
      C_val = get_indv_bits(ulap, 5, (uint8_t *)"8 6 4 2 0");
      D_val = get_num_bits(ulap, 9, 10);
      E_val = get_indv_bits(ulap, 7, (uint8_t *)"13 11 9 7 5 3 1");
      F_val = 0;
      break;
    case PERIPHERAL_PAGE_RESPONSE:
      X_val = get_num_bits(get_Xprp(), 5, 0);
      Y1_val = get_num_bits(clk, 1, 1);
      Y2_val = 32 * Y1_val;
      A_val = get_num_bits(ulap, 5, 23);
      B_val = get_num_bits(ulap, 4, 19);
      C_val = get_indv_bits(ulap, 5, (uint8_t *)"8 6 4 2 0");
      D_val = get_num_bits(ulap, 9, 10);
      E_val = get_indv_bits(ulap, 7, (uint8_t *)"13 11 9 7 5 3 1");
      F_val = 0;
      break;
    case INQ_RESP:
      X_val = get_num_bits(get_Xir(), 5, 0);
      Y1_val = 1;
      Y2_val = 32 * 1;
      A_val = get_num_bits(ulap, 5, 23);
      B_val = get_num_bits(ulap, 4, 19);
      C_val = get_indv_bits(ulap, 5, (uint8_t *)"8 6 4 2 0");
      D_val = get_num_bits(ulap, 9, 10);
      E_val = get_indv_bits(ulap, 7, (uint8_t *)"13 11 9 7 5 3 1");
      F_val = 0;
      break;
    case BASIC_CHNL:
      // todo
      break;
    case ADAPT_CHNL:
      // todo
      break;
    default:
      break;
  }

  // printf("X_val : 0x%X\n\tY1_val : 0x%X\n\tY2_val : 0x%X\n\tA_val : 0x%X\n\tB_val :0x%X\n\tC_val : 0x%X\n\tD_val : 0x%X\n\tE_val : 0x%X\n\tF_val : 0x%X\n\t", X_val, Y1_val, Y2_val,  A_val,   B_val,   C_val,   D_val,   E_val,  F_val );
  uint8_t adder_2_out = get_num_bits(Adder_2(X_val, A_val), 5, 0);
  uint8_t XOR_out =
    get_num_bits(XOR(adder_2_out, get_num_bits(B_val, 4, 0)), 5, 0);
  uint8_t XOR_out_2 = ((get_num_bits(C_val, 1, 4) ^ Y1_val) << 4) | ((get_num_bits(C_val, 1, 3) ^ Y1_val) << 3) | ((get_num_bits(C_val, 1, 2) ^ Y1_val) << 2) | ((get_num_bits(C_val, 1, 1) ^ Y1_val) << 1) | (get_num_bits(C_val, 1, 0) ^ Y1_val);
  uint8_t perm_out = PERM_5(XOR_out, XOR_out_2, get_num_bits(D_val, 9, 0));
  // printf("Perm_out : 0x%X ", perm_out);
  perm_out = get_num_bits(perm_out, 5, 0);
  Perm5_out = perm_out;
  uint8_t channel_num =
    get_num_bits(Adder_4(perm_out, E_val, F_val, Y2_val) % 79, 7, 0);
  // sim_printf("adder_2_out : 0x%X XOR_out : 0x%X XOR_out_2 : 0x%X perm_out
  // :0x%X ", adder_2_out, XOR_out, XOR_out_2, perm_out)

  return channel_num;
}

uint32_t remapping_func(uint32_t channel)
{
  // return channel;
  // sim_printf("Channel -> %u ", channel);
  uint32_t remapped_channel = channel;
  uint32_t adder_out;
  if (used_channel_map[channel] == 1) {
    // sim_printf("channel %u", channel);
    adder_out = Adder_4(E_val, Y2_val, F_val, Perm5_out) % Nmin;
    used_channel_map[channel] = 1;
    return adder_out;
  }
  used_channel_map[channel] = 1;
  return remapped_channel;
}

uint32_t get_chn_number(frequency_hop_io_t *frequency_hop_params)
{
  n = frequency_hop_params->n;
  k_offset = frequency_hop_params->k_offset;
  k_nudge = frequency_hop_params->k_nudge;
  sequence_selection = frequency_hop_params->sequence_selection;
  interlace_offset = frequency_hop_params->interlace_offset;
  afh_map_ptr = frequency_hop_params->afh_map_ptr;
  clk = frequency_hop_params->clk;
  ulap = frequency_hop_params->ulap;
  frozen_clk = frequency_hop_params->frozen_clk;
#if 0
  sim_printf("n: %u k_offset: %u k_nudge: %u sequence_selection: %u "
             "interlace_offset: %u clk: %x ulap: %x frozen_clk: %u ",
             n, k_offset, k_nudge, sequence_selection, interlace_offset, clk,
             ulap, frozen_clk);
#endif
  RF_channel_index = hopEngine();
  // sim_printf("Channel before remap %u", RF_channel_index);
  return remap_arr[RF_channel_index];
}

void init_hop_params_memory()
{
  current_frequency_hop_params =
    (frequency_hop_io_t *)malloc(sizeof(frequency_hop_io_t));
  memset(current_frequency_hop_params, 0, sizeof(frequency_hop_io_t));
}

int main(int argc, char *argv[])
{
  uint8_t ticks = 32;
  frequency_hop_io_t *curr_frequency_hop_params =
    (frequency_hop_io_t *)malloc(sizeof(frequency_hop_io_t));
  uint8_t current_channel;
  uint16_t *afh_map;
  // curr_frequency_hop_params->afh_map_ptr = afh_map;
  afh_map = (uint16_t *)calloc(sizeof(uint8_t), 80);
  uint8_t count = 0;
  uint8_t state = 0;
  printf("\t");
  for (int i = 0; i <= 0xf; i++) {
    printf(" %x\t", i);
  }
  printf("\n");
  for (uint64_t j = 0x14; j <= 0x14 + 0x30; j += 0x10) {
    printf("\n%lx\t", j);
    curr_frequency_hop_params->n = 0;

    for (uint64_t i = 0; i <= 0xf; i += 2) {
      curr_frequency_hop_params->n++;
      curr_frequency_hop_params->k_offset = 24;
      curr_frequency_hop_params->clk = i + j;
      curr_frequency_hop_params->sequence_selection = CENTRAL_PAGE_RESPONSE;
      curr_frequency_hop_params->k_nudge = 0;
      curr_frequency_hop_params->ulap = 0;
      curr_frequency_hop_params->frozen_clk = 0x12;
      current_channel = get_chn_number(curr_frequency_hop_params);
      printf("%u\t", current_channel);
      if (i % 0x1e == 0 && i != 0) {
        printf("\n");
      }
    }
  }
  printf("\n");
  free(curr_frequency_hop_params);
  free(afh_map);
  return 0;
}
