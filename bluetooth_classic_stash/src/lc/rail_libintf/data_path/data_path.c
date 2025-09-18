#include "data_path.h"
#include <stdlib.h>  /* For malloc and free functions */
#include <stdio.h>
#include <stdint.h>
#include "debug.h"  /* For BTC_PRINT */

/**
 * @file data_path.c
 * @brief Implementation of the HSS to LPWSS data path for Bluetooth Classic
 *
 * This file implements buffer management and data transfer mechanisms between
 * Host Subsystem (HSS) and Low Power Wireless Subsystem (LPWSS) with support
 * for multiple connection instances (ACL/SCO).
 */

/** Global variables for buffer management */
linear_buffer_t linear_buffer_pool[LINEAR_BUFFER_POOL_SIZE];
uint32_t data_pool[LINEAR_BUFFER_POOL_SIZE][DEFAULT_BUFFER_SIZE / 4];
buffer_fifo_manager_t fifo_manager = { 0 }; // Manager for multiple FIFOs

/**
 * @brief Get a pointer to the shared FIFO for a specific instance.
 *
 * This function returns a pointer to the specified FIFO instance.
 * It validates the instance_id to ensure it's within range and active.
 *
 * @param instance_id ID of the FIFO instance (connection ID)
 * @return Pointer to the specified FIFO instance or NULL if invalid
 */
buffer_fifo_t *get_shared_fifo_ptr(uint8_t instance_id)
{
  if (instance_id < MAX_CONNECTION_INSTANCES && fifo_manager.is_instance_active[instance_id]) {
    return &fifo_manager.fifo_instances[instance_id];
  }
  return NULL;
}

/**
 * @brief Initialize a new FIFO instance for a connection.
 *
 * This function initializes a new FIFO instance for a specific connection.
 * It finds an available instance slot and sets it up for use.
 *
 * @param instance_id ID to assign to the new instance
 * @param riscv_memory_indicator_address Pointer to a uint32_t to receive the FIFO address (may be NULL)
 * @return true if initialization was successful, false otherwise
 */
bool init_data_path(uint8_t instance_id, uint32_t riscv_memory_indicator_address)
{
  if (instance_id < MAX_CONNECTION_INSTANCES && !fifo_manager.is_instance_active[instance_id]) {
    // Initialize the FIFO instance
    fifo_manager.fifo_instances[instance_id].read_index = 0;
    fifo_manager.fifo_instances[instance_id].write_index = 0;
    memset(fifo_manager.fifo_instances[instance_id].data_buffer, 0,
           sizeof(fifo_manager.fifo_instances[instance_id].data_buffer));
    // Mark the instance as active
    fifo_manager.is_instance_active[instance_id] = 1;
    fifo_manager.active_instances++;

    // Only write to riscv_memory_indicator_address if it is non-NULL
    if (riscv_memory_indicator_address != 0) {
      fifo_manager.fifo_instances[instance_id].riscv_addr = riscv_memory_indicator_address;
    }
    return true;
  }
  return false;
}

/**
 * @brief Deinitialize a FIFO instance when a connection is closed.
 *
 * This function cleans up a FIFO instance when it's no longer needed.
 *
 * @param instance_id ID of the instance to deinitialize
 * @return true if deinitialization was successful, false otherwise
 */
bool deinit_fifo_instance(uint8_t instance_id)
{
  if (instance_id < MAX_CONNECTION_INSTANCES && fifo_manager.is_instance_active[instance_id]) {
    // Mark the instance as inactive
    fifo_manager.is_instance_active[instance_id] = 0;
    fifo_manager.active_instances--;
    return true;
  }
  return false;
}

/**
 * @brief Enqueue a buffer address to the FIFO for a specific instance.
 *
 * This function adds a buffer address to the specified FIFO instance for transmission.
 * It validates the instance_id before performing the enqueue operation.
 * When a new buffer address is enqueued, it wakes up HSS if it was sleeping.
 *
 * @param instance_id ID of the FIFO instance (connection ID)
 * @param buffer_addr Pointer to the buffer address to be enqueued
 */
bool enqueue_to_fifo(uint8_t instance_id, void *buffer_addr)
{
  buffer_fifo_t *fifo = get_shared_fifo_ptr(instance_id);
  if (!fifo || !buffer_addr ) {
    return false;
  }

  uint32_t next_write_index = (fifo->write_index + 1) & HSS_LPWSS_FIFO_MASK;
  if (next_write_index == (fifo->read_index & HSS_LPWSS_FIFO_MASK)) {
    return false; // FIFO is full, cannot enqueue
  }
  // Enqueue the buffer address
  fifo->data_buffer[fifo->write_index] = (uint32_t)buffer_addr;
  fifo->write_index = next_write_index;
  //performing pointer operations to update the global buffer address
  uint32_t *riscv_addr_ptr = (uint32_t *)fifo_manager.fifo_instances[instance_id].riscv_addr;
  *riscv_addr_ptr = (uint32_t)&fifo_manager.fifo_instances[instance_id];
  return true;
}

/**
 * @brief Dequeue a buffer from the FIFO for a specific instance.
 *
 * This function removes a buffer from the specified FIFO instance.
 * It validates the instance_id before performing the dequeue operation.
 *
 * @param instance_id ID of the FIFO instance (connection ID)
 * @return true if dequeue operation was successful, false otherwise
 */
bool dequeue_from_fifo(uint8_t instance_id)
{
  buffer_fifo_t *fifo = get_shared_fifo_ptr(instance_id);
  if (!fifo) {
    return false;     // Invalid instance
  }

  if ((fifo->read_index & HSS_LPWSS_FIFO_MASK) == (fifo->write_index & HSS_LPWSS_FIFO_MASK)) {
    uint32_t *riscv_addr_ptr = (uint32_t *)fifo->riscv_addr;
    *riscv_addr_ptr = 0;
    return false;     // FIFO is empty, nothing to dequeue
  }

  linear_buffer_t *buffer = (linear_buffer_t *)(fifo->data_buffer[fifo->read_index]);
  fifo->read_index = (fifo->read_index + 1) & HSS_LPWSS_FIFO_MASK;
  //performing pointer operations to reset the address in the FIFO instance
  if (verify_buffer_and_free(instance_id, buffer)) {
    return true;
  }
  return false;
}

/**
 * @brief Implementation of create_linear_buffer.
 *
 * This function is the actual implementation that allocates a new
 * linear buffer from the pool, initializes it, and returns it.
 *
 * @return Pointer to the newly created linear buffer or NULL if pool is full
 */
void *data_path_create_linear_buffer(void)
{
  for (uint32_t i = 0; i < LINEAR_BUFFER_POOL_SIZE; ++i) {
    if (linear_buffer_pool[i].length == 0) {
      linear_buffer_pool[i].length = DEFAULT_BUFFER_SIZE;
      linear_buffer_pool[i].data = (uint32_t)(&data_pool[i][0]);
      return &linear_buffer_pool[i];
    }
  }
  return NULL;  // Pool is full
}

/**
 * @brief Verify buffer integrity and free it for a specific instance.
 *
 * This function checks if the buffer belongs to the specified FIFO instance,
 * verifies its integrity, and frees it for reuse.
 *
 * @param instance_id ID of the FIFO instance (connection ID)
 * @param buffer_addr Address of the buffer to verify and freea
 * @return true if verification and free operation was successful, false otherwise
 */
bool verify_buffer_and_free(uint8_t instance_id, void *buffer_addr)
{
  buffer_fifo_t *fifo = get_shared_fifo_ptr(instance_id);
  if (!fifo || !buffer_addr) {
    return false;
  }
  for (uint8_t i = 0; i < LINEAR_BUFFER_POOL_SIZE; i++) {
    if ((uint32_t)&linear_buffer_pool[i] == (uint32_t)buffer_addr) {
      linear_buffer_pool[i].length = 0;
      linear_buffer_pool[i].data = 0;
      return true;
    }
  }
  return false;
}

/**
 * @brief Get an available (unused) FIFO instance ID.
 *
 * This function searches for an unused FIFO instance and returns its ID.
 * It's used when a new connection needs to be established but the exact
 * instance ID doesn't matter.
 *
 * @return Available instance ID or 0xFF if no instances are available
 */
uint8_t get_free_fifo_instance_id(void)
{
  for (uint8_t id = 0; id < MAX_CONNECTION_INSTANCES; id++) {
    if (!fifo_manager.is_instance_active[id]) {
      return id;
    }
  }
  return 0xFF;  // No free instances
}

/**
 * @brief Check if a specific FIFO instance is active.
 *
 * This function checks if a particular FIFO instance is currently in use.
 *
 * @param instance_id ID of the FIFO instance to check
 * @return true if the instance is active, false otherwise
 */
bool is_fifo_instance_active(uint8_t instance_id)
{
  if (instance_id < MAX_CONNECTION_INSTANCES) {
    return (fifo_manager.is_instance_active[instance_id] != 0);
  }
  return false;
}

/**
 * @brief Get the count of active FIFO instances.
 *
 * This function returns the number of currently active FIFO instances.
 *
 * @return Number of active FIFO instances
 */
uint8_t get_active_fifo_instance_count(void)
{
  return fifo_manager.active_instances;
}

// Sleep/wakeup functions will be implemented in the future
// - check_and_sleep_if_no_data()
// - sleep_hss()
// - wake_hss()
// - init_hss_sleep_mechanism()

void reset_riscv_addr(uint8_t instance_id)
{
  buffer_fifo_t *fifo = get_shared_fifo_ptr(instance_id);
  if ((fifo->read_index & HSS_LPWSS_FIFO_MASK) == (fifo->write_index & HSS_LPWSS_FIFO_MASK)) {
    uint32_t *riscv_addr_ptr = (uint32_t *)fifo_manager.fifo_instances[instance_id].riscv_addr;
    *riscv_addr_ptr = 0;
    return;
  }
  return;
}

void init_fifo_manager(void)
{
  memset(&fifo_manager, 0, sizeof(buffer_fifo_manager_t));
  // Initialize all FIFO instances but keep them inactive
  for (uint8_t id = 0; id < MAX_CONNECTION_INSTANCES; id++) {
    fifo_manager.fifo_instances[id].read_index = 0;
    fifo_manager.fifo_instances[id].write_index = 0;
    fifo_manager.fifo_instances[id].riscv_addr = 0;
    memset(fifo_manager.fifo_instances[id].data_buffer, 0,
           sizeof(fifo_manager.fifo_instances[id].data_buffer));
  }
  return;
}
