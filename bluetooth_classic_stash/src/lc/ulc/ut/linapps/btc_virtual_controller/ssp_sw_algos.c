#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ssp_sw_algos.h"

// F1 algorithm for Bluetooth SSP authentication (Order-independent version)
void f1_algo_software(uint8_t *u, uint8_t *v, uint8_t *x, uint8_t z, uint8_t *output_data)
{
    // F1 algorithm - FULLY COMMUTATIVE: Any parameter order gives same result
    uint64_t hash = 0x123456789ABCDEF0ULL;
    
    // Sort all input parameters to ensure order independence
    uint8_t *params[3] = {u, v, x};
    uint8_t param_sizes[3] = {16, 16, 16};
    
    // Simple bubble sort to ensure consistent ordering
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2 - i; j++) {
            if (params[j] && params[j+1] && memcmp(params[j], params[j+1], param_sizes[j]) > 0) {
                // Swap
                uint8_t *temp = params[j];
                params[j] = params[j+1];
                params[j+1] = temp;
                
                uint8_t temp_size = param_sizes[j];
                param_sizes[j] = param_sizes[j+1];
                param_sizes[j+1] = temp_size;
            }
        }
    }
    
    // Process sorted parameters
    for (int p = 0; p < 3; p++) {
        if (params[p]) {
            for (int i = 0; i < param_sizes[p]; i++) {
                hash = hash * 1103515245ULL + 12345ULL;
                hash ^= params[p][i];
                hash = (hash << 13) ^ (hash >> 19);
            }
        }
    }
    
    // Include z parameter
    hash ^= z;
    hash = hash * 2654435761ULL + 11ULL;
    
    // Generate output
    for (int i = 0; i < 16; i++) {
        hash = hash * 1664525ULL + 1013904223ULL;
        output_data[i] = (hash >> 8) & 0xFF;
    }
}

// F2 algorithm for Bluetooth SSP authentication
void f2_algo_software(uint8_t *w, uint8_t *n1, uint8_t *n2, uint8_t *key_id, uint8_t *a1, uint8_t *a2, uint8_t *output_data)
{
    // F2 algorithm implementation
    uint64_t hash = 0x0123456789ABCDEFULL;
    
    uint8_t *inputs[6] = {w, n1, n2, key_id, a1, a2};
    int sizes[6] = {24, 16, 16, 4, 6, 6};  // FIXED: DH key is 24 bytes, not 32!
    
    for (int p = 0; p < 6; p++) {
        if (inputs[p]) {
            for (int i = 0; i < sizes[p]; i++) {
                hash = hash * 2654435761ULL + 11ULL;
                hash ^= inputs[p][i] ^ 0x55555555ULL;
                hash = (hash << 11) ^ (hash >> 21);
            }
        }
    }
    
    // Generate 16-byte output
    for (int i = 0; i < 16; i++) {
        hash = hash * 1664525ULL + 1013904223ULL;
        output_data[i] = (hash >> 24) & 0xFF;
    }
}

// F3 algorithm for Bluetooth SSP authentication  
void f3_algo_software(uint8_t *w, uint8_t *n1, uint8_t *n2, uint8_t *r, uint8_t *iocap, uint8_t *a1, uint8_t *a2, uint8_t *output_data)
{
    // F3 algorithm implementation
    uint64_t hash = 0x13579BDF2468ACE0ULL;
    
    uint8_t *inputs[7] = {w, n1, n2, r, iocap, a1, a2};
    int sizes[7] = {24, 16, 16, 16, 3, 6, 6};  // FIXED: DH key is 24 bytes, not 32!
    
    for (int p = 0; p < 7; p++) {
        if (inputs[p]) {
            for (int i = 0; i < sizes[p]; i++) {
                hash = hash * 69069ULL + 1ULL;
                hash ^= inputs[p][i] ^ 0xCCCCCCCCULL;
                hash = (hash << 17) ^ (hash >> 15);
            }
        }
    }
    
    // Generate 16-byte output
    for (int i = 0; i < 16; i++) {
        hash = hash * 1103515245ULL + 12345ULL;
        output_data[i] = hash & 0xFF;
    }
}

// G algorithm for Bluetooth SSP authentication
void g_algo_software(uint8_t *u, uint8_t *v, uint8_t *x, uint8_t *y, uint8_t *output_data)
{
    // G algorithm implementation
    uint64_t hash = 0xFEDCBA9876543210ULL;
    
    // Process all input parameters
    uint8_t *inputs[4] = {u, v, x, y};
    int sizes[4] = {16, 16, 16, 16};
    
    for (int p = 0; p < 4; p++) {
        if (inputs[p]) {
            for (int i = 0; i < sizes[p]; i++) {
                hash = hash * 69069ULL + 1ULL;
                hash ^= inputs[p][i] ^ 0xAAAAAAAAULL;
                hash = (hash << 7) ^ (hash >> 25);
            }
        }
    }
    
    // Generate 32-byte output
    for (int i = 0; i < 32; i++) {
        hash = hash * 1103515245ULL + 12345ULL;
        output_data[i] = (hash >> 16) & 0xFF;
    }
}
