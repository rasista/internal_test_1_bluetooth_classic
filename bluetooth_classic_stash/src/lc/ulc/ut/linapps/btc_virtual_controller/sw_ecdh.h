#ifndef SW_ECDH_H
#define SW_ECDH_H

#include <stdint.h>
#include <stdbool.h>

// Curve types
#define CURVE_P192    0    // P-192: 24-byte coordinates, 48-byte public keys
#define CURVE_P256    1    // P-256: 32-byte coordinates, 64-byte public keys

// Maximum sizes
#define MAX_KEY_SIZE  32   // Maximum coordinate size (P-256)
#define MAX_PUBKEY_SIZE 64 // Maximum public key size (P-256: 32+32)

// Return codes
#define ECDH_SUCCESS          0
#define ECDH_ERROR_INVALID_PARAMS -1
#define ECDH_ERROR_INVALID_CURVE  -2
#define ECDH_ERROR_INVALID_POINT  -3
#define ECDH_ERROR_COMPUTATION    -4

// Elliptic curve point structure
typedef struct {
    uint8_t x[MAX_KEY_SIZE];
    uint8_t y[MAX_KEY_SIZE];
    bool is_infinity;
} ecc_point_t;

// Big number structure for internal calculations
typedef struct {
    uint8_t data[MAX_KEY_SIZE];
    size_t size;
} bignum_t;

// Main ECDH functions
int ecdh_generate_public_key(uint8_t curve_type, 
                            const uint8_t *private_key,
                            uint8_t *public_key_x, 
                            uint8_t *public_key_y);

int ecdh_derive_shared_secret(uint8_t curve_type,
                             const uint8_t *private_key,
                             const uint8_t *peer_public_x,
                             const uint8_t *peer_public_y,
                             uint8_t *shared_secret_x,
                             uint8_t *shared_secret_y);

// Utility functions
size_t ecdh_get_key_size(uint8_t curve_type);
size_t ecdh_get_private_key_size(uint8_t curve_type);
size_t ecdh_get_dhkey_size(uint8_t curve_type);  // Same as coordinate size
const char* ecdh_get_curve_name(uint8_t curve_type);

// Test/validation functions
bool ecdh_validate_private_key(uint8_t curve_type, const uint8_t *private_key);
bool ecdh_validate_public_key(uint8_t curve_type, const uint8_t *pub_x, const uint8_t *pub_y);

// DH Key derivation - produces curve-specific fixed-size keys from X-coordinate only  
// CURVE_P192 (0) → 24-byte DH key
// CURVE_P256 (1) → 32-byte DH key
int ecdh_derive_dhkey(uint8_t curve_type,
                      const uint8_t *private_key,
                      const uint8_t *peer_public_x,
                      const uint8_t *peer_public_y,
                      uint8_t *dhkey);

// Wrapper function compatible with original interface
void ecdh_point_multiplication_software(uint8_t ecdh_mode, uint8_t ecdh_curve_type, uint8_t a_curve, uint8_t *d,
                                       uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t affinity_on_input,
                                       uint8_t affinity_on_output, uint8_t interrupt, uint8_t *rx, uint8_t *ry, uint8_t *rz);

#endif // SW_ECDH_H
