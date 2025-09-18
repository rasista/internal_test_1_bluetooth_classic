#include "stdio.h"
#include <inttypes.h>
#include <simulation_interface.h>
#include <stdlib.h>

uint32_t glbl_sim_sys_tick = 0;
void restart_cmd_handler(int argc, uint8_t *argv[])
{
}

void end_cmd_handler(int argc, uint8_t *argv[])
{
  cmd_response((uint8_t *)"SimulationEnded\n");
}

void update_sys_tick_handler(int argc, uint8_t *argv[])
{
  glbl_sim_sys_tick = atoi((char*)argv[0]);
}
