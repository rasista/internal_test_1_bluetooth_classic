#include "debug.h"
#include "stdio.h"
#include "stdarg.h"
#include "btc_plt_deps.h"

uint32_t debug_init_done = 0;
btc_lock_t debug_print_lock;
btc_lock_t debug_hex_dump_lock;
btc_lock_t debug_assert_lock;
//! function to initialize debug

void debug_init(void)
{
  if (debug_init_done) {
    return;
  }
  btc_plt_create_lock(&debug_assert_lock);
  btc_plt_create_lock(&debug_print_lock);
  btc_plt_create_lock(&debug_hex_dump_lock);

  debug_init_done = 1;
}
//! function to take variable args
void btc_assert(enum BTC_ASSERT_TYPE msg, const char *file, int line)
{
  btc_plt_acquire_lock(&debug_assert_lock);
  btc_print("BTC_ASSERT %d  %s  :  %d \n", msg, file, line);
  while (1);
  btc_plt_release_lock(&debug_assert_lock);
}

//! function btc_print that takes variable args
void btc_print(const char *fmt, ...)
{
  btc_plt_acquire_lock(&debug_print_lock);
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  btc_plt_release_lock(&debug_print_lock);
}
//! function to dump raw bytes, no more than 16 bytes per line.
void btc_hex_dump(char *msg, uint8_t *bytes, uint16_t len)
{
  (void)bytes;
  btc_plt_acquire_lock(&debug_hex_dump_lock);
  btc_print("%s\n", msg);
  for (int i = 0; i < len; i++) {
    if (i % 16 == 0) {
      btc_print("\n");
    }
    BTC_PRINT("0x%02x ", bytes[i]);
  }
  BTC_PRINT("\n");
  btc_plt_release_lock(&debug_hex_dump_lock);
}
