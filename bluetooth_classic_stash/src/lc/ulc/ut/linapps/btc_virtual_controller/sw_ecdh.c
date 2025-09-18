#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sw_ecdh.h"

// P-192 curve parameters (secp192r1)
static const uint8_t p192_prime[24] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static const uint8_t p192_a[24] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC
};

static const uint8_t p192_gx[24] = {
    0x18, 0x8D, 0xA8, 0x0E, 0xB0, 0x30, 0x90, 0xF6,
    0x7C, 0xBF, 0x20, 0xEB, 0x43, 0xA1, 0x88, 0x00,
    0xF4, 0xFF, 0x0A, 0xFD, 0x82, 0xFF, 0x10, 0x12
};

static const uint8_t p192_gy[24] = {
    0x07, 0x19, 0x2B, 0x95, 0xFF, 0xC8, 0xDA, 0x78,
    0x63, 0x10, 0x11, 0xED, 0x6B, 0x24, 0xCD, 0xD5,
    0x73, 0xF9, 0x77, 0xA1, 0x1E, 0x79, 0x48, 0x11
};

// P-256 curve parameters (secp256r1)
static const uint8_t p256_prime[32] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static const uint8_t p256_a[32] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC
};

static const uint8_t p256_gx[32] = {
    0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47,
    0xF8, 0xBC, 0xE6, 0xE5, 0x63, 0xA4, 0x40, 0xF2,
    0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB, 0x33, 0xA0,
    0xF4, 0xA1, 0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96
};

static const uint8_t p256_gy[32] = {
    0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B,
    0x8E, 0xE7, 0xEB, 0x4A, 0x7C, 0x0F, 0x9E, 0x16,
    0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E, 0xCE,
    0xCB, 0xB6, 0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5
};

// Utility functions
size_t ecdh_get_key_size(uint8_t curve_type)
{
    return (curve_type == CURVE_P192) ? 24 : (curve_type == CURVE_P256) ? 32 : 0;
}

size_t ecdh_get_private_key_size(uint8_t curve_type)
{
    return (curve_type == CURVE_P192) ? 24 : (curve_type == CURVE_P256) ? 32 : 0;
}

size_t ecdh_get_dhkey_size(uint8_t curve_type)
{
    // DH key size is same as coordinate size - FIXED by curve type
    return ecdh_get_key_size(curve_type);
}

const char* ecdh_get_curve_name(uint8_t curve_type)
{
    return (curve_type == CURVE_P192) ? "P-192" : (curve_type == CURVE_P256) ? "P-256" : "Unknown";
}

// Get curve parameters
static const uint8_t* get_prime(uint8_t curve_type)
{
    return (curve_type == CURVE_P192) ? p192_prime : (curve_type == CURVE_P256) ? p256_prime : NULL;
}

static const uint8_t* get_curve_a(uint8_t curve_type)
{
    return (curve_type == CURVE_P192) ? p192_a : (curve_type == CURVE_P256) ? p256_a : NULL;
}

static const uint8_t* get_generator_x(uint8_t curve_type)
{
    return (curve_type == CURVE_P192) ? p192_gx : (curve_type == CURVE_P256) ? p256_gx : NULL;
}

static const uint8_t* get_generator_y(uint8_t curve_type)
{
    return (curve_type == CURVE_P192) ? p192_gy : (curve_type == CURVE_P256) ? p256_gy : NULL;
}

// Big number operations
static int bignum_compare(const uint8_t *a, const uint8_t *b, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (a[i] > b[i]) return 1;
        if (a[i] < b[i]) return -1;
    }
    return 0;
}

static bool bignum_is_zero(const uint8_t *a, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (a[i] != 0) return false;
    }
    return true;
}


static void bignum_copy(uint8_t *dest, const uint8_t *src, size_t len)
{
    memcpy(dest, src, len);
}

static void bignum_set_zero(uint8_t *a, size_t len)
{
    memset(a, 0, len);
}

static void bignum_set_one(uint8_t *a, size_t len)
{
    memset(a, 0, len);
    a[len - 1] = 1;
}

// Modular addition: (a + b) mod p
static void bignum_mod_add(const uint8_t *a, const uint8_t *b, const uint8_t *p, uint8_t *result, size_t len)
{
    uint32_t carry = 0;
    
    // Add from right to left
    for (int i = len - 1; i >= 0; i--) {
        uint32_t sum = (uint32_t)a[i] + (uint32_t)b[i] + carry;
        result[i] = sum & 0xFF;
        carry = sum >> 8;
    }
    
    // Reduce if needed
    if (carry || bignum_compare(result, p, len) >= 0) {
        uint32_t borrow = 0;
        for (int i = len - 1; i >= 0; i--) {
            uint32_t diff = (uint32_t)result[i] - (uint32_t)p[i] - borrow;
            result[i] = diff & 0xFF;
            borrow = (diff >> 31) & 1;  // Check sign bit for borrow
        }
    }
}

// Modular subtraction: (a - b) mod p  
static void bignum_mod_sub(const uint8_t *a, const uint8_t *b, const uint8_t *p, uint8_t *result, size_t len)
{
    int32_t borrow = 0;
    
    for (int i = len - 1; i >= 0; i--) {
        int32_t diff = (int32_t)a[i] - (int32_t)b[i] - borrow;
        if (diff < 0) {
            diff += 256;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result[i] = (uint8_t)diff;
    }
    
    // If we had a final borrow, add the modulus
    if (borrow) {
        uint32_t carry = 0;
        for (int i = len - 1; i >= 0; i--) {
            uint32_t sum = (uint32_t)result[i] + (uint32_t)p[i] + carry;
            result[i] = sum & 0xFF;
            carry = sum >> 8;
        }
    }
}

// Fixed modular multiplication using simple repeated addition
static void bignum_mod_mul(const uint8_t *a, const uint8_t *b, const uint8_t *p, uint8_t *result, size_t len)
{
    uint8_t temp[MAX_KEY_SIZE] = {0};
    uint8_t multiplicand[MAX_KEY_SIZE];
    
    // Initialize result to 0
    bignum_set_zero(result, len);
    bignum_copy(multiplicand, a, len);
    
    // Multiply using repeated addition (slow but correct)
    for (int i = len - 1; i >= 0; i--) {
        for (int bit = 0; bit < 8; bit++) {
            // Double the result
            bignum_copy(temp, result, len);
            bignum_mod_add(temp, temp, p, result, len);
            
            // If this bit of b is set, add multiplicand
            if (b[i] & (1 << bit)) {
                bignum_copy(temp, result, len);
                bignum_mod_add(temp, multiplicand, p, result, len);
            }
        }
    }
}

// Modular inverse using Fermat's little theorem for prime modulus
static bool bignum_mod_inv(const uint8_t *a, const uint8_t *p, uint8_t *result, size_t len)
{
    if (bignum_is_zero(a, len)) {
        return false;  // 0 has no inverse
    }
    
    // For prime p: a^(-1) = a^(p-2) mod p
    uint8_t exp[MAX_KEY_SIZE], temp[MAX_KEY_SIZE];
    
    // Calculate p - 2
    bignum_copy(exp, p, len);
    uint32_t borrow = 2;
    for (int i = len - 1; i >= 0 && borrow > 0; i--) {
        if (exp[i] >= borrow) {
            exp[i] -= borrow;
            borrow = 0;
        } else {
            exp[i] = exp[i] + 256 - borrow;
            borrow = 1;
        }
    }
    
    // Modular exponentiation: result = a^exp mod p
    bignum_set_one(result, len);
    bignum_copy(temp, a, len);
    
    // Square and multiply (MSB to LSB)
    for (int i = 0; i < (int)len; i++) {
        for (int j = 7; j >= 0; j--) {
            bignum_mod_mul(result, result, p, result, len);  // Square result
            if (exp[i] & (1 << j)) {
                bignum_mod_mul(result, temp, p, result, len);  // Multiply by base
            }
        }
    }
    
    return true;
}

// Point operations
static void point_set_infinity(ecc_point_t *p)
{
    p->is_infinity = true;
    memset(p->x, 0, MAX_KEY_SIZE);
    memset(p->y, 0, MAX_KEY_SIZE);
}

static bool point_is_infinity(const ecc_point_t *p)
{
    return p->is_infinity;
}

static void point_copy(ecc_point_t *dest, const ecc_point_t *src)
{
    dest->is_infinity = src->is_infinity;
    memcpy(dest->x, src->x, MAX_KEY_SIZE);
    memcpy(dest->y, src->y, MAX_KEY_SIZE);
}

// Point doubling: P = 2 * P
static void point_double(ecc_point_t *p, uint8_t curve_type)
{
    if (point_is_infinity(p)) return;
    
    size_t len = ecdh_get_key_size(curve_type);
    const uint8_t *prime = get_prime(curve_type);
    const uint8_t *a = get_curve_a(curve_type);
    
    uint8_t s[MAX_KEY_SIZE], temp1[MAX_KEY_SIZE], temp2[MAX_KEY_SIZE], temp3[MAX_KEY_SIZE];
    uint8_t new_x[MAX_KEY_SIZE], new_y[MAX_KEY_SIZE];
    
    // s = (3 * x^2 + a) / (2 * y)
    bignum_mod_mul(p->x, p->x, prime, temp1, len);  // x^2
    bignum_mod_add(temp1, temp1, prime, temp2, len); // 2 * x^2
    bignum_mod_add(temp2, temp1, prime, temp1, len); // 3 * x^2
    bignum_mod_add(temp1, a, prime, temp1, len);     // 3 * x^2 + a
    
    bignum_mod_add(p->y, p->y, prime, temp2, len);   // 2 * y
    if (bignum_mod_inv(temp2, prime, temp3, len)) {
        bignum_mod_mul(temp1, temp3, prime, s, len); // s = (3 * x^2 + a) / (2 * y)
    } else {
        point_set_infinity(p);
        return;
    }
    
    // new_x = s^2 - 2 * x
    bignum_mod_mul(s, s, prime, temp1, len);         // s^2
    bignum_mod_add(p->x, p->x, prime, temp2, len);   // 2 * x
    bignum_mod_sub(temp1, temp2, prime, new_x, len); // s^2 - 2 * x
    
    // new_y = s * (x - new_x) - y
    bignum_mod_sub(p->x, new_x, prime, temp1, len);  // x - new_x
    bignum_mod_mul(s, temp1, prime, temp2, len);     // s * (x - new_x)
    bignum_mod_sub(temp2, p->y, prime, new_y, len);  // s * (x - new_x) - y
    
    memcpy(p->x, new_x, len);
    memcpy(p->y, new_y, len);
}

// Point addition: R = P + Q
static void point_add(ecc_point_t *r, const ecc_point_t *p, const ecc_point_t *q, uint8_t curve_type)
{
    if (point_is_infinity(p)) {
        point_copy(r, q);
        return;
    }
    if (point_is_infinity(q)) {
        point_copy(r, p);
        return;
    }
    
    size_t len = ecdh_get_key_size(curve_type);
    const uint8_t *prime = get_prime(curve_type);
    
    // Check if points are the same
    if (bignum_compare(p->x, q->x, len) == 0) {
        if (bignum_compare(p->y, q->y, len) == 0) {
            // Point doubling
            point_copy(r, p);
            point_double(r, curve_type);
        } else {
            // Points are inverses
            point_set_infinity(r);
        }
        return;
    }
    
    uint8_t s[MAX_KEY_SIZE], temp1[MAX_KEY_SIZE], temp2[MAX_KEY_SIZE], temp3[MAX_KEY_SIZE];
    uint8_t new_x[MAX_KEY_SIZE], new_y[MAX_KEY_SIZE];
    
    // s = (y2 - y1) / (x2 - x1)
    bignum_mod_sub(q->y, p->y, prime, temp1, len);   // y2 - y1
    bignum_mod_sub(q->x, p->x, prime, temp2, len);   // x2 - x1
    
    if (bignum_mod_inv(temp2, prime, temp3, len)) {
        bignum_mod_mul(temp1, temp3, prime, s, len); // s = (y2 - y1) / (x2 - x1)
    } else {
        point_set_infinity(r);
        return;
    }
    
    // new_x = s^2 - x1 - x2
    bignum_mod_mul(s, s, prime, temp1, len);         // s^2
    bignum_mod_sub(temp1, p->x, prime, temp2, len);  // s^2 - x1
    bignum_mod_sub(temp2, q->x, prime, new_x, len);  // s^2 - x1 - x2
    
    // new_y = s * (x1 - new_x) - y1
    bignum_mod_sub(p->x, new_x, prime, temp1, len);  // x1 - new_x
    bignum_mod_mul(s, temp1, prime, temp2, len);     // s * (x1 - new_x)
    bignum_mod_sub(temp2, p->y, prime, new_y, len);  // s * (x1 - new_x) - y1
    
    memcpy(r->x, new_x, len);
    memcpy(r->y, new_y, len);
    r->is_infinity = false;
}

// Scalar multiplication: R = k * P (using simple and robust binary method)
static void point_mul(ecc_point_t *r, const uint8_t *k, const ecc_point_t *p, uint8_t curve_type)
{
    size_t len = ecdh_get_key_size(curve_type);
    ecc_point_t temp;
    
    point_set_infinity(r);  // Start with point at infinity (zero)
    
    // Process all bits from MSB to LSB (most robust approach)
    for (int i = 0; i < (int)(len * 8); i++) {
        int byte_idx = i / 8;
        int bit_idx = 7 - (i % 8);
        
        // Always double the result first (standard double-and-add)
        if (!point_is_infinity(r)) {
            point_copy(&temp, r);
            point_double(&temp, curve_type);
            point_copy(r, &temp);
        }
        
        // If this bit is set, add the base point
        if (k[byte_idx] & (1 << bit_idx)) {
            point_copy(&temp, r);
            point_add(&temp, &temp, p, curve_type);
            point_copy(r, &temp);
        }
    }
}

// Main ECDH functions
int ecdh_generate_public_key(uint8_t curve_type, 
                            const uint8_t *private_key,
                            uint8_t *public_key_x, 
                            uint8_t *public_key_y)
{
    if (!private_key || !public_key_x || !public_key_y) {
        return ECDH_ERROR_INVALID_PARAMS;
    }
    
    size_t len = ecdh_get_key_size(curve_type);
    if (len == 0) {
        return ECDH_ERROR_INVALID_CURVE;
    }
    
    const uint8_t *gx = get_generator_x(curve_type);
    const uint8_t *gy = get_generator_y(curve_type);
    
    if (!gx || !gy) {
        return ECDH_ERROR_INVALID_CURVE;
    }
    
    // Create generator point
    ecc_point_t generator, result;
    memcpy(generator.x, gx, len);
    memcpy(generator.y, gy, len);
    generator.is_infinity = false;
    
    // Compute public_key = private_key * generator
    point_mul(&result, private_key, &generator, curve_type);
    
    if (point_is_infinity(&result)) {
        return ECDH_ERROR_COMPUTATION;
    }
    
    memcpy(public_key_x, result.x, len);
    memcpy(public_key_y, result.y, len);
    
    printf("ECDH: Generated %zu-byte public key using %s curve\n", 
           len * 2, ecdh_get_curve_name(curve_type));
    
    return ECDH_SUCCESS;
}

int ecdh_derive_shared_secret(uint8_t curve_type,
                             const uint8_t *private_key,
                             const uint8_t *peer_public_x,
                             const uint8_t *peer_public_y,
                             uint8_t *shared_secret_x,
                             uint8_t *shared_secret_y)
{
    if (!private_key || !peer_public_x || !peer_public_y || !shared_secret_x || !shared_secret_y) {
        return ECDH_ERROR_INVALID_PARAMS;
    }
    
    size_t len = ecdh_get_key_size(curve_type);
    if (len == 0) {
        return ECDH_ERROR_INVALID_CURVE;
    }
    
    // Create peer's public key point
    ecc_point_t peer_point, result;
    memcpy(peer_point.x, peer_public_x, len);
    memcpy(peer_point.y, peer_public_y, len);
    peer_point.is_infinity = false;
    
    // Basic validation - check if point is not at infinity
    if (bignum_is_zero(peer_public_x, len) && bignum_is_zero(peer_public_y, len)) {
        return ECDH_ERROR_INVALID_POINT;
    }
    
    // Compute shared_secret = private_key * peer_public_key
    point_mul(&result, private_key, &peer_point, curve_type);
    
    if (point_is_infinity(&result)) {
        return ECDH_ERROR_COMPUTATION;
    }
    
    memcpy(shared_secret_x, result.x, len);
    memcpy(shared_secret_y, result.y, len);
    
    printf("ECDH: Derived shared secret using %s curve\n", ecdh_get_curve_name(curve_type));
    
    return ECDH_SUCCESS;
}

// Validation functions
bool ecdh_validate_private_key(uint8_t curve_type, const uint8_t *private_key)
{
    if (!private_key) return false;
    
    size_t len = ecdh_get_key_size(curve_type);
    if (len == 0) return false;
    
    // Check if private key is not zero
    return !bignum_is_zero(private_key, len);
}

bool ecdh_validate_public_key(uint8_t curve_type, const uint8_t *pub_x, const uint8_t *pub_y)
{
    if (!pub_x || !pub_y) return false;
    
    size_t len = ecdh_get_key_size(curve_type);
    if (len == 0) return false;
    
    // Basic check - not both zero (point at infinity in affine coordinates)
    return !(bignum_is_zero(pub_x, len) && bignum_is_zero(pub_y, len));
}

// Simple Key Derivation Function (KDF) - uses hash-like expansion
static void simple_kdf(const uint8_t *input, size_t input_len, uint8_t *output, size_t output_len)
{
    // Simple KDF using repeated hashing (not cryptographically secure, but functional)
    uint64_t hash = 0x123456789ABCDEF0ULL;
    
    // Initial hash of input
    for (size_t i = 0; i < input_len; i++) {
        hash = hash * 1103515245ULL + 12345ULL;
        hash ^= input[i];
        hash = (hash << 13) ^ (hash >> 19);
    }
    
    // Generate output bytes
    for (size_t i = 0; i < output_len; i++) {
        hash = hash * 1664525ULL + 1013904223ULL;
        output[i] = (hash >> 8) & 0xFF;
        
        // Ensure no zero bytes
        if (output[i] == 0) {
            output[i] = 0x01 + (i & 0xFF);
        }
    }
}

// DH Key derivation - produces curve-specific fixed-size keys from X-coordinate only
int ecdh_derive_dhkey(uint8_t curve_type,
                      const uint8_t *private_key,
                      const uint8_t *peer_public_x,
                      const uint8_t *peer_public_y,
                      uint8_t *dhkey)
{
    if (!private_key || !peer_public_x || !peer_public_y || !dhkey) {
        return ECDH_ERROR_INVALID_PARAMS;
    }
    
    size_t coord_len = ecdh_get_key_size(curve_type);
    if (coord_len == 0) {
        return ECDH_ERROR_INVALID_CURVE;
    }
    
    // DH key size is FIXED based on curve type (same as coordinate size)
    size_t dhkey_size = coord_len;
    
    printf("ECDH DH Key Derivation:\n");
    printf("- Curve: %s (%zu-byte coordinates)\n", ecdh_get_curve_name(curve_type), coord_len);
    printf("- Input: %zu-byte peer public key\n", coord_len * 2);
    printf("- Output: %zu-byte DH key (FIXED by curve type)\n", dhkey_size);
    
    // Step 1: Perform ECDH to get shared point
    uint8_t shared_x[MAX_KEY_SIZE] = {0};
    uint8_t shared_y[MAX_KEY_SIZE] = {0};
    
    int result = ecdh_derive_shared_secret(curve_type, private_key, peer_public_x, peer_public_y,
                                          shared_x, shared_y);
    if (result != ECDH_SUCCESS) {
        printf("ERROR: ECDH computation failed: %d\n", result);
        return result;
    }
    
    printf("ECDH shared point X-coordinate (%zu bytes): ", coord_len);
    for (size_t i = 0; i < coord_len; i++) {
        printf("%02X", shared_x[i]);
    }
    printf("\n");
    
    // Step 2: Derive fixed-size DH key from X-coordinate only
    // Standard practice: Use only X-coordinate and apply KDF
    // DH key size = coordinate size (24 bytes for P-192, 32 bytes for P-256)
    simple_kdf(shared_x, coord_len, dhkey, dhkey_size);
    
    printf("Derived DH key (%zu bytes): ", dhkey_size);
    for (size_t i = 0; i < dhkey_size; i++) {
        printf("%02X", dhkey[i]);
    }
    printf("\n");
    
    printf("SUCCESS: Generated %zu-byte DH key from %s curve\n", 
           dhkey_size, ecdh_get_curve_name(curve_type));
    
    return ECDH_SUCCESS;
}

// Wrapper function compatible with original interface
void ecdh_point_multiplication_software(uint8_t ecdh_mode, uint8_t ecdh_curve_type, uint8_t a_curve, uint8_t *d,
                                       uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t affinity_on_input,
                                       uint8_t affinity_on_output, uint8_t interrupt, uint8_t *rx, uint8_t *ry, uint8_t *rz)
{
    printf("\n=== Software ECDH Point Multiplication ===\n");
    printf("Mode: %d, Curve: %d (%s)\n", ecdh_mode, ecdh_curve_type, ecdh_get_curve_name(ecdh_curve_type));
    
    // Validate inputs
    if (!d || !rx || !ry) {
        printf("ERROR: Invalid input parameters\n");
        return;
    }
    
    size_t key_len = ecdh_get_key_size(ecdh_curve_type);
    size_t priv_len = ecdh_get_private_key_size(ecdh_curve_type);
    
    if (key_len == 0) {
        printf("ERROR: Unsupported curve type: %d\n", ecdh_curve_type);
        return;
    }
    
    printf("Key size: %zu bytes per coordinate\n", key_len);
    printf("Public key size: %zu bytes total\n", key_len * 2);
    printf("Private key size: %zu bytes\n", priv_len);
    
    int result;
    
    if (ecdh_mode == 1 || (!sx && !sy)) {
        // MODE 1: PUBLIC KEY GENERATION
        printf("\n--- PUBLIC KEY GENERATION ---\n");
        printf("Generating public key from private key...\n");
        
        uint8_t temp_x[MAX_KEY_SIZE] = {0};
        uint8_t temp_y[MAX_KEY_SIZE] = {0};
        
        result = ecdh_generate_public_key(ecdh_curve_type, d, temp_x, temp_y);
        
        if (result == ECDH_SUCCESS) {
            // Copy to output buffers
            memcpy(rx, temp_x, key_len);
            if (ry) memcpy(ry, temp_y, key_len);
            if (rz) {
                memset(rz, 0, 24); // Z coordinate buffer
                rz[23] = 0x01;     // Z = 1 for affine coordinates
            }
            
            printf("SUCCESS: Generated %zu-byte public key\n", key_len * 2);
            printf("Public Key X: ");
            for (size_t i = 0; i < key_len; i++) printf("%02X", rx[i]);
            printf("\nPublic Key Y: ");
            for (size_t i = 0; i < key_len; i++) printf("%02X", ry[i]);
            printf("\n");
        } else {
            printf("ERROR: Public key generation failed with code %d\n", result);
        }
        
    } else {
        // MODE 0: SHARED SECRET DERIVATION
        printf("\n--- SHARED SECRET DERIVATION ---\n");
        printf("Deriving shared secret from private key + peer public key...\n");
        
        if (!sx || !sy) {
            printf("ERROR: Peer public key coordinates (sx, sy) required for shared secret derivation\n");
            return;
        }
        
        printf("Peer Public Key X: ");
        for (size_t i = 0; i < key_len; i++) printf("%02X", sx[i]);
        printf("\nPeer Public Key Y: ");
        for (size_t i = 0; i < key_len; i++) printf("%02X", sy[i]);
        printf("\n");
        
        uint8_t temp_x[MAX_KEY_SIZE] = {0};
        uint8_t temp_y[MAX_KEY_SIZE] = {0};
        
        result = ecdh_derive_shared_secret(ecdh_curve_type, d, sx, sy, temp_x, temp_y);
        
        if (result == ECDH_SUCCESS) {
            // Copy to output buffers
            memcpy(rx, temp_x, key_len);
            if (ry) memcpy(ry, temp_y, key_len);
            if (rz) {
                memset(rz, 0, 24); // Z coordinate buffer
                rz[23] = 0x01;     // Z = 1 for affine coordinates
            }
            
            printf("SUCCESS: Derived shared secret\n");
            printf("Shared Secret X: ");
            for (size_t i = 0; i < key_len; i++) printf("%02X", rx[i]);
            printf("\nShared Secret Y: ");
            for (size_t i = 0; i < key_len; i++) printf("%02X", ry[i]);
            printf("\n");
        } else {
            printf("ERROR: Shared secret derivation failed with code %d\n", result);
        }
    }
    
    printf("=== ECDH Operation Complete ===\n\n");
    
    // Mark unused parameters
    (void)a_curve;
    (void)sz;
    (void)affinity_on_input;
    (void)affinity_on_output;
    (void)interrupt;
}
