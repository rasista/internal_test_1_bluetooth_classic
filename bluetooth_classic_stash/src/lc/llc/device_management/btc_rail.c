/**
 * @file btc_rail.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#include "btc_rail.h"
#include "btc_device.h"
#include "state_machine.h"
#include "debug_logs.h"
#ifdef SIM
#include "rail_simulation.h"
#else
#include "btc_rail_intf.h"
#include "rail_seq.h"
#include "sl_debug_btc_llc_autogen.h"
extern btc_dev_t btc_dev; // Global variable for the Bluetooth device structure
#endif

extern uint32_t get_current_clk(); //these functions are defined in rail_simulation.c file, Added externs in case #ifdef sim not defined.
extern void rail_process_rx_header_info();
extern uint32_t rail_is_buffc_triggered(buffc_t *buffc);
extern buffc_t *trigger_buffc_if_payload_present(buffc_t *buffc, uint16_t length);
extern void rail_process_arqn_stage1();
extern void rail_process_arqn_stage2();
extern void rail_wait_for_bufc_transfer_status();
extern void rail_assert(char *file, uint32_t line);

const uint32_t target_bd_clk_lookup[4][4] = { { 4, 1, 2, 3 }, { 3, 4, 1, 2 }, { 2, 3, 4, 1 }, { 1, 2, 3, 4 } };

extern uint8_t rail_get_hec_status();
extern uint8_t rail_get_crc_status();
extern uint8_t rail_get_mic_status();

/**
 * @brief function returns the very next 2NPX bd_clk number from current bd_clk,
 * for example, if current bd_clk is 0, and requested bd_clk type is 2NP0,
 * function returns 4 but if requested bd_clk type is 2NP1, function returns 1.
 *
 * @param current_bd_clk
 * @param btc_bd_clk_identifier
 * @return uint32_t
 */
static inline uint32_t btc_rail_get_target_bd_clk(uint32_t current_bd_clk,
                                                  btc_bd_clk_identifier_t btc_bd_clk_identifier)
{
  return ((current_bd_clk + target_bd_clk_lookup[current_bd_clk & 0x3][btc_bd_clk_identifier]));
}
/**
 * @brief Wrapper function to get target bd_clk
 *
 * @param current_bd_clk
 * @param btc_bd_clk_identifier
 * @return uint32_t
 */
uint32_t get_target_bd_clk(uint32_t current_bd_clk, btc_bd_clk_identifier_t btc_bd_clk_identifier)
{
  return btc_rail_get_target_bd_clk(current_bd_clk, btc_bd_clk_identifier);
}

/**
 * @brief computes btc_trx_configs_t on the basis of input parameters
 *
 * @param bd_clk_handle : handle to bd_clk counter module
 * @param bt_bd_clk_identifier  : target bd_clk boundary offset
 * @param btc_trx_configs : data needed for RAIL configurations
 * @return uint32_t
 */
uint32_t btc_get_txrx_clk_configs(btc_bd_clk_identifier_t bt_bd_clk_identifier,
                                  btc_trx_configs_t *btc_trx_configs)
{
  uint32_t current_bd_clk = 0;
  uint32_t fractional_bd_clk = 0;
  current_bd_clk = RAIL_SEQ_BTC_GetBtcDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id);
  fractional_bd_clk = current_bd_clk & 0xF;
  current_bd_clk = current_bd_clk >> CLK_SCALE_FACTOR;
 
  if(fractional_bd_clk > BTC_CLK_FRACTIONAL_THRESHOLD)
  {
#ifndef SIM
    /* Fractional bd clk is not sufficient to schedule the packet. Increment the bd clk by 1 */
    sl_btdm_fast_log(btc_log_rail_clk_fractional_error, COMPONENT_SEQ_BTC, TRACE, "Fractional bd clk is not sufficient to schedule the packet. Incrementing bd clk by 1");
    current_bd_clk++;
#endif
  }
  btc_trx_configs->btc_rail_trx_config.clk =
    btc_rail_get_target_bd_clk(current_bd_clk, bt_bd_clk_identifier);
  btc_trx_configs->btc_rail_trx_config.tsf =
    RAIL_SEQ_BTC_ConvertBtcDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk) >> TSF_SCALE_FACTOR;
  return 0;
}

uint32_t btc_get_txrx_clk_configs_with_clk(uint32_t clk,
                                           btc_bd_clk_identifier_t bt_bd_clk_identifier,
                                           btc_trx_configs_t *btc_trx_configs)
{
  btc_trx_configs->btc_rail_trx_config.clk =
    btc_rail_get_target_bd_clk(clk, bt_bd_clk_identifier);
  btc_trx_configs->btc_rail_trx_config.tsf =
    RAIL_SEQ_BTC_ConvertBtcDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, btc_trx_configs->btc_rail_trx_config.clk) >> TSF_SCALE_FACTOR;
  return 0;
}

/**
 * @brief computes tsf based on the time provided
 *
 * @param bd_clk_handle : handle to bd_clk counter module
 * @param btc_trx_configs : data needed for RAIL configurations
 * @param target_time : clk time from which tsf will be calculated
 */
void btc_get_txrx_clk_configs_with_target_time(btc_trx_configs_t *btc_trx_configs,
                                               uint32_t target_time)
{
  // get target tsf value from target bd_clk
  btc_trx_configs->btc_rail_trx_config.tsf =
    RAIL_SEQ_BTC_ConvertBtcDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, target_time) >> TSF_SCALE_FACTOR;
}

/**
 * @brief funciton will interact with HW channel selection algorithm
 * @param bd_clk
 * @param csa_procedure
 * @return uint32_t
 */
uint32_t btc_get_channel(btc_csa_procedure_t csa_procedure, uint32_t bd_clk)
{
  return rail_get_channel(csa_procedure, bd_clk);
}

/**
 * @brief funciton calls RAIL APIs to initialise HW module to start ticking at
 * every 312.5 us
 *
 * @param clk_offset
 * @return uint32_t
 */
uint32_t btc_init_bd_clk_counter(uint32_t clk_offset)
{
  return rail_init_bd_clk_counter(clk_offset);
}

/**
 * @brief function schedules a RAIL activity for BT ID packet TX.
 * This is a dedicated API for ID TX and not same as any other packet TX as ID
 * packet TX requires special BB handling.
 *
 * @param btc_trx_configs
 * @return uint32_t
 */
uint32_t btc_schedule_id_tx(btc_trx_configs_t *btc_trx_configs)
{
  return rail_schedule_id_tx(btc_trx_configs);
}

uint32_t btc_schedule_id_rx(btc_trx_configs_t *btc_trx_configs)
{
  return rail_schedule_id_rx(btc_trx_configs);
}

uint32_t btc_schedule_tx(btc_trx_configs_t *btc_trx_configs)
{
  return rail_schedule_tx(btc_trx_configs);
}

uint32_t btc_schedule_rx(btc_trx_configs_t *btc_trx_configs)
{
  return rail_schedule_rx(btc_trx_configs);
}

/**
 * @brief function call is used to get the bd clk count
 *
 * @param bd_clk_handle
 * @return uint32_t
 */
uint32_t btc_get_current_bd_clk_cnt(bd_clk_counter_t *bd_clk_handle)
{
  return RAIL_SEQ_BTC_GetBtcDate(bd_clk_handle->bd_clk_counter_id) >> CLK_SCALE_FACTOR;
}

/**
 * @brief wraper function returns the very next 2NPX bd_clk number from current
 * bd_clk
 *
 * @param bd_clk_identifier
 * @return uint32_t
 */
uint32_t btc_get_target_bd_clk(uint32_t current_bd_clk, btc_bd_clk_identifier_t btc_bd_clk_identifier)
{
  return btc_rail_get_target_bd_clk(current_bd_clk, btc_bd_clk_identifier);
}

/**
 * @brief function call used to get the current clock value in simulation
 *
 *
 * @return uint32_t
 */
uint32_t btc_get_current_clk()
{
  return get_current_clk();
}

/**
 * @brief Function call to parse rx_packet header information,its definition will be moved to acl file after packets,header implementation
 *
 */
void btc_rx_header_info()
{
  rail_process_rx_header_info();
}

/**
 * @brief Function call to trigger circular buffer,its definition will be moved to acl file after packets,header implementation
 *
 */
buffc_t *btc_trigger_buffc(buffc_t *buffc, uint8_t length)
{
  return trigger_buffc_if_payload_present(buffc, length);
}

/**
 * @brief Function call to check if circular buffer is triggered or not,its definition will be moved to acl file after packets,header implementation
 *
 */
uint32_t btc_is_buffc_triggered(buffc_t *buffc)
{
  return rail_is_buffc_triggered(buffc);
}

/**
 * @brief Function call for updating arqn schemes,its definition will be moved to acl file after packets,header implementation
 *
 */
void btc_arqn_stage1()
{
  rail_process_arqn_stage1();
}

void btc_arqn_stage2()
{
  rail_process_arqn_stage2();
}

/**
 * @brief Function call to check the buffer status,its definition will be moved to acl file after packets,header implementation
 *
 */
void btc_wait_for_bufc_transfer_status()
{
  rail_wait_for_bufc_transfer_status();
}

/**
 * @brief Function call to update rail about connection completion information
 *
 */
void rail_configure_idle()
{
  //dummy function
}

uint8_t btc_get_hec_status()    // Wrapper function to get the status of the HEC and this would be called by FW APIs to get the value of HEC
{
  return (rail_get_hec_status() == 1) ? TRUE : FALSE;
}

uint8_t btc_get_crc_status()    // Wrapper function to get the status of the CRC and this would be called by FW APIs to get the value of CRC
{
  return (rail_get_crc_status() == 1) ? TRUE : FALSE;
}

uint8_t btc_get_mic_status()    // Wrapper function to get the status of the MIC and this would be called by FW APIs to get the value of MIC
{
  return rail_get_mic_status();
}

void btc_rail_assert(char *file, uint32_t line)
{
  (void)file;
  (void)line;
  //rail_assert(__FILE__, __LINE__); // Rail Fail occurs, enters into assert
}

void btc_tx_end_handler()
{
  btc_dev_t *btc_dev_ptr = BTC_DEV_PTR;
  if (btc_dev_ptr->l2_scheduled_sm) {
    SM_STEP(PAGE_SM, btc_dev_ptr->l2_scheduled_sm, TX_DONE);
  }
}

void btc_tx_abort_handler()
{
  btc_dev_t *btc_dev_ptr = BTC_DEV_PTR;
  if (btc_dev_ptr->l2_scheduled_sm) {
    SM_STEP(PAGE_SM, btc_dev_ptr->l2_scheduled_sm, TX_ABORT);
  }
}

void btc_rx_done_handler()
{
  btc_dev_t *btc_dev_ptr = BTC_DEV_PTR;
  if (btc_dev_ptr->l2_scheduled_sm) {
    SM_STEP(PAGE_SM, btc_dev_ptr->l2_scheduled_sm, RX_DONE);
  }
}

void btc_rx_timeout_handler()
{
  btc_dev_t *btc_dev_ptr = BTC_DEV_PTR;
  if (btc_dev_ptr->l2_scheduled_sm) {
    SM_STEP(PAGE_SM, btc_dev_ptr->l2_scheduled_sm, RX_TIMEOUT);
  }
}

void btc_rx_header_received_handler()
{
  btc_dev_t *btc_dev_ptr = BTC_DEV_PTR;
  SM_STEP(PAGE_SM, btc_dev_ptr->l2_scheduled_sm, RX_HEADER_RECEIVED);
}

void btc_rx_abort_handler()
{
  btc_dev_t *btc_dev_ptr = BTC_DEV_PTR;
  SM_STEP(PAGE_SM, btc_dev_ptr->l2_scheduled_sm, RX_ABORT);
}
