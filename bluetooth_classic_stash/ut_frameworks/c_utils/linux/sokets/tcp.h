#ifndef __TCP_H__
#define __TCP_H__
#include <stdint.h>
#include <stdio.h>
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"

int create_tcp_soket(int port);
int accept_tcp_soket(int soket);
int connect_to_server(const char* ip, int port);
int send_data(int soket, const void* data, int size);
int receive_data(int soket, void* data, int size);
void close_soket(int socket);

#endif