//! implemets different functions for working with TCP sokets
#include "tcp.h"
//! Function for creating a TCP soket
//! @param[in] port - port number
//! @return - soket descriptor
int soket;
int create_tcp_soket(int port)
{
  int option = 1;
  int soket  = -1;
  while (soket < 0) {
    soket = socket(AF_INET, SOCK_STREAM, 0);
    if (soket < 0) {
      perror("socket");
      sleep(1);
    }
  }
  setsockopt(soket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  struct sockaddr_in addr;
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(soket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return -1;
  }

  if (listen(soket, 1) < 0) {
    perror("listen");
    return -1;
  }

  return soket;
}

//! Function for accepting a connection
//! @param[in] soket - soket descriptor
//! @return - accepted soket descriptor
int accept_tcp_soket(int soket)
{
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  int accepted_soket = accept(soket, (struct sockaddr *)&addr, &addr_len);
  if (accepted_soket < 0) {
    perror("accept");
    return -1;
  }

  return accepted_soket;
}

//! Function for connecting to a server
//! @param[in] ip - server IP address
//! @param[in] port - server port number
//! @return - soket descriptor
int connect_to_server(const char *ip, int port)
{
  int status = -1;
  int soket  = -1;
  while (soket < 0) {
    soket = socket(AF_INET, SOCK_STREAM, 0);
    if (soket < 0) {
      perror("socket");
      sleep(1);
    }
  }

  struct sockaddr_in addr;
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  while (status != 0) {
    status = connect(soket, (struct sockaddr *)&addr, sizeof(addr)) < 0;
    perror("connect");
    sleep(1);
  }
  printf("Connection Complete\n");

  return soket;
}

//! Function for sending data
//! @param[in] soket - soket descriptor
//! @param[in] data - data to send
//! @param[in] size - data size
//! @return - number of sent bytes
int send_data(int soket, const void *data, int size)
{
  int sent = send(soket, data, size, 0);
  if (sent < 0) {
    perror("send");
    return -1;
  }

  return sent;
}

//! Function for receiving data
//! @param[in] soket - soket descriptor
//! @param[out] data - buffer for received data
//! @param[in] size - buffer size
//! @return - number of received bytes
int receive_data(int soket, void *data, int size)
{
  int received = recv(soket, data, size, 0);
  if (received <= 0) {
    perror("recv");
    return -1;
  }

  return received;
}

//! Function for closing a soket
//! @param[in] soket - soket descriptor
void close_soket(int soket)
{
  close(soket);
}

//! main function to unit test the TCP sokets
//! function takes 2 arguments, port number and role. Role can be either "server" or "client"
//! server opens the port on local host and wait for connection. once connectionestablished,
//! it sends a Server hello and waits for client hello. Once client hello is received
//1 client connets to server and waits for server hello. Once server hello is received, it sends client hello
int __attribute__((weak)) main(int argc, char *argv[])
{
  if (argc != 3) {
    printf("Usage: %s <port> <server|client>\n", argv[0]);
    return 1;
  }

  int port = atoi(argv[1]);
  if (port <= 0 || port > 65535) {
    printf("Invalid port number\n");
    return 1;
  }

  if (strcmp(argv[2], "server") == 0) {
    int soket = create_tcp_soket(port);
    if (soket < 0) {
      return 1;
    }

    int accepted_soket = accept_tcp_soket(soket);
    if (accepted_soket < 0) {
      return 1;
    }

    char buffer[1024];
    int received = receive_data(accepted_soket, buffer, sizeof(buffer));
    if (received < 0) {
      return 1;
    }

    buffer[received] = '\0';
    printf("Received: %s\n", buffer);

    int sent = send_data(accepted_soket, "Server hello", 12);
    if (sent < 0) {
      return 1;
    }

    close_soket(accepted_soket);
    close_soket(soket);
  } else if (strcmp(argv[2], "client") == 0) {
    int soket = connect_to_server("127.0.0.1", port);
    if (soket < 0) {
      return 1;
    }
    int sent = send_data(soket, "Client hello", 12);
    if (sent < 0) {
      return 1;
    }
    char buffer[1024];
    int received = receive_data(soket, buffer, sizeof(buffer));
    if (received < 0) {
      return 1;
    }
    printf("Received: %s\n", buffer);
  } else {
    printf("Invalid role\n");
    return 1;
  }
}
