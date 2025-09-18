#ifndef __UART_H__
#define __UART_H__
//! File provides public APIs present in uart.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

// UART file descriptor
extern int uart_fd;
// Function to open UART port
int uart_open(const char *port, int baudrate);
// Function to close UART port
void uart_close();
// Function to write data to UART port
int uart_write(const uint8_t *data, int len);
// Function to read data from UART port
int16_t uart_read(uint8_t *data);
#endif