#include "simulation_interface.h"
#include "generic_command_handling.h"
#include <arpa/inet.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

FILE *fp = NULL;
FILE *output_ptr = NULL;

void prepare_cmd_args(uint8_t *uart_buff);
uint8_t response_buffer[1024];
uint32_t response_buffer_len = 0;

void cmd_response(const uint8_t *response_str)
{
  size_t response_length = strlen((const char*)response_str);
  memcpy(&response_buffer[response_buffer_len], response_str, response_length);
  response_buffer_len += response_length;
}

void signal_handler(int sig)
{
  printf("Terminated by Signal : %d\n", sig);
  fclose(output_ptr);
  exit(0);
}

void udpread(int udp_sock, char *buffer, struct sockaddr_in *addr,
             socklen_t *len)
{
  unsigned int n;
  n = recvfrom(udp_sock, buffer, 1024, MSG_WAITALL, (struct sockaddr *)addr,
               len);
  buffer[n] = '\0';
}

int main(int argc, char *argv[])
{
  // Socket variables
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  char buffer[1024];
  char result_str[100] = { 0 };
  int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  int true = 1;
  setsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));
  output_ptr = stdout;

  if (argc == 2) {
    output_ptr = fopen((char *)argv[1], "w");
  }

  // Configure socket addresses
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(5005);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  registering_signals();

  // Bind socket to address
  if (0 == bind(udp_sock, (const struct sockaddr *)&addr, sizeof(addr))) {
    while (1) {
      udpread(udp_sock, buffer, &addr, &len);
      sim_printf("Command Rcvd : %s", buffer);
      // Process the command and populate response_buffer
      prepare_cmd_args((uint8_t *)buffer);
      // Set response
      if (strlen((char *)response_buffer)) {
        strcpy(result_str, (char *)response_buffer);
      } else {
        strcpy(result_str, "OK");
      }
      sim_printf("Command Sent : %s ", result_str);
      // Send response
      if (sendto(udp_sock, (const char *)result_str, strlen(result_str),
                 MSG_CONFIRM, (const struct sockaddr *)&addr, len) < 0) {
        printf("Unable to send message\n");
      }
      if (!strcmp(result_str, "SimulationEnded\n")) {
        return 1;
      }
    }
  } else {
    perror("Failed to bind, port is already in use");
  }
  return 1;
}

/**
 * @brief Function to register termination signals to this process
 *
 */
void registering_signals()
{
  for (int i = 1; i < __SIGRTMAX; i++) {
    signal(i, signal_handler);
  }
  return;
}
