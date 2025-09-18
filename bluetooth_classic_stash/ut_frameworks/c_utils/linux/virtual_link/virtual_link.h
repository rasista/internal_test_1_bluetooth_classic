#ifndef __VIRTUAL_LINK_H__
#define __VIRTUAL_LINK_H__
#include <stdint.h>

typedef void(vitual_link_received_done_handler_t)(uint8_t *data, uint8_t length);

void *virtual_link_loop(void *arg);
void virtual_link_api_listen_connection(uint16_t port);
void virtual_link_api_cancel_listen();
void virtual_link_api_connect(uint8_t ip_0, uint8_t ip_1, uint8_t ip_2, uint8_t ip_3, uint16_t port);
void virtual_link_api_cancel_connect();
int virtual_link_api_send(uint8_t *data_buf, uint8_t len);
void ulc_send_acl_tx(uint8_t *pkt, uint8_t pkt_length);
void virtual_link_register_receive_cb(vitual_link_received_done_handler_t *receive_done_callback);
void killFirmware();
#endif
