#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "sl_btc_seq_interface.h"
#include "sl_host_to_lpw_interface.h"
#include "hss_cmd_handler_autogen.h"
#include "rail_simulation.h"
#include "message_decode.h"
#include "btc_plt_deps.h"
#include "sl_btc_seq_interface.h"
btc_dev_t btc_dev[MAX_NUMBER_BTC_DEVICES];
extern uint32_t current_device_index;
shared_mem_t queue;

void hss_cmd_payload_start_procedure_payload_init_btc_device_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    queue.pending_queue.plt_lock.acquire_lock = btc_plt_acquire_lock;
    queue.pending_queue.plt_lock.release_lock = btc_plt_release_lock;
    queue.processed_queue.plt_lock.acquire_lock = btc_plt_acquire_lock;
    queue.processed_queue.plt_lock.release_lock = btc_plt_release_lock;
    // Adding a huge offset for now, this will be provided by host
    queue.metadata_size = PKB_DATA_OFFSET;
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_init_btc_device;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_page_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_page;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_page_scan_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_page_scan;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_inquiry_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_inquiry;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_inquiry_scan_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_inquiry_scan;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_acl_central_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_acl_central;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_acl_peripheral_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_acl_peripheral;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_esco_central_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_esco_central;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_esco_peripheral_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_esco_peripheral;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_roleswitch_central_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_roleswitch_central;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_roleswitch_peripheral_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_roleswitch_peripheral;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_payload_test_mode_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_test_mode;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_page_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_page;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_page_scan_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_page_scan;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_inquiry_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_inquiry;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_encryption_parameters_aes_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_encryption;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_encryption_parameters_e0_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_encryption;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_inquiry_scan_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_inquiry_scan;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_afh_parameters_acl_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_afh_parameters_acl;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_sniff_parameters_acl_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_sniff_parameters_acl;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_device_parameters_acl_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_device_parameters_acl;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_test_mode_params_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_test_mode_params;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_device_parameters_esco_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_device_parameters_esco;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_device_parameters_roleswitch;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_roleswitch_parameters_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_roleswitch_parameters;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_fixed_btc_channel;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_page_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_page;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_inquiry_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_inquiry;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_page_scan_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_page_scan;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_inquiry_scan_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_inquiry_scan;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_acl_central_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_acl_central;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_esco_central_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_esco_central;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_roleswitch_central_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_roleswitch_central;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_acl_peripheral_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_acl_peripheral;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_esco_peripheral_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_esco_peripheral;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_roleswitch_peripheral;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_start_procedure_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    *HSS_CMD_START_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_start_procedure;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_get_procedure_payload_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_set_procedure_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    *HSS_CMD_SET_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_set_procedure;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_stop_procedure_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    *HSS_CMD_CMD_TYPE((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    *HSS_CMD_STOP_PROCEDURE_ID((uint8_t *)(c_out_payload + PKB_DATA_OFFSET)) = hss_cmd_stop_procedure;
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_cmd_payload_get_procedure_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_event_payload_page_complete_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_event_payload_page_scan_complete_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
void hss_event_handler(uint32_t argc, uint8_t *argv[])
{
    char c_out_payload[1024] = {0};
    char c_in_payload[1024] = {0};
    current_device_index = atoi((char*)argv[0]);
    int i = 1;
    int offset = 0;
    while (i < argc) {
        strcpy(c_in_payload + offset, (char*)argv[i]);
        offset += strlen((char*)argv[i]);
        i++;
        offset++;
    }
    c_in_payload[offset] = '\0';
    memset(c_out_payload, '\0', sizeof(char) * 1000);
    sl_btc_seq_interface_command_parser((uint8_t *)(c_out_payload + PKB_DATA_OFFSET), (uint8_t*)c_in_payload);
    pkb_t *pnd_cmd = (pkb_t *)c_out_payload;
    enqueue(&queue.pending_queue, &pnd_cmd->queue_node);
    lpw_mailbox_handler(&queue, &btc_dev[current_device_index]);
}
