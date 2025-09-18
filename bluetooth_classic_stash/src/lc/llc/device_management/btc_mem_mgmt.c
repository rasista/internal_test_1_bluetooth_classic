/**
 * @file btc_mem_mgmt.c
 * @author  (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-05
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#include "btc_mem_mgmt.h"

// Global array to hold the buffers
static uint32_t  buffer_pool[(NUM_BUFFERS * BUFFER_SIZE) / 4]; // To make buffer address 32 bit aligned.

// Array to keep track of buffer allocation status
static bool buffer_allocated[NUM_BUFFERS];

// Free list to keep track of available buffers
static uint32_t  free_list[NUM_BUFFERS];
static uint32_t  free_list_head = 0;

// Initialize the buffer pool
void init_buffer_pool()
{
  for (uint32_t  i = 0; i < NUM_BUFFERS; i++) {
    buffer_allocated[i] = false;
    free_list[i] = i;
  }
  free_list_head = 0;
}

// Allocate a buffer from the pool
char* allocate_buffer()
{
  if (free_list_head < NUM_BUFFERS) {
    uint32_t  index = free_list[free_list_head++];
    buffer_allocated[index] = true;
    return (char*)&buffer_pool[index * BUFFER_SIZE];
  }
  // No available buffer
  return NULL;
}

// Free a previously allocated buffer
void free_buffer(char* buffer)
{
  int32_t  index = (buffer - (char*)buffer_pool) / BUFFER_SIZE;
  if (index >= 0 && index < NUM_BUFFERS && buffer_allocated[index]) {
    buffer_allocated[index] = false;
    free_list[--free_list_head] = index;
  } else {
    // Buffer not found or already freed
  }
}
