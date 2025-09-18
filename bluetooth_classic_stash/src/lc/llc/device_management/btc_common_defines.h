#ifndef BTC_COMMON_DEFINES_H
#define BTC_COMMON_DEFINES_H
#include "procedure_triggers_autogen.h"
#include "stddef.h"
#include "stdint.h"

#define BD_ADDR_LEN 6
#define SYNC_WORD_LEN 8
#define LAP_ADDR_LEN 3
#define CHANNEL_MAP_SIZE 10
#ifndef BT_SLOT_WIDTH
  #define BT_SLOT_WIDTH 625
#endif

#define ENABLE_SLOW_HOP (BT_SLOT_WIDTH == 625) ? 0 : 1

typedef __uint8_t uint8_t;
typedef __uint16_t uint16_t;
typedef __uint32_t uint32_t;
typedef __uint64_t uint64_t;

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) \
  ((type *)((char *)(ptr) - offsetof(type, member)))

typedef struct bd_clk_counter_s {
  uint32_t bd_clk_counter_id;
} bd_clk_counter_t;

typedef enum state_machine_enum {
  inquiry_sm_context,
  inquiry_scan_sm_context,
  page_sm_context,
  page_scan_sm_context,
  acl_central_sm_context,
  acl_peripheral_sm_context,
  roleswitch_central_sm_context,
  roleswitch_peripheral_sm_context,
  esco_central_sm_context,
  esco_peripheral_sm_context,
  test_mode_sm_context,
} state_machine_enum_t;

#endif
