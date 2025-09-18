#ifndef __VIRTUAL_LINK_PRIVATE_H__
#define __VIRTUAL_LINK_PRIVATE_H__
#include <stdint.h>
#include "thread.h"
#include "btc_plt_deps.h"

#define MAX_BUFF_SIZE 1024

enum virtual_link_command {
  VIRTUAL_LINK_COMMAND_LISTEN_CONNECTION = 0x00,
  VIRTUAL_LINK_COMMAND_CANCEL_LISTEN = 0x01,
  VIRTUAL_LINK_COMMAND_CONNECT = 0x02,
  VIRTUAL_LINK_COMMAND_CANCEL_CONNECT = 0x03,
  VIRTUAL_LINK_COMMAND_SEND_DATA = 0x04,
  VIRTUAL_LINK_COMMAND_FREE_SENT_DATA = 0x05
};

#define SET_VIRTUAL_LINK_COMMAND(virtual_link, command) \
  btc_plt_acquire_lock(&virtual_link.command_lock);     \
  (virtual_link.command_bitmap |= (1 << command));      \
  btc_plt_release_lock(&virtual_link.command_lock);

#define CLEAR_VIRTUAL_LINK_COMMAND(virtual_link, command) \
  btc_plt_acquire_lock(&virtual_link.command_lock);       \
  (virtual_link.command_bitmap &= ~(1 << command));       \
  btc_plt_release_lock(&virtual_link.command_lock);

typedef struct virtual_link_s {
  uint32_t command_bitmap;
  btc_pend_t command_pend;
  btc_lock_t command_lock;
  uint32_t connect_port;
  uint8_t connect_ip[4];
  pthread_t listen_connection_thread;
  int32_t connection_socket;
  vitual_link_received_done_handler_t *receive_done_callback;
} virtual_link_t;

typedef void(cmd_complete_handler_t)(void *ctx, uint8_t status);

void *listen_connection_task(void *cmd_complete_handler);
void *connect_task(void *cmd_complete_handler);
void *receive_data_task(void*);

void virtual_link_listen(cmd_complete_handler_t *cmd_complete_handler);
void virtual_link_connect(cmd_complete_handler_t *cmd_complete_handler);
void virtual_link_cancel_listen(cmd_complete_handler_t *cmd_complete_handler);
void virtual_link_cancel_connect(cmd_complete_handler_t *cmd_complete_handler);

#endif
