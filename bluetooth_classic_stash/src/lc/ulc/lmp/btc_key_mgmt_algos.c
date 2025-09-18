#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"
#include "btc_lmp.h"
#include "btc_process_hci_cmd.h"
#include "btc_hci_events.h"
#include "btc_dev.h"
#include "hci_cmd_get_defines_autogen.h"
#include "btc_key_mgmt_algos.h"
#ifdef SCHED_SIM
#include "ssp_sw_algos.h"
#endif

#define INPUT_DATA_BLOCK_SIZE 16
#define INPUT_KEY_SIZE        16
#define NUM_OF_ROUNDS         8
#define SUB_KEY_SIZE          16
#define PADDED_KEY_LENGTH     17
#define NUM_OF_BA_OCTETS      6
#define NUM_OF_COF_OCTETS     12

#define AR       0
#define AR_PRIME 1

const uint8_t e_box[256] = {
  0x01, 0x2d, 0xe2, 0x93, 0xbe, 0x45, 0x15, 0xae, 0x78, 0x03, 0x87, 0xa4, 0xb8, 0x38, 0xcf, 0x3f, 0x08, 0x67, 0x09,
  0x94, 0xeb, 0x26, 0xa8, 0x6b, 0xbd, 0x18, 0x34, 0x1b, 0xbb, 0xbf, 0x72, 0xf7, 0x40, 0x35, 0x48, 0x9c, 0x51, 0x2f,
  0x3b, 0x55, 0xe3, 0xc0, 0x9f, 0xd8, 0xd3, 0xf3, 0x8d, 0xb1, 0xff, 0xa7, 0x3e, 0xdc, 0x86, 0x77, 0xd7, 0xa6, 0x11,
  0xfb, 0xf4, 0xba, 0x92, 0x91, 0x64, 0x83, 0xf1, 0x33, 0xef, 0xda, 0x2c, 0xb5, 0xb2, 0x2b, 0x88, 0xd1, 0x99, 0xcb,
  0x8c, 0x84, 0x1d, 0x14, 0x81, 0x97, 0x71, 0xca, 0x5f, 0xa3, 0x8b, 0x57, 0x3c, 0x82, 0xc4, 0x52, 0x5c, 0x1c, 0xe8,
  0xa0, 0x04, 0xb4, 0x85, 0x4a, 0xf6, 0x13, 0x54, 0xb6, 0xdf, 0x0c, 0x1a, 0x8e, 0xde, 0xe0, 0x39, 0xfc, 0x20, 0x9b,
  0x24, 0x4e, 0xa9, 0x98, 0x9e, 0xab, 0xf2, 0x60, 0xd0, 0x6c, 0xea, 0xfa, 0xc7, 0xd9, 0x00, 0xd4, 0x1f, 0x6e, 0x43,
  0xbc, 0xec, 0x53, 0x89, 0xfe, 0x7a, 0x5d, 0x49, 0xc9, 0x32, 0xc2, 0xf9, 0x9a, 0xf8, 0x6d, 0x16, 0xdb, 0x59, 0x96,
  0x44, 0xe9, 0xcd, 0xe6, 0x46, 0x42, 0x8f, 0x0a, 0xc1, 0xcc, 0xb9, 0x65, 0xb0, 0xd2, 0xc6, 0xac, 0x1e, 0x41, 0x62,
  0x29, 0x2e, 0x0e, 0x74, 0x50, 0x02, 0x5a, 0xc3, 0x25, 0x7b, 0x8a, 0x2a, 0x5b, 0xf0, 0x06, 0x0d, 0x47, 0x6f, 0x70,
  0x9d, 0x7e, 0x10, 0xce, 0x12, 0x27, 0xd5, 0x4c, 0x4f, 0xd6, 0x79, 0x30, 0x68, 0x36, 0x75, 0x7d, 0xe4, 0xed, 0x80,
  0x6a, 0x90, 0x37, 0xa2, 0x5e, 0x76, 0xaa, 0xc5, 0x7f, 0x3d, 0xaf, 0xa5, 0xe5, 0x19, 0x61, 0xfd, 0x4d, 0x7c, 0xb7,
  0x0b, 0xee, 0xad, 0x4b, 0x22, 0xf5, 0xe7, 0x73, 0x23, 0x21, 0xc8, 0x05, 0xe1, 0x66, 0xdd, 0xb3, 0x58, 0x69, 0x63,
  0x56, 0x0f, 0xa1, 0x31, 0x95, 0x17, 0x07, 0x3a, 0x28
};

const uint8_t l_box[256] = {
  0x80, 0x00, 0xb0, 0x09, 0x60, 0xef, 0xb9, 0xfd, 0x10, 0x12, 0x9f, 0xe4, 0x69, 0xba, 0xad, 0xf8, 0xc0, 0x38, 0xc2,
  0x65, 0x4f, 0x06, 0x94, 0xfc, 0x19, 0xde, 0x6a, 0x1b, 0x5d, 0x4e, 0xa8, 0x82, 0x70, 0xed, 0xe8, 0xec, 0x72, 0xb3,
  0x15, 0xc3, 0xff, 0xab, 0xb6, 0x47, 0x44, 0x01, 0xac, 0x25, 0xc9, 0xfa, 0x8e, 0x41, 0x1a, 0x21, 0xcb, 0xd3, 0x0d,
  0x6e, 0xfe, 0x26, 0x58, 0xda, 0x32, 0x0f, 0x20, 0xa9, 0x9d, 0x84, 0x98, 0x05, 0x9c, 0xbb, 0x22, 0x8c, 0x63, 0xe7,
  0xc5, 0xe1, 0x73, 0xc6, 0xaf, 0x24, 0x5b, 0x87, 0x66, 0x27, 0xf7, 0x57, 0xf4, 0x96, 0xb1, 0xb7, 0x5c, 0x8b, 0xd5,
  0x54, 0x79, 0xdf, 0xaa, 0xf6, 0x3e, 0xa3, 0xf1, 0x11, 0xca, 0xf5, 0xd1, 0x17, 0x7b, 0x93, 0x83, 0xbc, 0xbd, 0x52,
  0x1e, 0xeb, 0xae, 0xcc, 0xd6, 0x35, 0x08, 0xc8, 0x8a, 0xb4, 0xe2, 0xcd, 0xbf, 0xd9, 0xd0, 0x50, 0x59, 0x3f, 0x4d,
  0x62, 0x34, 0x0a, 0x48, 0x88, 0xb5, 0x56, 0x4c, 0x2e, 0x6b, 0x9e, 0xd2, 0x3d, 0x3c, 0x03, 0x13, 0xfb, 0x97, 0x51,
  0x75, 0x4a, 0x91, 0x71, 0x23, 0xbe, 0x76, 0x2a, 0x5f, 0xf9, 0xd4, 0x55, 0x0b, 0xdc, 0x37, 0x31, 0x16, 0x74, 0xd7,
  0x77, 0xa7, 0xe6, 0x07, 0xdb, 0xa4, 0x2f, 0x46, 0xf3, 0x61, 0x45, 0x67, 0xe3, 0x0c, 0xa2, 0x3b, 0x1c, 0x85, 0x18,
  0x04, 0x1d, 0x29, 0xa0, 0x8f, 0xb2, 0x5a, 0xd8, 0xa6, 0x7e, 0xee, 0x8d, 0x53, 0x4b, 0xa1, 0x9a, 0xc1, 0x0e, 0x7a,
  0x49, 0xa5, 0x2c, 0x81, 0xc4, 0xc7, 0x36, 0x2b, 0x7f, 0x43, 0x95, 0x33, 0xf2, 0x6c, 0x68, 0x6d, 0xf0, 0x02, 0x28,
  0xce, 0xdd, 0x9b, 0xea, 0x5e, 0x99, 0x7c, 0x14, 0x86, 0xcf, 0xe5, 0x42, 0xb8, 0x40, 0x78, 0x2d, 0x3a, 0xe9, 0x64,
  0x1f, 0x92, 0x90, 0x7d, 0x39, 0x6f, 0xe0, 0x89, 0x30
};

const uint8_t bias_function[16][16] = {
  { 0x46, 0x97, 0xb1, 0xba, 0xa3, 0xb7, 0x10, 0x0a, 0xc5, 0x37, 0xb3, 0xc9, 0x5a, 0x28, 0xac, 0x64 },
  { 0xec, 0xab, 0xaa, 0xc6, 0x67, 0x95, 0x58, 0x0d, 0xf8, 0x9a, 0xf6, 0x6e, 0x66, 0xdc, 0x05, 0x3d },
  { 0x8a, 0xc3, 0xd8, 0x89, 0x6a, 0xe9, 0x36, 0x49, 0x43, 0xbf, 0xeb, 0xd4, 0x96, 0x9b, 0x68, 0xa0 },
  { 0x5d, 0x57, 0x92, 0x1f, 0xd5, 0x71, 0x5c, 0xbb, 0x22, 0xc1, 0xbe, 0x7b, 0xbc, 0x99, 0x63, 0x94 },
  { 0x2a, 0x61, 0xb8, 0x34, 0x32, 0x19, 0xfd, 0xfb, 0x17, 0x40, 0xe6, 0x51, 0x1d, 0x41, 0x44, 0x8f },
  { 0xdd, 0x04, 0x80, 0xde, 0xe7, 0x31, 0xd6, 0x7f, 0x01, 0xa2, 0xf7, 0x39, 0xda, 0x6f, 0x23, 0xca },
  { 0x3a, 0xd0, 0x1c, 0xd1, 0x30, 0x3e, 0x12, 0xa1, 0xcd, 0x0f, 0xe0, 0xa8, 0xaf, 0x82, 0x59, 0x2c },
  { 0x7d, 0xad, 0xb2, 0xef, 0xc2, 0x87, 0xce, 0x75, 0x06, 0x13, 0x02, 0x90, 0x4f, 0x2e, 0x72, 0x33 },
  { 0xc0, 0x8d, 0xcf, 0xa9, 0x81, 0xe2, 0xc4, 0x27, 0x2f, 0x6c, 0x7a, 0x9f, 0x52, 0xe1, 0x15, 0x38 },
  { 0xfc, 0x20, 0x42, 0xc7, 0x08, 0xe4, 0x09, 0x55, 0x5e, 0x8c, 0x14, 0x76, 0x60, 0xff, 0xdf, 0xd7 },
  { 0xfa, 0x0b, 0x21, 0x00, 0x1a, 0xf9, 0xa6, 0xb9, 0xe8, 0x9e, 0x62, 0x4c, 0xd9, 0x91, 0x50, 0xd2 },
  { 0x18, 0xb4, 0x07, 0x84, 0xea, 0x5b, 0xa4, 0xc8, 0x0e, 0xcb, 0x48, 0x69, 0x4b, 0x4e, 0x9c, 0x35 },
  { 0x45, 0x4d, 0x54, 0xe5, 0x25, 0x3c, 0x0c, 0x4a, 0x8b, 0x3f, 0xcc, 0xa7, 0xdb, 0x6b, 0xae, 0xf4 },
  { 0x2d, 0xf3, 0x7c, 0x6d, 0x9d, 0xb5, 0x26, 0x74, 0xf2, 0x93, 0x53, 0xb0, 0xf0, 0x11, 0xed, 0x83 },
  { 0xb6, 0x03, 0x16, 0x73, 0x3b, 0x1e, 0x8e, 0x70, 0xbd, 0x86, 0x1b, 0x47, 0x7e, 0x24, 0x56, 0xf1 },
  { 0x88, 0x46, 0x97, 0xb1, 0xba, 0xa3, 0xb7, 0x10, 0x0a, 0xc5, 0x37, 0xb3, 0xc9, 0x5a, 0x28, 0xac }
};

void e21_algo(uint8_t *rand_num, uint8_t *bd_addr, uint8_t *output_data)
{
  uint8_t input2_to_ar_prime[INPUT_KEY_SIZE];

  /* Generating the first input */
  rand_num[15] ^= NUM_OF_BA_OCTETS;

  /* Generating the second input */
  memcpy(input2_to_ar_prime, bd_addr, NUM_OF_BA_OCTETS);
  memcpy(&input2_to_ar_prime[NUM_OF_BA_OCTETS], bd_addr, NUM_OF_BA_OCTETS);
  memcpy(&input2_to_ar_prime[2 * NUM_OF_BA_OCTETS], bd_addr, (INPUT_KEY_SIZE - 2 * NUM_OF_BA_OCTETS));

  ar_algo(rand_num, input2_to_ar_prime, AR_PRIME, output_data);
  rand_num[15] ^= NUM_OF_BA_OCTETS;
  return;
}

void e22_algo(uint8_t *rand_num, uint8_t *pin, uint8_t pin_length, uint8_t *bd_addr, uint8_t *output_data)
{
  uint8_t modified_pin[INPUT_DATA_BLOCK_SIZE];
  uint8_t new_pin_length;

  /* Modified PIN */
  if (pin_length < 16) {
    memcpy(modified_pin, pin, pin_length);
    if ((INPUT_DATA_BLOCK_SIZE - pin_length) < NUM_OF_BA_OCTETS) {
      memcpy(&modified_pin[pin_length], bd_addr, INPUT_DATA_BLOCK_SIZE - pin_length);
      new_pin_length = INPUT_DATA_BLOCK_SIZE;
    } else {
      memcpy(&modified_pin[pin_length], bd_addr, NUM_OF_BA_OCTETS);
      new_pin_length = pin_length + NUM_OF_BA_OCTETS;
      /* First input to AR_PRIME block */
      if ((new_pin_length << 1) < INPUT_DATA_BLOCK_SIZE) {
        memcpy(&modified_pin[new_pin_length], modified_pin, new_pin_length);
        memcpy(&modified_pin[(new_pin_length << 1)], modified_pin, INPUT_DATA_BLOCK_SIZE - (new_pin_length << 1));
      } else {
        memcpy(&modified_pin[new_pin_length], modified_pin, INPUT_DATA_BLOCK_SIZE - new_pin_length);
      }
    }
    pin = modified_pin;
  } else {
    new_pin_length = INPUT_DATA_BLOCK_SIZE;
  }
  rand_num[15] ^= new_pin_length;
  ar_algo(pin, rand_num, AR_PRIME, output_data);
  rand_num[15] ^= new_pin_length;
}

void hash_func(uint8_t *link_key, uint8_t *input1, uint8_t *input2, uint8_t input2_length, uint8_t *output_data)
{
  uint8_t intermediate_ip[INPUT_DATA_BLOCK_SIZE];
  uint8_t output_data1[INPUT_DATA_BLOCK_SIZE];
  uint8_t *k_prime = output_data1;
  uint8_t byte_position;

  memcpy(intermediate_ip, input2, input2_length);
  if (input2_length == NUM_OF_BA_OCTETS) {
    /* Getting a 16 byte input from BD_ADDR */
    memcpy(&intermediate_ip[NUM_OF_BA_OCTETS], input2, NUM_OF_BA_OCTETS);
    memcpy(&intermediate_ip[2 * NUM_OF_BA_OCTETS], input2, (INPUT_KEY_SIZE - 2 * NUM_OF_BA_OCTETS));
  } else {
    /* Padding input2 to INPUT_DATA_BLOCK_SIZE */
    memcpy(&intermediate_ip[NUM_OF_COF_OCTETS], input2, INPUT_KEY_SIZE - input2_length);
  }

  /* Perform first round of AR */
  ar_algo(link_key, input1, AR, output_data1);

  for (byte_position = 0; byte_position < INPUT_DATA_BLOCK_SIZE; byte_position++) {
    output_data1[byte_position] ^= input1[byte_position];
    intermediate_ip[byte_position] += output_data1[byte_position];
  }

  /* Generating second key from link key */
  k_prime[0]  = link_key[0] + 233;
  k_prime[1]  = link_key[1] ^ 229;
  k_prime[2]  = link_key[2] + 223;
  k_prime[3]  = link_key[3] ^ 193;
  k_prime[4]  = link_key[4] + 179;
  k_prime[5]  = link_key[5] ^ 167;
  k_prime[6]  = link_key[6] + 149;
  k_prime[7]  = link_key[7] ^ 131;
  k_prime[8]  = link_key[8] ^ 233;
  k_prime[9]  = link_key[9] + 229;
  k_prime[10] = link_key[10] ^ 223;
  k_prime[11] = link_key[11] + 193;
  k_prime[12] = link_key[12] ^ 179;
  k_prime[13] = link_key[13] + 167;
  k_prime[14] = link_key[14] ^ 149;
  k_prime[15] = link_key[15] + 131;

  ar_algo(k_prime, intermediate_ip, AR_PRIME, output_data);
}

void e3_algo(uint8_t *link_key, uint8_t *rand_num, uint8_t *cof, uint8_t *output_data)
{
  hash_func(link_key, rand_num, cof, NUM_OF_COF_OCTETS, output_data);
}

void e1_algo(uint8_t *link_key, uint8_t *rand_num, uint8_t *bd_addr, uint8_t *output_data)
{
  hash_func(link_key, rand_num, bd_addr, NUM_OF_BA_OCTETS, output_data);
}

void calculate_kc_prime_from_kc(uint16_t *kc, uint8_t key_size, uint16_t *kc_prime)
{
  (void)kc;
  (void)key_size;
  (void)kc_prime;
  //  TODO:
}

void f3_algo(uint8_t *w, uint8_t *n1, uint8_t *n2, uint8_t *r, uint8_t *io_cap, uint8_t *a1, uint8_t *a2, uint8_t *output_data)
{
#ifdef SCHED_SIM
  f3_algo_software(w, n1, n2, r, io_cap, a1, a2, output_data);
#else
  (void)w;
  (void)n1;
  (void)n2;
  (void)r;
  (void)io_cap;
  (void)a1;
  (void)a2;
  (void)output_data;
#endif
}
void g_algo(uint8_t *u, uint8_t *v, uint8_t *x, uint8_t *y, uint8_t *output_data)
{
#ifdef SCHED_SIM
  g_algo_software(u, v, x, y, output_data);
#else
  (void)u;
  (void)v;
  (void)x;
  (void)y;
  (void)output_data;
#endif
}

void f2_algo(uint8_t *w, uint8_t *n1, uint8_t *n2, uint8_t *key_id, uint8_t *a1, uint8_t *a2, uint8_t *output_data)
{
#ifdef SCHED_SIM
  f2_algo_software(w, n1, n2, key_id, a1, a2, output_data);
#else
  (void)w;
  (void)n1;
  (void)n2;
  (void)key_id;
  (void)a1;
  (void)a2;
  (void)output_data;
#endif
}

void ar_algo(uint8_t *original_key, uint8_t *original_data, uint8_t ar_type, uint8_t *output_data)
{
  uint8_t ip_data_copy1[INPUT_DATA_BLOCK_SIZE];
  uint8_t ip_data_copy2[INPUT_DATA_BLOCK_SIZE];
  uint8_t subkey[SUB_KEY_SIZE];
  uint8_t padded_key[PADDED_KEY_LENGTH];
  uint8_t bias_vector;
  uint8_t x, y;
  uint8_t *ip_data  = ip_data_copy1;
  uint8_t *op_data =  ip_data_copy2;
  uint8_t ip_byte_position;
  uint8_t byte_position, round_num;

  /* Preparing the inputs to start Ar */
  padded_key[16] = 0x0;
  for (byte_position = 0; byte_position < INPUT_KEY_SIZE; byte_position++) {
    padded_key[16] ^= original_key[byte_position];
    padded_key[byte_position] = original_key[byte_position];
  }

  memcpy(ip_data_copy1, original_data, INPUT_DATA_BLOCK_SIZE);

  /* 8 rounds of Ar */
  for (round_num = 1; round_num <= NUM_OF_ROUNDS; round_num++) {
    if (round_num & 0x1) {
      ip_data = ip_data_copy1;
      op_data = ip_data_copy2;
    } else {
      ip_data = ip_data_copy2;
      op_data = ip_data_copy1;
    }

    if (round_num != 1) {
      for (byte_position = 0; byte_position < SUB_KEY_SIZE; byte_position++) {
        /* First Sub key calculation */
        ip_byte_position = (byte_position + ((round_num << 1) - 2));
        if (ip_byte_position >= PADDED_KEY_LENGTH) {
          ip_byte_position -= PADDED_KEY_LENGTH;
        }
        /* Rotate each byte, 3 bits to the left */
        padded_key[ip_byte_position] = (padded_key[ip_byte_position] << 3) | (padded_key[ip_byte_position] >> 5);

        /* Bias vector calculation */
        bias_vector = bias_function[(round_num << 1) - 3][byte_position];

        subkey[byte_position] = padded_key[ip_byte_position] + bias_vector;
      }
      /* Rotate the remaining byte for next usage */
      ip_byte_position = (byte_position + ((round_num << 1) - 2));
      if (ip_byte_position >= PADDED_KEY_LENGTH) {
        ip_byte_position -= PADDED_KEY_LENGTH;
      }
      padded_key[ip_byte_position] = (padded_key[ip_byte_position] << 3) | (padded_key[ip_byte_position] >> 5);
    }

    for (byte_position = 0; byte_position < INPUT_DATA_BLOCK_SIZE; byte_position++) {
      if (((byte_position & 0x3) == 0x0) || ((byte_position & 0x3) == 0x3)) {
        if ((ar_type == AR_PRIME) && round_num == 3) {
          ip_data[byte_position] ^= original_data[byte_position];
        }
        if (round_num != 1) {
          ip_data[byte_position] ^= subkey[byte_position];
        } else {
          ip_data[byte_position] ^= padded_key[byte_position];
        }
        ip_data[byte_position] = e_box[ip_data[byte_position]];
      } else {
        if ((ar_type == AR_PRIME) && round_num == 3) {
          ip_data[byte_position] += original_data[byte_position];
        }
        if (round_num != 1) {
          ip_data[byte_position] += subkey[byte_position];
        } else {
          ip_data[byte_position] += padded_key[byte_position];
        }
        ip_data[byte_position] = l_box[ip_data[byte_position]];
      }
    }

    for (byte_position = 0; byte_position < SUB_KEY_SIZE; byte_position++) {
      /* Second Sub key calculation */
      ip_byte_position = (byte_position + (round_num << 1) - 1);
      if (ip_byte_position >= PADDED_KEY_LENGTH) {
        ip_byte_position -= PADDED_KEY_LENGTH;
      }
      /* Rotate each byte, 3 bits to the left */
      padded_key[ip_byte_position] = (padded_key[ip_byte_position] << 3) | (padded_key[ip_byte_position] >> 5);

      /* Bias vector calculation */
      bias_vector = bias_function[(round_num << 1) - 2][byte_position];

      subkey[byte_position] = padded_key[ip_byte_position] + bias_vector;
    }
    /* Rotate the remaining byte for next usage */
    ip_byte_position = (byte_position + (round_num << 1) - 1);
    if (ip_byte_position >= PADDED_KEY_LENGTH) {
      ip_byte_position -= PADDED_KEY_LENGTH;
    }
    padded_key[ip_byte_position] = (padded_key[ip_byte_position] << 3) | (padded_key[ip_byte_position] >> 5);

    for (byte_position = 0; byte_position < INPUT_DATA_BLOCK_SIZE; byte_position++) {
      if (((byte_position & 0x3) == 0) || ((byte_position & 0x3) == 3)) {
        ip_data[byte_position] += subkey[byte_position];
      } else {
        ip_data[byte_position] ^= subkey[byte_position];
      }
    }

    /* Computing all common rounds here */
    x = ip_data[8] + ip_data[9] + (ip_data[10] << 1) + ip_data[11] + (ip_data[12] << 1) + (ip_data[13] << 1)
        + (ip_data[14] << 3) + (ip_data[15] << 2);
    y = (ip_data[0] << 1) + ip_data[1] + ip_data[2] + ip_data[3] + (ip_data[4] << 2) + (ip_data[5] << 1) + ip_data[6]
        + ip_data[7];
    op_data[0] = (x << 1) + y;
    op_data[1] = x + y;

    x = ip_data[4] + ip_data[5] + (ip_data[6] << 1) + ip_data[7] + (ip_data[8] << 3) + (ip_data[9] << 2)
        + (ip_data[10] << 1) + (ip_data[11] << 1);
    y = ip_data[0] + ip_data[1] + (ip_data[2] << 2) + (ip_data[3] << 1) + (ip_data[12] << 1) + ip_data[13] + ip_data[14]
        + ip_data[15];
    op_data[2] = (x << 1) + y;
    op_data[3] = x + y;

    x = (ip_data[0] << 3) + (ip_data[1] << 2) + ip_data[2] + ip_data[3] + (ip_data[4] << 1) + ip_data[5]
        + (ip_data[6] << 1) + (ip_data[7] << 1);
    y = ip_data[8] + ip_data[9] + (ip_data[10] << 2) + (ip_data[11] << 1) + ip_data[12] + ip_data[13]
        + (ip_data[14] << 1) + ip_data[15];
    op_data[4] = (x << 1) + y;
    op_data[5] = x + y;

    x = ip_data[0] + ip_data[1] + (ip_data[2] << 1) + ip_data[3] + (ip_data[4] << 1) + (ip_data[5] << 1)
        + (ip_data[6] << 3) + (ip_data[7] << 2);
    y = (ip_data[8] << 1) + ip_data[9] + ip_data[10] + ip_data[11] + (ip_data[12] << 2) + (ip_data[13] << 1)
        + ip_data[14] + ip_data[15];
    op_data[6] = (x << 1) + y;
    op_data[7] = x + y;

    x = (ip_data[0] << 1) + ip_data[1] + (ip_data[2] << 1) + (ip_data[3] << 1) + (ip_data[4] << 3) + (ip_data[5] << 2)
        + ip_data[6] + ip_data[7];
    y = ip_data[8] + ip_data[9] + (ip_data[10] << 1) + ip_data[11] + ip_data[12] + ip_data[13] + (ip_data[14] << 2)
        + (ip_data[15] << 1);
    op_data[8] = (x << 1) + y;
    op_data[9] = x + y;

    x = (ip_data[0] << 1) + (ip_data[1] << 1) + (ip_data[2] << 3) + (ip_data[3] << 2) + (ip_data[12] << 1) + ip_data[13]
        + ip_data[14] + ip_data[15];
    y = ip_data[4] + ip_data[5] + (ip_data[6] << 1) + ip_data[7] + (ip_data[8] << 2) + (ip_data[9] << 1) + ip_data[10]
        + ip_data[11];
    op_data[10] = (x << 1) + y;
    op_data[11] = x + y;

    x = (ip_data[8] << 1) + (ip_data[9] << 1) + (ip_data[10] << 3) + (ip_data[11] << 2) + ip_data[12] + ip_data[13]
        + (ip_data[14] << 1) + ip_data[15];
    y = ip_data[0] + ip_data[1] + (ip_data[2] << 1) + ip_data[3] + ip_data[4] + ip_data[5] + (ip_data[6] << 2)
        + (ip_data[7] << 1);
    op_data[12] = (x << 1) + y;
    op_data[13] = x + y;

    x = (ip_data[8] << 1) + ip_data[9] + ip_data[10] + ip_data[11] + (ip_data[12] << 3) + (ip_data[13] << 2)
        + (ip_data[14] << 1) + (ip_data[15] << 1);
    y = (ip_data[0] << 2) + (ip_data[1] << 1) + ip_data[2] + ip_data[3] + (ip_data[4] << 1) + ip_data[5] + ip_data[6]
        + ip_data[7];
    op_data[14] = (x << 1) + y;
    op_data[15] = x + y;
  }

  /* Last Sub key calculation */
  for (byte_position = 0; byte_position < SUB_KEY_SIZE; byte_position++) {
    /* Rotate each byte, 3 bits to the left */
    ip_byte_position = (byte_position + 16);
    if (ip_byte_position >= PADDED_KEY_LENGTH) {
      ip_byte_position -= PADDED_KEY_LENGTH;
    }
    padded_key[ip_byte_position] = (padded_key[ip_byte_position] << 3) | (padded_key[ip_byte_position] >> 5);

    subkey[byte_position] = padded_key[ip_byte_position] + bias_function[15][byte_position];
  }

  for (byte_position = 0; byte_position < INPUT_DATA_BLOCK_SIZE; byte_position++) {
    if (((byte_position & 0x3) == 0) || ((byte_position & 0x3) == 3)) {
      output_data[byte_position] = (op_data[byte_position]) ^ subkey[byte_position];
    } else {
      output_data[byte_position] = op_data[byte_position] + subkey[byte_position];
    }
  }
}

void f1_algo(uint8_t *u, uint8_t *v, uint8_t *x, uint8_t z, uint8_t *output_data)
{
#ifdef SCHED_SIM
  f1_algo_software(u, v, x, z, output_data);
#else
  (void)u;
  (void)v;
  (void)x;
  (void)z;
  (void)output_data;
#endif
}