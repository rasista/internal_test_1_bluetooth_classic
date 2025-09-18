#include "host_lpw_interface.h"
#include "btc_device.h"
#include "state_machine.h"
#include "rail_simulation.h"
#include "simulation_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern btc_dev_t btc_dev[MAX_NUMBER_BTC_DEVICES];
int8_t g_u8_eir_sim_status;
int g_u8_packet_type;
extern int8_t g_u8_railStatus;
extern uint16_t packet_len;
extern int8_t g_central_flag;
extern int8_t g_peripheral_flag;
extern int8_t g_rs_flag_central;
extern int8_t g_rs_flag_peripheral;
extern uint8_t g_hec_status;
extern uint8_t g_crc_status;

void var_status_handler(uint32_t argc, uint8_t *argv[])
{
  if (!strcmp((char*)argv[1], "rail_status")) {
    g_u8_railStatus = atoi((char*)argv[2]);
  } else if (!strcmp(((char*)argv[1]), "crc_status")) {
    g_crc_status = atoi((char*)argv[2]);
  } else if (!strcmp(((char*)argv[1]), "hec_status")) {
    g_hec_status = atoi((char*)argv[2]);
  } else if (!strcmp(((char*)argv[1]), "eir_status")) {
    g_u8_eir_sim_status = atoi((char*)argv[2]);
  } else if (!strcmp(((char*)argv[1]), "packet_type")) {
    g_u8_packet_type = atoi((char*)argv[3]);
    if (g_u8_packet_type == DH1) {
      packet_len = (1 * (625) * (SCALING_FACTOR)) - 1;
    } else if (g_u8_packet_type == DH3) {
      packet_len = (3 * (625) * (SCALING_FACTOR)) - 1;
    } else if (g_u8_packet_type == DH5) {
      packet_len = (5 * (625) * (SCALING_FACTOR)) - 1;
    } else {
    }  // }else if(g_u8_packet_type == FHS){
    //   packet_len = 552;
    // }
    if (!strcmp(((char*)argv[2]), "central")) {
      g_central_flag    = 1;
      g_peripheral_flag = 0;
    } else if (!strcmp(((char*)argv[2]), "peripheral")) {
      g_central_flag    = 0;
      g_peripheral_flag = 1;
    } else if (!strcmp(((char*)argv[2]), "rs_peripheral")) {
      g_rs_flag_peripheral = 1;
      g_rs_flag_central = 0;
    } else if (!strcmp((char*)(argv[2]), "rs_central")) {
      g_rs_flag_central = 1;
      g_rs_flag_peripheral = 0;
    }
  }
  return;
}

void rail_lpw_procedure_complete_event(uint8_t *buffer, uint16_t buff_len)
{
  uint8_t cmd_buf[1024];
  int offset = snprintf((char*)cmd_buf, 1023, "btc_dev_%d complete_event", current_device_index);
  for (uint16_t i = 0; i < buff_len && offset < 1023; ++i) {
    offset += snprintf((char*)cmd_buf + offset, 1023 - offset, " %u", buffer[i]);
  }

  cmd_response(cmd_buf);
}

void set_payload_handler(uint32_t argc, uint8_t *argv[])
{
  //function is used to set the payload, will be after packet implementation
}

void btc_roleswitch_complete_indication_to_ull(sm_t *connection_info_sm, uint8_t *buffer, uint8_t status)
{
  uint8_t cmd_buf[EVENT_BUFF_LEN + 1];
  snprintf((char*)cmd_buf, EVENT_BUFF_LEN, "btc_dev_%d roleswitch_complete %u %u %u", connection_info_sm->current_device_index,
           buffer[0], buffer[1], buffer[2]);
  cmd_response(cmd_buf);
}

void btc_inquiry_complete_indication_to_ull(sm_t *inquiry_sm, uint8_t *buffer)
{
  uint8_t cmd_buf[EVENT_BUFF_LEN + 1];
  snprintf((char*)cmd_buf, EVENT_BUFF_LEN, "btc_dev_1 procedure_complete %u %u %u",
           buffer[0], buffer[1], buffer[2]);
  cmd_response(cmd_buf);
}

void btc_page_complete_indication_to_ull(sm_t *page_sm, uint8_t *buffer)
{
  uint8_t cmd_buf[EVENT_BUFF_LEN + 1];
  snprintf((char*)cmd_buf, EVENT_BUFF_LEN, "btc_dev_1 procedure_complete %u %u %u",
           buffer[0], buffer[1], buffer[2]);
  cmd_response(cmd_buf);
}

void btc_inquiry_scan_complete_indication_to_ull(sm_t *inquiry_scan_sm, uint8_t *buffer)
{
  uint8_t cmd_buf[EVENT_BUFF_LEN + 1];
  snprintf((char*)cmd_buf, EVENT_BUFF_LEN, "btc_dev_1 procedure_complete %u %u %u",
           buffer[0], buffer[1], buffer[2]);
  cmd_response(cmd_buf);
}

void btc_page_scan_complete_indication_to_ull(sm_t *page_scan_sm, uint8_t *buffer)
{
  uint8_t cmd_buf[EVENT_BUFF_LEN + 1];
  snprintf((char*)cmd_buf, EVENT_BUFF_LEN, "btc_dev_1 procedure_complete %u %u %u",
           buffer[0], buffer[1], buffer[2]);
  cmd_response(cmd_buf);
}

void btc_connection_complete_indication_to_ull(sm_t *connection_info_sm, uint8_t *buffer)
{
  uint8_t cmd_buf[EVENT_BUFF_LEN + 1];
  snprintf((char*)cmd_buf, EVENT_BUFF_LEN, "btc_dev_1 procedure_complete %u %u %u",
           buffer[0], buffer[1], buffer[2]);
  cmd_response(cmd_buf);
}
