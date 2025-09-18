#include "access_code_gen.h"

/**
 * @brief   This function implements polynomial modulo division
 * @param   dividend polynomial
 * @param   divisor polynomial
 * @param   remainder polynomial address pointer where the result is to be stored
 * @return  void
 */
void lc_polynom_modulo(uint32_t *dividend, uint32_t *divisor, uint32_t *remainder)
{
  uint8_t i;

  i = 64 - 35 + 1; // represents the number of bits to process,sync words use (64,30) optimized block code
  while (i--) {
    if (dividend[1] & 0x80000000) {
      dividend[1] = dividend[1] ^ divisor[1];
      dividend[0] = dividend[0] ^ divisor[0];
    }
    dividend[1] = dividend[1] << 1;
    if (dividend[0] & 0x80000000) {
      dividend[1] |= 0x1;
    }
    dividend[0] = dividend[0] << 1;
  }
  remainder[1] = dividend[1];
  remainder[0] = dividend[0];

  return;
}

/**
 * @brief   This function synthesizes the access code
 * @param   Input address which contains both LAP and UAP out of which only LAP is used
 * @param   Resultant access code address pointer
 * @return  void
 */
void access_code_synth(uint32_t *lap_addr, uint32_t *bt_acc_code)
{
  uint32_t a;
  uint32_t b0;
  uint32_t b1;
  uint32_t x;
  uint32_t x_bar[2];
  uint32_t gen_divisor[2];
  uint32_t c_bar[2];

  // Initialising values

  /*PN sequence
     64 bit full length pseudo-random noise (PN) sequence improves the auto correlation properties of the access code.*/
  bt_acc_code[0] = 0xBBCC54FC;
  //  LAP value
  a = lap_addr[0] & 0xFFFFFF;
  bt_acc_code[1] = 0x83848D96;
  //  Barker sequences (Only 6 valid bits)
  b0 = 0x2C;
  b1 = 0x13;
  //  Generator polynomial (Only 35 valid bits from lsb)
  gen_divisor[0] = 0x85713DA9;
  gen_divisor[1] = 0x5;

  /* Step 1: Based on the 24th bit of a, XOR a with either b1 or b0 shifted left by 24 bits, and store the result in x. */
  if (a & (1 << 23)) {
    x = a ^ (b1 << 24);
  } else {
    x = a ^ (b0 << 24);
  }

  /* Step 2: XOR x with the second bt_acc_code element (right-shifted by 2), shift x_bar[0] left by 2 and store in x_bar[1].
     Update x_bar[1] by XORing with the second bt_acc_code element. */
  x_bar[0] = x ^ (bt_acc_code[1] >> 2);

  /* Shifting x_bar to right by 34 bits */
  x_bar[1] = (x_bar[0] << 2);
  x_bar[0] = 0;

  bt_acc_code[1] ^= x_bar[1];

  gen_divisor[1] = gen_divisor[1] << 29;
  gen_divisor[1] |= (gen_divisor[0] >> 3);
  gen_divisor[0] = 0x20000000;

  /* Step 3: Call lc_polynom_modulo with x_bar, gen_divisor, and c_bar to perform polynomial division and store the result in c_bar.
     Then, update c_bar by ORing the first element right-shifted by 30 bits with the second element left-shifted by 2 bits,
     and right-shift the second element by 30 bits. */
  lc_polynom_modulo(x_bar, gen_divisor, c_bar);

  c_bar[0] = c_bar[0] >> 30;
  c_bar[0] |= (c_bar[1] << 2);
  c_bar[1] = c_bar[1] >> 30;

  bt_acc_code[0] ^= c_bar[0];
  bt_acc_code[1] ^= c_bar[1];

  return;
}
