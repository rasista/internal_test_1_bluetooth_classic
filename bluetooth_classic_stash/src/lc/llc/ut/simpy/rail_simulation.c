/**
 * @file rail_simulation.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-25
 *
 * @copyright Copyright (c) 2023- 2024
 *
 */

#include "rail_simulation.h"
#include "host_lpw_interface.h"
#include "acl_central.h"
#include "acl_peripheral.h"
#include "btc_device.h"
#include "btc_rail.h"
#include "inquiry.h"
#include "inquiry_scan.h"
#include "inquiry_sm_autogen.h"
#include "sl_btc_seq_interface.h"
#include "message_decode.h"
#include "page.h"
#include "page_scan.h"
#include "procedure_triggers_autogen.h"
#include "simulation_interface.h"
#include "state_machine.h"
#include <stdio.h>
#include <stdlib.h>
#include "roleswitch_central.h"
#include "esco.h"
#include "roleswitch_peripheral.h"

uint32_t end_tsf;
uint32_t start_tsf;
uint32_t inquiry_id_no;
uint32_t glbl_clk_sim_offset;
uint32_t clk_sim_offset[MAX_NUMBER_BTC_DEVICES];
uint32_t current_device_index;
int8_t g_u8_railStatus;
size_t buff_size = 0;
uint8_t *g_buffer;
uint16_t packet_len;
int8_t g_central_flag;
int8_t g_peripheral_flag;
int8_t g_rs_flag_central;
int8_t g_rs_flag_peripheral;
uint8_t g_hec_status = SIM_HEC_BIT;
uint8_t g_crc_status = SIM_CRC_BIT;
extern uint32_t id_counter;
extern uint8_t *packet;
extern size_t buffer_size;
extern btc_dev_t btc_dev[MAX_NUMBER_BTC_DEVICES];
uint8_t event_buffer[1024];
uint8_t *received_rx_pkt = NULL;
uint16_t received_rx_pkt_len = 0;


// typedef struct buffc_s buffc_t;

void tx_end_handler(uint32_t argc, uint8_t *argv[])
{
  current_device_index = atoi((char*)argv[0]);
  sim_printf("TX set_state 0");
  btc_tx_end_handler();
}

void tx_abort_handler(uint32_t argc, uint8_t *argv[])
{
  current_device_index = atoi((char*)argv[0]);
  sim_printf("TX set_state 0");
  btc_tx_abort_handler();
}

void rx_done_handler(uint32_t argc, uint8_t *argv[])
{
  current_device_index = atoi((char*)argv[0]);
  sim_printf("RX set_state 0");
  btc_rx_done_handler();
}

void rx_timeout_handler(uint32_t argc, uint8_t *argv[])
{
  current_device_index = atoi((char*)argv[0]);
  sim_printf("RX set_state 0");
  btc_rx_timeout_handler();
}

void backoff_done_handler(uint32_t argc, uint8_t *argv[])
{
  current_device_index = atoi((char*)argv[0]);
  // SM_STEP(INQUIRY_SCAN_SM
  // ,&btc_dev_cb[current_device_index].inquiry_scan_cb.inquiry_scan_sm_cb
  // ,BACKOFF_DONE);
}

void rx_header_received_handler(uint32_t argc, uint8_t *argv[])
{
  current_device_index = atoi((char*)argv[0]);
  btc_rx_header_received_handler();
}

void rx_abort_handler(uint32_t argc, uint8_t *argv[])
{
  current_device_index = atoi((char*)argv[0]);
  btc_rx_abort_handler();
}

typedef struct buffc_s buffc_t;
typedef struct sim_bd_clk_counter_s {
  uint32_t clk_r_offset;
  uint32_t allocated;
} sim_bd_clk_counter_t;
sim_bd_clk_counter_t glbl_bd_clk_counter[MAX_NUMBER_BTC_DEVICES][MAX_BD_CLK_COUNT_HANDLES];

void rail_assert(char *file, uint32_t line)
{
  sim_printf("Assert!!! %s, %d\n", file, line);
  while (1)
    ; // Rail Fail occurs, enters into assert
}
/**
 * @brief function returns the current bd_clk count value corresponding to
 * bd_clk handle
 *
 * @param bd_clk_handle
 * @return uint32_t
 */
uint32_t RAIL_SEQ_BTC_GetBtcDate(uint8_t ClkId)
{
  return (glbl_sim_sys_tick - clk_sim_offset[current_device_index])
         / (312.5 * SCALING_FACTOR);
}

uint32_t RAIL_SEQ_BTC_ConvertBtcDate(uint8_t ClkId, uint32_t btcDate)
{
  return (btcDate * 312.5 * SCALING_FACTOR);
}

/**
 * @brief Get channel number
 * @param  pointer to btcFhData
 * @return void
 */
void RAIL_SEQ_BTC_FhCopro(RAIL_SEQ_BtcFhConfig_t *btcFhConfig, RAIL_SEQ_BtcFhResult_t *btcFhResult)
{
  //Rail function will be added here
}

uint32_t rail_get_channel(btc_csa_procedure_t csa_procedure, uint32_t bd_clk)
{
  return 1;
}

/**
 * @brief
 *
 * @param clk_offset
 * @return uint32_t
 */
uint32_t rail_init_bd_clk_counter(uint32_t clk_offset)
{
  for (uint32_t i = 0; i < MAX_BD_CLK_COUNT_HANDLES; i++) {
    if (glbl_bd_clk_counter[current_device_index][i].clk_r_offset == clk_offset) {
      return i;
    }
  }
  for (int i = 0; i < MAX_BD_CLK_COUNT_HANDLES; i++) {
    if (!glbl_bd_clk_counter[current_device_index][i].allocated) {
      glbl_bd_clk_counter[current_device_index][i].clk_r_offset = clk_offset;
      return i;
    }
  }
  rail_assert(__FILE__, __LINE__);
  return 0;
}

void init_btc_device_handler()
{
  int i = 0;
  // memset(frequency_hop_io_p, 0, sizeof(frequency_hop_io_t));
  if (i == MAX_NUMBER_BTC_DEVICES) {
    rail_assert(__FILE__, __LINE__);
  }
  // btc_dev[current_device_index].acl = (acl_t *)calloc(sizeof(acl_t), 1);
  clk_sim_offset[current_device_index] = glbl_sim_sys_tick;

  return;
}

uint32_t rail_schedule_id_tx(btc_trx_configs_t *txrx_configs)
{
  uint32_t return_sts;
  if (g_u8_railStatus == BTC_RAIL_PASS_STATUS) {
    int8_t cmd_buffer[1024] = { 0 };
    uint32_t packet_dur = 68 * SCALING_FACTOR;
    start_tsf = txrx_configs->btc_rail_trx_config.tsf + clk_sim_offset[current_device_index];
    end_tsf = packet_dur + start_tsf;
    uint8_t target_model;
    target_model = current_device_index == 1 ? 2 : 1;
    snprintf((char*)cmd_buffer, 1023, "rail_%d  schedule_tx %d %d %u %u %u\n",
             current_device_index, 0, start_tsf, end_tsf,
             target_model,
             IS_TX);
    cmd_response((uint8_t*)cmd_buffer);
    printf("%u TX set_state 1", start_tsf);
    return_sts = BTC_RAIL_PASS_STATUS;
  } else {
    //to toggle railStatus flag back to pass so that it doesn't enter into infinite loop
    g_u8_railStatus = BTC_RAIL_PASS_STATUS;
    return_sts = BTC_RAIL_FAIL_STATUS;
  }
  return return_sts;
}

uint32_t rail_schedule_id_rx(btc_trx_configs_t *txrx_configs)
{
  uint32_t return_sts;
  if (g_u8_railStatus == BTC_RAIL_PASS_STATUS) {
    int8_t cmd_buffer[1024] = { 0 };
    uint32_t packet_dur =  txrx_configs->rx_window;
    start_tsf = txrx_configs->btc_rail_trx_config.tsf + clk_sim_offset[current_device_index];
    end_tsf = packet_dur + start_tsf;
    uint8_t target_model;
    target_model = current_device_index == 1 ? 2 : 1;
    snprintf((char*)cmd_buffer, 1023, "rail_%d schedule_rx %d %d %u %u %u\n",
             current_device_index, 0, start_tsf, end_tsf,
             target_model,
             IS_RX);
    printf("%u RX set_state 1", start_tsf);
    cmd_response((uint8_t*)cmd_buffer);
    return_sts = BTC_RAIL_PASS_STATUS;
  } else {
    //to toggle railStatus flag back to pass so that it doesn't enter into infinite loop
    g_u8_railStatus = BTC_RAIL_PASS_STATUS;
    return_sts = BTC_RAIL_FAIL_STATUS;
  }
  return return_sts;
}

uint32_t rail_schedule_tx(btc_trx_configs_t *txrx_configs)
{
  uint32_t return_sts;
  if (g_u8_railStatus == BTC_RAIL_PASS_STATUS) {
    uint8_t cmd_buffer[1024] = { 0 };
    start_tsf                = txrx_configs->btc_rail_trx_config.tsf + clk_sim_offset[current_device_index];
    if (g_central_flag || g_rs_flag_peripheral) {
      txrx_configs->tx_window = packet_len;
      g_central_flag            = 0;
      if (g_rs_flag_peripheral) {
        g_rs_flag_peripheral = 0;
      }
    }
    end_tsf = start_tsf + txrx_configs->tx_window;
    uint8_t target_model;
    target_model = current_device_index == 1 ? 2 : 1;
    snprintf((char*)cmd_buffer, 1024, "rail_%d schedule_tx %d %d %u %u %u\n",
             current_device_index, 0, start_tsf, end_tsf,
             target_model,
             IS_TX);
    printf("%u TX set_state 1", start_tsf);
    cmd_response(cmd_buffer);
    return_sts = BTC_RAIL_PASS_STATUS;
  } else {
    //to toggle railStatus flag back to pass so that it doesn't enter into infinite loop
    g_u8_railStatus = BTC_RAIL_PASS_STATUS;
    return_sts = BTC_RAIL_FAIL_STATUS;
  }
  return return_sts;
}

uint32_t rail_schedule_rx(btc_trx_configs_t *txrx_configs)
{
  uint32_t return_sts;
  if (g_u8_railStatus == BTC_RAIL_PASS_STATUS) {
    uint8_t cmd_buffer[1024] = { 0 };
    start_tsf                = txrx_configs->btc_rail_trx_config.tsf + clk_sim_offset[current_device_index];
    uint32_t rx_tout;
    if (g_peripheral_flag || g_rs_flag_central) {
      txrx_configs->rx_window = packet_len;
      rx_tout                  = start_tsf + txrx_configs->rx_window;
      g_peripheral_flag        = 0;
      if (g_rs_flag_central) {
        g_rs_flag_central = 0;
      }
    } else {
      rx_tout = start_tsf + txrx_configs->rx_window + 270;
    }
    uint8_t target_model;
    target_model = current_device_index == 1 ? 2 : 1;
    snprintf((char*)cmd_buffer, 1024, "rail_%d schedule_rx %d %d %u %u %u\n",
             current_device_index, 0, start_tsf, rx_tout,
             target_model,
             IS_RX);
    cmd_response(cmd_buffer);
    printf("%u RX set_state 1", start_tsf);
    return_sts = BTC_RAIL_PASS_STATUS;
  } else {
    //to toggle railStatus flag back to pass so that it doesn't enter into infinite loop
    g_u8_railStatus = BTC_RAIL_PASS_STATUS;
    return_sts = BTC_RAIL_FAIL_STATUS;
  }
  return return_sts;
}

uint32_t get_current_clk()
{
  return glbl_sim_sys_tick;
}

buffc_t buffc;
// Initialize the circular buffer
void initialize_buffer(buffc_t *buffc)
{
  buffc->head  = 0;
  buffc->tail  = 0;
  buffc->count = 0;
}

// Add data to the circular buffer
void add_to_buffer(buffc_t *buffc, int value)
{
  if (buffc->count == buffc->size) {
    printf("Buffer is full. Data cannot be added.\n");
  }

  buffc->data[buffc->head] = value;
  buffc->head              = (buffc->head + 1) % BUFC_SIZE;

  if (buffc->head == buffc->tail) {
    sim_printf("buffer empty");
  }
}

void remove_from_buffer(buffc_t *buffc, int *value)
{
  if ((buffc->count) == 0) {
    sim_printf("Buffer is empty. No data to remove.\n");
  }
  *value      = buffc->data[buffc->tail];
  buffc->tail = (buffc->tail + 1) % BUFC_SIZE;
}

uint32_t parse_rx_packet(buffc_t *buffc)
{
  /*uint8_t rx_pkt[16];
     uint16_t packet_header = *(uint16_t *)&rx_pkt[4];*/
  return 0;
}

// packet parsing, no buffc, from register its copying value

void rail_process_rx_header_info()
{
  buffc_t buffc;
  parse_rx_packet(&buffc);
}

buffc_t *trigger_buffc_if_payload_present(buffc_t *buffc, uint16_t length) /*
                                                                              // Allocate memory for the requested length and copy data
                                                                              buffc *data = (buffc *)malloc(length);
                                                                              if (data != NULL) {
                                                                              // Copy the requested data from the buffer to the
                                                                              allocated memory memcpy(data, buffc, length); return
                                                                              data; } else {
                                                                              // Allocation failed
                                                                              return NULL;
                                                                              }
                                                                              this function will be used after buffc implementation is
                                                                              in place
                                                                            */
{
  /*
     Function will be implemented after the buffc implementation is in place.
   */
  return NULL;
}

/**
 * @brief Dummy implementation of RAIL_SEQ_Idle for simulation
 *
 * This function simulates the behavior of stopping radio operations
 * without actually controlling any hardware. In real implementation,
 * this would abort ongoing operations and idle the radio.
 *
 * @param mode The idle mode (RAIL_IDLE_ABORT, etc.)
 * @param wait Whether to wait for completion
 *
 * @return RAIL_STATUS_NO_ERROR always in simulation
 */
RAIL_Status_t RAIL_SEQ_Idle(RAIL_IdleMode_t mode, bool wait)
{
  // Unused parameters in simulation
  (void)mode;
  (void)wait;

  // Debug print to show function was called
  sim_printf("RAIL_SEQ_Idle called: mode=%d, wait=%d", mode, wait);

  // Always return success in simulation environment
  return RAIL_STATUS_NO_ERROR;
}

uint32_t rail_is_buffc_triggered(
  buffc_t *buffc)   // check if there is any value in buffc showing that the
                    // buffc is triggered if there is a value present
{
  for (int i = 0; i < BUFC_SIZE; i++) {
    if (!(buffc->head == buffc->tail)) {
      return 1;
    }
  }
  return 0;
}

void rail_process_arqn_stage1()
{
  // add call of the API after implementation
} // dummy function

void wait_for_bufc_transfer_status()
{
  /*
     Function will be implemented after the buffc implemetation is in place.
     dummy function
   */
}

void rail_process_arqn_stage2()
{
  // add call of the API after implementation
} // dummy function

void rail_wait_for_bufc_transfer_status()
{
  // add call of the API after implementation
} // dummy function

void log2vcd_prints_wrapper(sm_t *l2_scheduled_sm)
{
  sim_printf("CURR_STATE set_state %s", l2_scheduled_sm->sm_name);
  return;
}

uint8_t rail_get_hec_status() // Function to get the value of hec bit from Hardware/Rail layer
{
  return g_hec_status;
}

uint8_t rail_get_crc_status() // Function to get the value of crc bit from Hardware/Rail layer
{
  return g_crc_status;
}

uint8_t rail_get_mic_status() // Function to get the value of mic bit from Hardware/Rail layer
{
  return SIM_MIC_BIT;
}

void rail_set_btc_clk_instance(bd_clk_counter_t *bd_clk_counter)
{
  // This function is used to set the bd_clk_counter instance in the rail layer
  // Currently, it does nothing as we are simulating the rail layer
  (void)bd_clk_counter;
}

/*
 * @brief This function returns device pointer, gives which device is currently active
 *
 */
btc_dev_t *get_btc_device_pointer_addr()
{
  return &btc_dev[current_device_index];
}

void RAIL_SEQ_BTC_setAccessAddress(uint32_t *accessAddress)
{
  (void)accessAddress;
}

void RAIL_SEQ_BTC_setHecInit(uint8_t hecInit)
{
  (void)hecInit;
}

void RAIL_SEQ_BTC_setCrcInit(uint16_t crcInit)
{
  (void)crcInit;
}

//Dummy Function
void RAIL_SEQ_BTC_InitBtcDate(uint32_t btc_clk_id, uint32_t btcReference, uint32_t protimerReference)
{
  (void)btc_clk_id;
  (void)btcReference;
  (void)protimerReference;
}

void RAIL_SEQ_BTC_ConfigAutoMode(uint32_t btc_clk_id, uint32_t mode)
{
  (void)btc_clk_id;
  (void)mode;
}

void RAIL_SEQ_BTC_GetLastRxDate(uint32_t bd_clk_counter_id, uint32_t *BtcDate, uint64_t *ProtimerDate)
{
  (void)ProtimerDate;
  *BtcDate = RAIL_SEQ_BTC_GetBtcDate(bd_clk_counter_id) >> CLK_SCALE_FACTOR;
}

void RAIL_SEQ_SendMbox(uint32_t event)
{
  (void)event;
}

void BTC_post_notification_to_ulc(pkb_t *pkb)
{
  (void)pkb;
}

pkb_t* get_buffer_for_notification(uint8_t **buffer)
{
  *buffer = event_buffer;
  return NULL;
}

void RAIL_SEQ_BTC_setE0Encryption(RAIL_SEQ_E0EncryptionConfig_t *e0EncConfig)
{
  (void)e0EncConfig;
}

void RAIL_SEQ_BTC_setAESEncryption(RAIL_SEQ_AESEncryptionConfig_t *aesEncConfig)
{
  (void)aesEncConfig;
}

void RAIL_SEQ_SetChannel(uint8_t channel)
{
  (void)channel;
}

uint16_t RAIL_SEQ_BTC_SlotOffsetUs(uint8_t clkIdbforeRoleswitch, uint8_t clkIdafterRoleswitch)
{
  (void)clkIdbforeRoleswitch;
  (void)clkIdafterRoleswitch;
  return 0;
}