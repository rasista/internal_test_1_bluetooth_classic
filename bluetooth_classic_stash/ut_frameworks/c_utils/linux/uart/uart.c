/***
 * File implements UART read and write functions for Linux
*/

#include "uart.h"
#include "stdint.h"
// UART file descriptor
int uart_fd = -1;

// UART configuration
struct termios uart_config;

// Function to open UART port
int uart_open(const char *port, int baudrate)
{
    // Open UART port
    uart_fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd == -1)
    {
        printf("Error - Unable to open UART port");
        while(1);
        return -1;
    }

    // Configure UART port
    tcgetattr(uart_fd, &uart_config);
    switch (baudrate)
    {
    case 9600:
        cfsetispeed(&uart_config, B9600);
        cfsetospeed(&uart_config, B9600);
        break;
    case 19200:
        cfsetispeed(&uart_config, B19200);
        cfsetospeed(&uart_config, B19200);
        break;
    case 38400:
        cfsetispeed(&uart_config, B38400);
        cfsetospeed(&uart_config, B38400);
        break;
    case 57600:
        cfsetispeed(&uart_config, B57600);
        cfsetospeed(&uart_config, B57600);
        break;
    case 115200:
        cfsetispeed(&uart_config, B115200);
        cfsetospeed(&uart_config, B115200);
        break;
    default:
        perror("Error - Invalid baudrate");
        return -1;
    }
    uart_config.c_cflag |= (CLOCAL | CREAD);
    uart_config.c_cflag &= ~PARENB;
    uart_config.c_cflag &= ~CSTOPB;
    uart_config.c_cflag &= ~CSIZE;
    uart_config.c_cflag |= CS8;
    uart_config.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    uart_config.c_iflag &= ~(IXON | IXOFF | IXANY);
    uart_config.c_oflag &= ~OPOST;
    uart_config.c_cc[VMIN] = 1;
    uart_config.c_cc[VTIME] = 0;
    tcsetattr(uart_fd, TCSANOW, &uart_config);

    return 0;
}

// Function to close UART port
void uart_close()
{
    close(uart_fd);
}

// Function to write data to UART port
int uart_write(const uint8_t *data, int len)
{
    int count = write(uart_fd, data, len);
    if (count < 0)
    {
        perror("Error - Unable to write to UART port");
        return -1;
    }
    return count;
}

// Function to read data from UART port
// Function to read data from UART port
int16_t uart_read(uint8_t *data)
{
    int16_t count;

    do {
        count = read(uart_fd, data, 1024);
    } while (count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK));

    if (count < 0)
    {
        printf("Error - Unable to read from UART port");
        while(1);
        return -1;
    }

    return count;
}

//! implement a weak main function to test the UART functions
//! this function will be overridden by the test code
//! function should take uart port as argument
int __attribute__((weak)) main(int argc, char *argv[])
{
    uint8_t data[] = {0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    // Check if UART port was passed as a command-line argument
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <UART port>\n", argv[0]);
        return -1;
    }

    // Open UART port
    if (uart_open(argv[1], 115200) < 0)
    {
        return -1;
    }

    printf("UART port opened successfully\n");
    // Write data to UART port
    if (uart_write((uint8_t*)data, sizeof(data)) < 0)
    {
        return -1;
    }
    printf("Data written to UART port\n");
    // Read data from UART port
    char buffer[1000];
    if (uart_read((uint8_t*)buffer) < 0)
    {
        return -1;
    }
    printf("Received data: %s\n", buffer);

    // Close UART port
    uart_close();

    return 0;
}
