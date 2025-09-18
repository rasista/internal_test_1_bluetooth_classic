// data_path_test.h
#ifndef DATA_PATH_TEST_H
#define DATA_PATH_TEST_H

#include <stdint.h>
#include "../data_path.h"
//TODO: need to build and test this file
// Test constants
#define TEST_INSTANCE_ID 0
#define TEST_INSTANCE_ID_2 1
#define TEST_BUFFER_SIZE 32

// Global variable declaration
extern volatile uint32_t global_address;

// Test function declarations
void test_init_and_instance_management(void);
void test_buffer_creation_and_free(void);
void test_fifo_enqueue_dequeue(void);
void test_fifo_full_condition(void);
void test_multi_instance(void);

#endif // DATA_PATH_TEST_H
