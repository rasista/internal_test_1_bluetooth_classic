
#ifndef _BT_KEY_MGMT_ALGOS_H
#define _BT_KEY_MGMT_ALGOS_H


#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"


void e1_algo(uint8_t *link_key, uint8_t *rand_num, uint8_t *bd_addr, uint8_t *output_data);
void e21_algo(uint8_t *rand_num, uint8_t *bd_addr, uint8_t *output_data);
void e22_algo(uint8_t *rand, uint8_t *pin, uint8_t pin_length, uint8_t *bd_addr, uint8_t *output_data);
void e3_algo(uint8_t *key, uint8_t *rand, uint8_t *aco, uint8_t *output_data);
void ar_algo(uint8_t *original_key, uint8_t *original_data, uint8_t ar_type, uint8_t *output_data);
void hash_func(uint8_t *link_key, uint8_t *input1, uint8_t *input2, uint8_t input2_length, uint8_t *output_data);
void calculate_kc_prime_from_kc(uint16_t *kc, uint8_t key_size, uint16_t *kc_prime);
void f3_algo(uint8_t *w, uint8_t *n1, uint8_t *n2, uint8_t *r, uint8_t *io_cap, uint8_t *a1, uint8_t *a2, uint8_t *output_data);
void g_algo(uint8_t *u, uint8_t *v, uint8_t *x, uint8_t *y, uint8_t *output_data);
void f2_algo(uint8_t *w, uint8_t *n1, uint8_t *n2, uint8_t *key_id, uint8_t *a1, uint8_t *a2, uint8_t *output_data);
void f1_algo(uint8_t *u, uint8_t *v, uint8_t *x, uint8_t z, uint8_t *output_data);
#endif