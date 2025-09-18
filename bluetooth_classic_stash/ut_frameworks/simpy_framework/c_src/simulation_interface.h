#ifndef SIMULATION_INTERFACE_H
#define SIMULATION_INTERFACE_H
#define PASS 0
#define FAIL 1
#include <inttypes.h>
#include <stdio.h>

extern FILE *fp;
extern FILE *output_ptr;
extern uint8_t response_buffer[];
extern uint32_t glbl_sim_sys_tick;
extern uint32_t current_device_index;
void cmd_response(const uint8_t *response_str);
void signal_handler(int sig);
void registering_signals();
static inline void bt_assert(uint8_t *msg) {
  printf("Assert!!! : %s", msg);
  while (1)
    ;
}

#define sim_printf(msg, ...)                                                   \
  fprintf(output_ptr, "\n %u " msg " \n", glbl_sim_sys_tick, ##__VA_ARGS__);
#endif
