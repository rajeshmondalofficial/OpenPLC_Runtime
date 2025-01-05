//-----------------------------------------------------------------------------
// Copyright 2022 Thiago Alves
// This file is part of the OpenPLC Software Stack.
//
// OpenPLC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenPLC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenPLC.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// This is the file for the network routines of the OpenPLC. It has procedures
// to create a socket and connect to a server.
// Thiago Alves, Nov 2022
//-----------------------------------------------------------------------------

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <sys/select.h>

#include "ladder.h"

#define METHOD_UDP 1
#define METHOD_TCP 0

#define UART_DEVICE "/dev/ttyAMA0"
#define BAUD_RATE 9600

int fd = -1;
int serial_fd = -1;
int listening = 0;
int bytes_received;

// Global Variables
int global_uart_fd = -1;
unsigned char inputData[256];
unsigned char rylr_config_resp[256];
unsigned char rylr_send_resp[256];
pthread_mutex_t uart_mutex;  // Mutex for thread-safe access
unsigned char dataReady = 0; // Flag for new data
int uart_listening = -1;     // Flag for listening UART
char log_msg[1000];
char rylr_message[1024];
int rylr_msg_counter = 0;
int rylr_send_msg_counter = 0;

// RYLR998 Modem Specific Variables
int mode_connection_id = -1;
int mode_mode = -1;

// UART Initialize
void uart_init(uint8_t *device)
{
    if (global_uart_fd < 0)
    {
        // Initialize UART Connection
        global_uart_fd = open(device, O_RDWR | O_NOCTTY);
        sprintf(log_msg, "UART: Connection Initialize: => %d\n", global_uart_fd);
        log(log_msg);
        if (global_uart_fd < 0)
        {
            perror("Error opening UART device");
            sprintf(log_msg, "UART: Connection Failed: => %d\n", global_uart_fd);
            log(log_msg);
        }

        // Configure UART settings
        struct termios options;
        tcgetattr(global_uart_fd, &options);

        options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; // Baud rate: 9600, 8 data bits, no parity, 1 stop bit
        options.c_iflag = IGNPAR;                       // Ignore framing errors
        options.c_oflag = 0;
        options.c_lflag = 0; // Non-canonical mode

        tcflush(global_uart_fd, TCIOFLUSH);           // Flush the input buffer
        tcsetattr(global_uart_fd, TCSANOW, &options); // Apply the configuration

        // Configure UART settings
        // struct termios options;
        // tcgetattr(global_uart_fd, &options);

        // // Input baud rate
        // cfsetispeed(&options, BAUD_RATE);

        // // Output baud rate
        // cfsetospeed(&options, BAUD_RATE);

        // // 8 data bits, 1 stop bit, no parity
        // options.c_cflag &= ~PARENB;
        // options.c_cflag &= ~CSTOPB;
        // options.c_cflag &= ~CSIZE;
        // options.c_cflag |= CS8;

        // // Enable receiver and ignore modem control lines
        // options.c_cflag |= (CLOCAL | CREAD);

        // // Set UART attributes
        // tcsetattr(global_uart_fd, TCSANOW, &options);
        // tcflush(global_uart_fd, TCIOFLUSH);

        // fcntl(global_uart_fd, F_SETFL, 0);
        // Initialize the mutex
        pthread_mutex_init(&uart_mutex, NULL);
    }
}

/** UART Communication Block */
int uart_send(uint8_t *message, uint8_t *device)
{
    char termination[] = {0x0D, 0x0A};

    if (global_uart_fd < 0)
    {
        uart_init(device);
        int write_id = write(global_uart_fd, message, strlen(message));
        write(global_uart_fd, termination, strlen(termination));
        // sprintf(log_msg, "UART: Write to connection: => %s\n", inputData);
        // log(log_msg);
        return global_uart_fd;
    }
    else
    {
        int write_id = write(global_uart_fd, message, strlen(message));
        write(global_uart_fd, termination, strlen(termination));
        // sprintf(log_msg, "UART: Write to connection: => %s\n", inputData);
        // log(log_msg);
        return global_uart_fd;
    }
}

// Listen to UART
void *uart_listener_thread(void *arg)
{
    char buffer[256];
    while (1)
    {
        int bytes_read = read(global_uart_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0'; // Null-terminate the received string
            // Lock the mutex to update shared data
            pthread_mutex_lock(&uart_mutex);
            strncpy(inputData, buffer, sizeof(inputData) - 1);
            // sprintf(log_msg, "UART: Connection Receive: => %s\n", inputData);
            // log(log_msg);
            inputData[sizeof(inputData) - 1] = '\0'; // Safety null-termination
            dataReady = 1;                           // Set flag to indicate data is ready
            pthread_mutex_unlock(&uart_mutex);
        }
        // usleep(1000); // Short sleep to avoid busy waiting
    }
    return NULL;
}

// UART Listen Thread
void start_uart_thread()
{
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, uart_listener_thread, NULL) != 0)
    {
        perror("Failed to create UWslART listener thread");
        uart_listening = -1;
    }
    else
    {
        uart_listening = 0; // Set flag to indicate UART listening
    }
}

// Process Data Received From UART
char *uart_listen(uint8_t *device, uint8_t *message, size_t buffer_size)
{
    if (global_uart_fd < 0)
    {
        uart_init(device);
    }
    if (uart_listening < 0)
    {
        start_uart_thread();
    }

    static char message_data[256]; // Static buffer to return data

    // Print buffer contents (example processing)
    pthread_mutex_lock(&uart_mutex);
    if (dataReady)
    {
        // sprintf(log_msg, "Received UART Data: => %s\n", inputData);
        // log(log_msg);
        // printf("Received UART Data: %s\n", inputData);

        strncpy(message_data, inputData, sizeof(message_data) - 1);
        message_data[sizeof(message_data) - 1] = '\0'; // Ensure null termination

        dataReady = 0; // Reset flag after processing
    }
    else
    {
        message_data[0] = '\0'; // Return an empty string if no data is ready
    }
    pthread_mutex_unlock(&uart_mutex);
    return message_data; // Return the received data as a string
}

int receive_uart_communication(uint8_t *device, uint8_t *message, size_t buffer_size)
{
    if (serial_fd < 0)
    {
        serial_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    }
    if (serial_fd == -1)
    {
        perror("Unable to open serial port");
        exit(1);
    }
    struct termios options;
    tcgetattr(serial_fd, &options);

    // Set baud rate
    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);
    // Configure 8N1 (8 data bits, no parity, 1 stop bit)
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    // Enable receiver and set local mode
    options.c_cflag |= (CLOCAL | CREAD);
    // Set timeout and minimum bytes to read
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 10;
    // Timeout in deciseconds
    // Apply the configuration
    tcsetattr(serial_fd, TCSANOW, &options);

    char log_msg[1000];

    int bytes_read = read(serial_fd, message, buffer_size);
    message[bytes_read] = 0;

    // if(listening == 0) {
    //     uart_listen(message, buffer_size);
    // }

    return bytes_received;
}

int connect_to_tcp_server(uint8_t *ip_address, uint16_t port, int method)
{
    int sockfd, connfd;
    char log_msg[1000];
    struct sockaddr_in servaddr, cli;

    if (method == METHOD_TCP)
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    else if (method == METHOD_UDP)
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1)
    {
        sprintf(log_msg, "TCP Client: error creating TCP socket => %s\n", strerror(errno));
        log(log_msg);
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));

    // Configure socket
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip_address);
    servaddr.sin_port = htons(port);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        sprintf(log_msg, "TCP Client: error connecting to server => %s\n", strerror(errno));
        log(log_msg);
        close(sockfd);
        return -1;
    }

    // Set non-blocking socket
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        sprintf(log_msg, "TCP Client: error reading flags from TCP socket => %s\n", strerror(errno));
        log(log_msg);
        return -1;
    }
    flags = (flags | O_NONBLOCK);
    if (fcntl(sockfd, F_SETFL, flags) != 0)
    {
        sprintf(log_msg, "TCP Client: error setting flags for TCP socket => %s\n", strerror(errno));
        log(log_msg);
        return -1;
    }

    return sockfd;
}

int send_tcp_message(uint8_t *msg, size_t msg_size, int socket_id)
{
    int bytes_sent = 0;
    char log_msg[1000];
    bytes_sent = write(socket_id, msg, msg_size);
    if (bytes_sent < 0)
    {
        sprintf(log_msg, "TCP Client: error sending msg to server => %s\n", strerror(errno));
        log(log_msg);
        return -1;
    }

    return bytes_sent;
}

int receive_tcp_message(uint8_t *msg_buffer, size_t buffer_size, int socket_id)
{
    int bytes_received = 0;
    char log_msg[1000];
    bytes_received = read(socket_id, msg_buffer, buffer_size);

    if (bytes_received < 0 && bytes_received != EAGAIN && bytes_received != EWOULDBLOCK)
    {
        // sprintf(log_msg, "TCP Client: error receiving msg from server => %s\n", strerror(errno));
        // log(log_msg);
        return -1;
    }
    else
    {
        msg_buffer[bytes_received] = 0;
        sprintf(log_msg, "TCP Client: msg from server => %s\n", msg_buffer);
        log(log_msg);
    }

    return bytes_received;
}

int close_tcp_connection(int socket_id)
{
    return close(socket_id);
}

// Codes for Lyra998 Modem Specific Code Blocks
// Get UART Connection for
int get_uart_connection(uint8_t *device, int baud_rate)
{
    if (mode_connection_id < 0)
    {
        mode_connection_id = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
        // Configure UART settings
        struct termios options;
        tcgetattr(mode_connection_id, &options);

        options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; // Baud rate: 9600, 8 data bits, no parity, 1 stop bit
        options.c_iflag = IGNPAR;                       // Ignore framing errors
        options.c_oflag = 0;
        options.c_lflag = 0; // Non-canonical mode

        tcflush(mode_connection_id, TCIOFLUSH); // Flush the input buffer
        tcsetattr(mode_connection_id, TCSANOW, &options);

        pthread_mutex_init(&uart_mutex, NULL);

        return mode_connection_id;
    }
    return mode_connection_id;
}

// RYLR998: This is a common listener for the RYLR998 Modem
void listen_rylr_receive(int connection_id)
{
    if (connection_id < 0)
    {
        log("Couldn't get connection");
        return;
    }
    char buffer[1024];
    int index = 0;
    while (1)
    {
        fd_set read_fds;
        struct timeval timeout;

        FD_ZERO(&read_fds);
        FD_SET(connection_id, &read_fds);

        timeout.tv_sec = 15; // Timeout of 2 seconds
        timeout.tv_usec = 0;

        if (select(connection_id + 1, &read_fds, NULL, NULL, &timeout) > 0)
        {
            if (FD_ISSET(connection_id, &read_fds))
            {
                char temp;
                if (read(connection_id, &temp, 1) > 0)
                {
                    if (temp == '\n')
                    { // End of packet
                        buffer[index] = '\0';
                        sprintf(log_msg, "RYLR: Received Bytes => %s\n", buffer);
                        log(log_msg);
                        index = 0;
                        // IF: RYLR998 Modem is CONFIG Mode
                        if (mode_mode == 1)
                        {
                            strcpy(rylr_config_resp, buffer);
                        }
                        // IF: RYLR998 Modem is Send Mode
                        if (mode_mode == 2)
                        {
                            strcpy(rylr_send_resp, buffer);
                        }
                        // IF: RYLR998 Modem is Receive Mode
                        if (mode_mode == 3)
                        {
                            strcpy(rylr_message, buffer);
                            rylr_msg_counter = rylr_msg_counter + 1;
                        }
                    }
                    else if (index < BUFFER_SIZE - 1)
                    {
                        buffer[index++] = temp;
                    }
                }
            }
        }
        else
        {
            if (mode_mode == 3)
            {
                log("No data received within timeout\n");
                printf("No data received within timeout\n");
            }
        }
    }
}

// RYLR Configuration Block
int rylr998_config(uint8_t *device, int baud_rate, bool read_trigger, bool write_trigger, uint8_t *payload, int mode)
{
    int connection_id = get_uart_connection(device, baud_rate);

    if (connection_id < 0)
    {
        return 0;
    }
    mode_mode = 1;

    pthread_t thread_id;

    // if (uart_listening < 0)
    // {
    //     if (pthread_create(&thread_id, NULL, listen_rylr_receive, connection_id) != 0)
    //     {
    //         perror("Failed to create UWslART listener thread");
    //         uart_listening = -1;
    //     }
    //     else
    //     {
    //         log("RYLR998: Listening for Messages...");
    //         uart_listening = 0; // Set flag to indicate UART listening
    //     }
    // }

    char at_command[256];
    char msg_buffer[1024];
    char numStr[32];
    char log_msg[1024];
    int index = 0;

    // Convert the integer to a string
    if (read_trigger)
    {
        if (mode == 4)
        {
            // AT+IPR
            sprintf(at_command, "AT+IPR?\r\n");
        }
        if (mode == 5)
        {
            sprintf(at_command, "AT+BAND?\r\n");
        }
        if (mode == 7)
        {
            sprintf(at_command, "AT+ADDRESS?\r\n");
        }
    }
    if (write_trigger)
    {
        if (mode == 4)
        {
            // AT+IPR
            sprintf(at_command, "AT+IPR=%s\r\n", payload);
        }
        if (mode == 5)
        {
            // AT+IPR
            sprintf(at_command, "AT+BAND=%s\r\n", payload);
        }
        if (mode == 7)
        {
            sprintf(at_command, "AT+ADDRESS=%s\r\n", payload);
        }
    }
    // sprintf(at_command, "AT+ADDRESS=%u\r\n", frequency);
    if (read_trigger || write_trigger)
    {

        int byte_write = write(connection_id, at_command, strlen(at_command));
        sprintf(log_msg, "RYLR: Write AT Command => %sBytes Write => %d\n", at_command, byte_write);
        log(log_msg);

        // int byte_read = read(connection_id, msg_buffer, sizeof(msg_buffer) - 1);
        // strncpy(rylr_config_resp, msg_buffer, sizeof(rylr_config_resp) - 1);

        // if (byte_read > 0)
        // {

        //     msg_buffer[byte_read] = '\0';
        //     sprintf(log_msg, "RYLR: Received Bytes => %s\n", rylr_config_resp);
        //     log(log_msg);
        //     // tcflush(connection_id, TCIOFLUSH);
        //     return connection_id;
        // }
    }
    return connection_id;
}

char *get_config_response() {
    return rylr_config_resp;
}

int rylr_send(int connection_id, bool trigger, int address, uint8_t *payload_data)
{
    char at_command[256];
    char msg_buffer[256];
    mode_mode = 2;

    // Convert the integer to a string
    sprintf(at_command, "AT+SEND=%u,%d,%s\r\n", address, strlen(payload_data), payload_data);
    if (trigger)
    {
        int byte_write = write(connection_id, at_command, strlen(at_command));
        sprintf(log_msg, "RYLR: Write AT Command => %s\n", at_command);
        log(log_msg);

        int byte_read = read(connection_id, msg_buffer, sizeof(msg_buffer) - 1);
        strncpy(rylr_send_resp, msg_buffer, sizeof(rylr_send_resp) - 1);
        sprintf(log_msg, "RYLR: Received Bytes => %d\n", byte_read);
        log(log_msg);

        if (byte_read > 0)
        {

            msg_buffer[byte_read] = '\0';
            sprintf(log_msg, "RYLR: Received Bytes Message => %s\n", msg_buffer);
            log(log_msg);
            rylr_send_msg_counter = rylr_send_msg_counter + 1;
            // tcflush(connection_id, TCIOFLUSH);
            return 1;
        }
    }
    return -1;
}

int get_rylr_send_msg_counter()
{
    return rylr_send_msg_counter;
}

char *get_send_resp()
{
    return rylr_send_resp;
}

// RYLR Received Block
char *rylr_receive(int connection_id)
{
    pthread_t thread_id;
    mode_mode = 3;

    if (uart_listening < 0)
    {
        if (pthread_create(&thread_id, NULL, listen_rylr_receive, connection_id) != 0)
        {
            perror("Failed to create UWslART listener thread");
            uart_listening = -1;
        }
        else
        {
            log("RYLR998: Listening for Messages...");
            uart_listening = 0; // Set flag to indicate UART listening
        }
    }

    return rylr_message;
}
// Get RYLR Message Counter
int get_rylr_msg_counter()
{
    return rylr_msg_counter;
}
