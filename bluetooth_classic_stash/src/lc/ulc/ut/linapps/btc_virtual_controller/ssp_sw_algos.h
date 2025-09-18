#ifndef SSP_SW_ALGOS_H
#define SSP_SW_ALGOS_H

#include <stdint.h>

// F1 algorithm for Bluetooth SSP authentication
void f1_algo_software(uint8_t *u, uint8_t *v, uint8_t *x, uint8_t z, uint8_t *output_data);

// F2 algorithm for Bluetooth SSP authentication
void f2_algo_software(uint8_t *w, uint8_t *n1, uint8_t *n2, uint8_t *key_id, uint8_t *a1, uint8_t *a2, uint8_t *output_data);

// F3 algorithm for Bluetooth SSP authentication
void f3_algo_software(uint8_t *w, uint8_t *n1, uint8_t *n2, uint8_t *r, uint8_t *io_cap, uint8_t *a1, uint8_t *a2, uint8_t *output_data);

// G algorithm for Bluetooth SSP authentication
void g_algo_software(uint8_t *u, uint8_t *v, uint8_t *x, uint8_t *y, uint8_t *output_data);

#endif // SSP_SW_ALGOS_H
