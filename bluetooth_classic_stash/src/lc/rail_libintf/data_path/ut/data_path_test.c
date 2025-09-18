#include "data_path_test.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TEST_INSTANCE_ID 0
#define TEST_INSTANCE_ID_2 1
#define TEST_BUFFER_SIZE 32

//need to build and  test this file TODO
volatile uint32_t global_address = 100;
void test_init_and_instance_management() {
    BTC_PRINT("Test: init_data_path and instance management...\n");
    memset(&fifo_manager, 0, sizeof(fifo_manager));
    init_data_path(TEST_INSTANCE_ID, (uint32_t)(uintptr_t)&global_address);
    // init_data_path(TEST_INSTANCE_ID, 0xa001c870UL);
    assert(is_fifo_instance_active(TEST_INSTANCE_ID));
    assert(get_active_fifo_instance_count() == 1);
    assert(deinit_fifo_instance(TEST_INSTANCE_ID));
    assert(!is_fifo_instance_active(TEST_INSTANCE_ID));
    BTC_PRINT("  Passed.\n");
}

void test_buffer_creation_and_free() {
    BTC_PRINT("Test: buffer creation and free...\n");
    memset(linear_buffer_pool, 0, sizeof(linear_buffer_pool));
    void *buf = data_path_create_linear_buffer();
    assert(buf != NULL);
    linear_buffer_t *lbuf = (linear_buffer_t*)buf;
    assert(lbuf->length != 0);
    uint8_t *data = get_buffer_data_ptr(lbuf);
    assert(data != NULL);
    for (int i = 0; i < 10; ++i) data[i] = i;
    SET_PACKET_LEN(lbuf->length, 10);
    init_fifo_instance(TEST_INSTANCE_ID, 0);
    assert(verify_buffer_and_free(TEST_INSTANCE_ID, buf));
    BTC_PRINT("  Passed.\n");
}

void test_fifo_enqueue_dequeue() {
    BTC_PRINT("Test: FIFO enqueue and dequeue...\n");
    memset(&fifo_manager, 0, sizeof(fifo_manager));
    memset(linear_buffer_pool, 0, sizeof(linear_buffer_pool));
    init_fifo_instance(TEST_INSTANCE_ID, 0);
    void *bufs[HSS_LPWSS_FIFO_SIZE-1];
    for (int i = 0; i < HSS_LPWSS_FIFO_SIZE-1; ++i) {
        bufs[i] = data_path_create_linear_buffer();
        assert(bufs[i] != NULL);
        linear_buffer_t *lbuf = (linear_buffer_t*)bufs[i];
        SET_PACKET_LEN(lbuf->length, 10);
        enqueue_to_fifo(TEST_INSTANCE_ID, bufs[i]);
    }
    
    // Dequeue and verify each buffer
    for (int i = 0; i < HSS_LPWSS_FIFO_SIZE-1; ++i) {
        assert(dequeue_from_fifo(TEST_INSTANCE_ID));
    }
    
    // Verify FIFO is empty
    assert(!dequeue_from_fifo(TEST_INSTANCE_ID));
    BTC_PRINT("  Passed.\n");
}

void test_fifo_full_condition() {
    BTC_PRINT("Test: FIFO full condition...\n");
    memset(&fifo_manager, 0, sizeof(fifo_manager));
    memset(linear_buffer_pool, 0, sizeof(linear_buffer_pool));
    init_fifo_instance(TEST_INSTANCE_ID, 0);
    void *bufs[HSS_LPWSS_FIFO_SIZE];
    for (int i = 0; i < HSS_LPWSS_FIFO_SIZE; ++i) {
        bufs[i] = data_path_create_linear_buffer();
        assert(bufs[i] != NULL);
        linear_buffer_t *lbuf = (linear_buffer_t*)bufs[i];
        SET_PACKET_LEN(lbuf->length, 10);
        enqueue_to_fifo(TEST_INSTANCE_ID, bufs[i]);
    }
    buffer_fifo_t *fifo = get_shared_fifo_ptr(TEST_INSTANCE_ID);
    void *extra_buf = data_path_create_linear_buffer();
    if (extra_buf) {
        uint32_t prev_write = fifo->write_index;
        enqueue_to_fifo(TEST_INSTANCE_ID, extra_buf);
        assert(fifo->write_index == prev_write); // Should not change
        (void)prev_write; // Prevent unused variable warning if asserts are disabled
    }
    BTC_PRINT("  Passed.\n");
}

void test_multi_instance() {
    BTC_PRINT("Test: multi-instance support...\n");
    memset(&fifo_manager, 0, sizeof(fifo_manager));
    memset(linear_buffer_pool, 0, sizeof(linear_buffer_pool));
    init_fifo_instance(TEST_INSTANCE_ID, 0);
    init_fifo_instance(TEST_INSTANCE_ID_2, 0);
    void *buf1 = data_path_create_linear_buffer();
    void *buf2 = data_path_create_linear_buffer();
    SET_PACKET_LEN(((linear_buffer_t*)buf1)->length, 10);
    SET_PACKET_LEN(((linear_buffer_t*)buf2)->length, 10);
    enqueue_to_fifo(TEST_INSTANCE_ID, buf1);
    enqueue_to_fifo(TEST_INSTANCE_ID_2, buf2);
    assert(dequeue_from_fifo(TEST_INSTANCE_ID));
    assert(dequeue_from_fifo(TEST_INSTANCE_ID_2));
    BTC_PRINT("  Passed.\n");
}

int main() {
    BTC_PRINT("Running standalone data_path tests...\n");
    test_init_and_instance_management();
    test_buffer_creation_and_free();
    test_fifo_enqueue_dequeue();
    test_fifo_full_condition();
    test_multi_instance();
    BTC_PRINT("All data_path tests passed!\n");
    return 0;
}
