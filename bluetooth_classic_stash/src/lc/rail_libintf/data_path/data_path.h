#ifndef DATA_PATH_H
#define DATA_PATH_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "debug.h"  /* For BTC_PRINT */

// Macros for buffer
#define DEFAULT_BUFFER_SIZE 1028 // 1021 is payload size, 2 bytes for packet header, 2 bytes for payload header, 3 bytes for padding
// FIFO size must be a power of two, and the number of buffers in the pool matches the FIFO size.
// The FIFO can hold exactly HSS_LPWSS_FIFO_SIZE buffers, using bitwise AND for wrap-around.
#define HSS_LPWSS_FIFO_SIZE 4  // Must be power of 2
#define HSS_LPWSS_FIFO_MASK (HSS_LPWSS_FIFO_SIZE - 1)

// Maximum number of connection instances supported (ACL/SCO)
#define MAX_CONNECTION_INSTANCES 4

// Total buffer pool size across all instances
#define LINEAR_BUFFER_POOL_SIZE (HSS_LPWSS_FIFO_SIZE * MAX_CONNECTION_INSTANCES)

/**
 * @brief Linear buffer structure containing metadata and data
 */
typedef struct {
  uint32_t length;
  uint32_t data;                 /**< Data portion of the buffer (address) */
} linear_buffer_t;

/**
 * @brief FIFO structure for a single connection instance
 */
typedef struct {
  uint32_t read_index;           /**< Current read position */
  uint32_t write_index;          /**< Current write position */
  uint32_t data_buffer[HSS_LPWSS_FIFO_SIZE]; /**< Array of buffer addresses */
  uint32_t riscv_addr;           /**< RISC-V address for this FIFO */
} buffer_fifo_t;

/**
 * @brief FIFO manager structure for multiple instances
 */
typedef struct {
  buffer_fifo_t fifo_instances[MAX_CONNECTION_INSTANCES];   /**< Multiple FIFO instances (ACL/SCO connections) */
  uint8_t active_instances;                                 /**< Number of active connections */
  uint8_t is_instance_active[MAX_CONNECTION_INSTANCES];
} buffer_fifo_manager_t;

// Global variable declarations
extern linear_buffer_t linear_buffer_pool[LINEAR_BUFFER_POOL_SIZE]; /**< Pool of linear buffers */
extern uint32_t data_pool[LINEAR_BUFFER_POOL_SIZE][DEFAULT_BUFFER_SIZE / 4]; /**< Data pool for linear buffers */
extern buffer_fifo_manager_t fifo_manager;  /**< Manager for multiple FIFOs */

// packet length macros
#define SET_PACKET_LEN(buffer, val)  ((buffer) = (val))

/**
 * @brief Get a pointer to the shared FIFO for a specific instance.
 *
 * @param instance_id ID of the FIFO instance (connection ID)
 * @return Pointer to the specified FIFO instance
 */
buffer_fifo_t *get_shared_fifo_ptr(uint8_t instance_id);

/**
 * @brief Get an available (unused) FIFO instance ID.
 *
 * @return Available instance ID or 0xFF if no instances are available
 */
uint8_t get_free_fifo_instance_id(void);

/**
 * @brief Check if a specific FIFO instance is active.
 *
 * @param instance_id ID of the FIFO instance to check
 * @return true if the instance is active, false otherwise
 */
bool is_fifo_instance_active(uint8_t instance_id);

/**
 * @brief Get the count of active FIFO instances.
 *
 * @return Number of active FIFO instances
 */
uint8_t get_active_fifo_instance_count(void);

/**
 * @brief Deinitialize a FIFO instance when a connection is closed.
 *
 * @param instance_id ID of the instance to deinitialize
 * @return true if deinitialization was successful, false otherwise
 */
bool deinit_fifo_instance(uint8_t instance_id);

/**
 * @brief Enqueue a buffer to the FIFO for a specific instance.
 *
 * @param instance_id ID of the FIFO instance (connection ID)
 * @param buffer_addr Address of the buffer to be enqueued
 */
bool enqueue_to_fifo(uint8_t instance_id, void *buffer_addr);

/**
 * @brief Dequeue a buffer from the FIFO for a specific instance.
 *
 * @param instance_id ID of the FIFO instance (connection ID)
 * @return true if dequeue operation was successful, false otherwise
 */
bool dequeue_from_fifo(uint8_t instance_id);

/**
 * @brief Verify buffer integrity and free it for a specific instance.
 *
 * @param instance_id ID of the FIFO instance (connection ID)
 * @param buffer_addr Address of the buffer to verify and free
 * @return true if verification and free operation was successful, false otherwise
 */
bool verify_buffer_and_free(uint8_t instance_id, void *buffer_addr);

/**
 * @brief Initialize the data path with linear buffers.
 *
 * This function initializes all FIFO instances but leaves them inactive (except instance 0),
 * and allocates the initial set of linear buffers for instance 0. It must be called
 * during system startup before any other data path functions.
 */
bool init_data_path(uint8_t instance_id, uint32_t riscv_memory_indicator_address);

/**
 * @brief Initialize the data path for a specific instance with a given buffer address.
 *
 * @param instance_id The FIFO instance ID to initialize
 * @param buffer_addr The buffer address to use for this instance
 */
void init_data_path_with_instance(uint8_t instance_id, uint32_t buffer_addr);

/**
 * @brief Reset the riscv address value in the FIFO instance.
 *
 * @param instance_id ID of the FIFO instance (connection ID)
 */
void reset_riscv_addr(uint8_t instance_id);

/**
 * @brief Initialize the FIFO manager.
 *
 * This function initializes the FIFO manager structure.
 */
void init_fifo_manager(void);

#endif // DATA_PATH_H
