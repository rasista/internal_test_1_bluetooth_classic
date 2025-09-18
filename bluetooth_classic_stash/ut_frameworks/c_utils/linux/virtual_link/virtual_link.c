//! file implements mocked functions for usched and LPW

#include "virtual_link.h"
#include "btc.h"
#include "hci.h"
#include "string.h"
#include "tcp.h"
#include "usched_vendor_specific.h"
#include "virtual_link_private.h"
#include "ulc.h"

virtual_link_t virtual_link;

/**
 * @brief Registers a callback handler for received data events.
 *
 * This function allows the top layer to register a callback handler that will
 * be invoked whenever data is received over the virtual link. By registering
 * a callback, the virtual link becomes reusable and decoupled from specific
 * implementations like UTF.
 *
 * @param receive_done_callback Pointer to the callback function that will handle
 *                              received data events. The callback should have
 *                              the signature defined by `vitual_link_received_done_handler_t`.
 *
 * @note The registered callback is stored in the `virtual_link` structure and
 *       invoked whenever data is received in the `receive_data_task`.
 */
void virtual_link_register_receive_cb(vitual_link_received_done_handler_t *receive_done_callback)
{
  virtual_link.receive_done_callback = receive_done_callback;
}

void virtual_link_api_listen_connection(uint16_t port)
{
  BTC_PRINT("Listening for connection..\n");
  virtual_link.connect_port = port;
  SET_VIRTUAL_LINK_COMMAND(virtual_link,
                           VIRTUAL_LINK_COMMAND_LISTEN_CONNECTION);
  btc_plt_remove_from_pend(&virtual_link.command_pend);
}

void virtual_link_api_cancel_listen()
{
  BTC_PRINT("Cancel listening..\n");
  SET_VIRTUAL_LINK_COMMAND(virtual_link, VIRTUAL_LINK_COMMAND_CANCEL_LISTEN);
  btc_plt_remove_from_pend(&virtual_link.command_pend);
}

void virtual_link_api_connect(uint8_t ip_0, uint8_t ip_1, uint8_t ip_2,
                              uint8_t ip_3, uint16_t port)
{
  BTC_PRINT("Connecting..\n");
  //! fill in the IP and port
  virtual_link.connect_ip[0] = ip_0;
  virtual_link.connect_ip[1] = ip_1;
  virtual_link.connect_ip[2] = ip_2;
  virtual_link.connect_ip[3] = ip_3;
  virtual_link.connect_port = port;
  SET_VIRTUAL_LINK_COMMAND(virtual_link, VIRTUAL_LINK_COMMAND_CONNECT);
  btc_plt_remove_from_pend(&virtual_link.command_pend);
}

int virtual_link_api_send(uint8_t *data, uint8_t length)
{
  BTC_PRINT("Data being sent ");
  for (int i = 0; i < length; i++) {
    if (i != length - 1) {
      BTC_PRINT(" %d ", data[i]);
    } else {
      BTC_PRINT(" %d \n", data[i]);
    }
  }
  return send_data(virtual_link.connection_socket, data, length);
}

void virtual_link_api_cancel_connect()
{
  SET_VIRTUAL_LINK_COMMAND(virtual_link, VIRTUAL_LINK_COMMAND_CANCEL_CONNECT);
  btc_plt_remove_from_pend(&virtual_link.command_pend);
}

void *listen_connection_task(void *cmd_complete_handler)
{
  cmd_complete_handler_t *cmd_complete_handler_ptr = (cmd_complete_handler_t *)cmd_complete_handler;
  //! create a TCP soket
  int32_t socket = create_tcp_soket(virtual_link.connect_port);

  if (socket < 0) {
    printf("Soket created\n");
    perror("create_tcp_soket");
    return NULL;
  }
  //! accept a connection
  printf("Listening..\n");
  int32_t accepted_soket = accept_tcp_soket(socket);
  if (accepted_soket < 0) {
    perror("accept_tcp_soket");
    return NULL;
  }
  close(socket);
  virtual_link.connection_socket = accepted_soket;
  printf("Accepted Connection\n");

  sleep(1);
  thread_create(&virtual_link.listen_connection_thread, receive_data_task,
                &socket);
  cmd_complete_handler_ptr(NULL, 0);
  return NULL;
}

void *connect_task(void *cmd_complete_handler)
{
  cmd_complete_handler_t *cmd_complete_handler_ptr = (cmd_complete_handler_t *)cmd_complete_handler;
  //! connect to the IP and port
  char ip_array[32] = { 0 };
  snprintf(ip_array, 32, "%d.%d.%d.%d", virtual_link.connect_ip[0],
           virtual_link.connect_ip[1], virtual_link.connect_ip[2],
           virtual_link.connect_ip[3]);
  int socket = connect_to_server(ip_array, virtual_link.connect_port);
  if (socket < 0) {
    perror("connect_to_server");
    return NULL;
  }
  BTC_PRINT("Connected on socket %d\n", socket);
  virtual_link.connection_socket = socket;
  sleep(1);
  thread_create(&virtual_link.listen_connection_thread, receive_data_task,
                &socket);
  cmd_complete_handler_ptr(NULL, 0);
  return NULL;
}

void *receive_data_task(void *socket)
{
  //! read data sent over the tcp connection established
  uint8_t local_buf_array_read[MAX_BUFF_SIZE] = { 0 };
  int received_pkt_len = 0;
  while (1) {
    BTC_PRINT("will receive data on %d\n", virtual_link.connection_socket);
    received_pkt_len = receive_data(virtual_link.connection_socket,
                                    local_buf_array_read, MAX_BUFF_SIZE);
    if (received_pkt_len > 0) {
      BTC_HEX_DUMP("TCP RX", local_buf_array_read, received_pkt_len);
      if (virtual_link.receive_done_callback == NULL) {
        BTC_ASSERT(NULL_POINTER);
      } else {
        virtual_link.receive_done_callback(local_buf_array_read, received_pkt_len);
      }
    } else {
      BTC_PRINT("receive data failed\n");
      break;
    }
  }
  return NULL;
}

void virtual_link_listen(cmd_complete_handler_t *cmd_complete_handler)
{
  thread_create(&virtual_link.listen_connection_thread, listen_connection_task, cmd_complete_handler);
  pthread_join(virtual_link.listen_connection_thread, NULL);
}

void virtual_link_connect(cmd_complete_handler_t *cmd_complete_handler)
{
  thread_create(&virtual_link.listen_connection_thread, connect_task, cmd_complete_handler);
  pthread_join(virtual_link.listen_connection_thread, NULL);
}

void virtual_link_cancel_listen(cmd_complete_handler_t *cmd_complete_handler)
{
  //! cancel the listen
  close_soket(virtual_link.connection_socket);
  pthread_cancel(virtual_link.listen_connection_thread);
  cmd_complete_handler(NULL, 0);
}

void virtual_link_cancel_connect(cmd_complete_handler_t *cmd_complete_handler)
{
  //! cancel the connection
  close_soket(virtual_link.connection_socket);
  pthread_cancel(virtual_link.listen_connection_thread);
  cmd_complete_handler(NULL, 0);
}

void *virtual_link_loop(void *ctx)
{
  btc_plt_create_pend(&virtual_link.command_pend);
  btc_plt_create_lock(&virtual_link.command_lock);
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  //! create a pend on the virtual link command bitmap
  while (1) {
    BTC_PRINT("Running virtual_link_loop\n");
    while (virtual_link.command_bitmap) {
      if (virtual_link.command_bitmap
          & (1 << VIRTUAL_LINK_COMMAND_LISTEN_CONNECTION)) {
        //! listen for a connection
        virtual_link_listen(ulc_ptr->cmd_complete_handler);
        // btc_plt_remove_from_pend(&virtual_link.read_start);
        CLEAR_VIRTUAL_LINK_COMMAND(virtual_link,
                                   VIRTUAL_LINK_COMMAND_LISTEN_CONNECTION);
      }
      if (virtual_link.command_bitmap
          & (1 << VIRTUAL_LINK_COMMAND_CANCEL_LISTEN)) {
        virtual_link_cancel_listen(ulc_ptr->cmd_complete_handler);
        //! cancel the listen
        CLEAR_VIRTUAL_LINK_COMMAND(virtual_link,
                                   VIRTUAL_LINK_COMMAND_CANCEL_LISTEN);
      }
      if (virtual_link.command_bitmap & (1 << VIRTUAL_LINK_COMMAND_CONNECT)) {
        virtual_link_connect(ulc_ptr->cmd_complete_handler);
        //! connect to the IP and port
        CLEAR_VIRTUAL_LINK_COMMAND(virtual_link, VIRTUAL_LINK_COMMAND_CONNECT);
      }
      if (virtual_link.command_bitmap
          & (1 << VIRTUAL_LINK_COMMAND_CANCEL_CONNECT)) {
        virtual_link_cancel_connect(ulc_ptr->cmd_complete_handler);
        //! cancel the connection
        CLEAR_VIRTUAL_LINK_COMMAND(virtual_link,
                                   VIRTUAL_LINK_COMMAND_CANCEL_CONNECT);
      }
    }
    btc_plt_add_to_pend(&virtual_link.command_pend);
  }
}
