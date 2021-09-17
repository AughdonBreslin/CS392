/*******************************************************************************
 * Name        : chatclient.c
 * Author      : Matthew Oyales & Audie Breslin
 * Date        : 7 July 2021
 * Description : Chat client using sockets API in C.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "util.h"
#define BUFSIZE 128 //Not including '\0'.

int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];

int handle_stdin() {
    return 0;
}

int handle_client_socket() {
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int bytes_recvd;
    int client_socket, ip_conversion, retval = EXIT_SUCCESS;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    char buf[BUFLEN];
    char *addr_str = argv[1];

    // Create a reliable, stream socket using TCP.
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    memset(&server_addr, 0, addrlen); // Zero out structure

    ip_conversion = inet_pton(AF_INET, addr_str, &server_addr.sin_addr);
    if (ip_conversion == 0) {
        fprintf(stderr, "Error: Invalid IP address '%s'.\n", addr_str);
        retval = EXIT_FAILURE;
        goto EXIT;
    } else if (ip_conversion < 0) {
        fprintf(stderr, "Error: Failed to convert IP address. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    int port;   
    if (!parse_int(argv[2], &port, "port number")) {
        return EXIT_FAILURE;
    } 
    if (port < 1024 || port > 65535) {
        fprintf(stderr, "Error: port must be in range [1024, 65535].\n");
        return EXIT_FAILURE;
    } 

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);


    // After parsing the command-line arguments, program should prompt user for username.
    // If name is 0 characters or too long, reprompt.
    while(true) {
        printf("Enter your username: ");
        fflush(stdout);
        int bytes_read = read(STDIN_FILENO, username, BUFSIZE);
        if (bytes_read < 0) {
            fprintf(stderr, "Error: Failed to read from stdin. %s.\n",
                    strerror(errno));
            retval = EXIT_FAILURE;
            goto EXIT;
        }

        // No input, reprompt.
        if (username[0] == '\n') {
            continue;
        }

        /** TODO: */
        // This condition is incorrect, not sure how to fix.
        if (bytes_read > MAX_NAME_LEN) {
            printf("Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
            continue;
        } else {
            username[bytes_read - 1] = '\0'; // Replace \n in input to \0.
            break;                         // Continue with the program.
        }
    }
    
    /**
     * After obtaining a username, print
     * "Hello, %s. Let's try to connect to the server.\n"
     */
    printf("Hello, %s. Let's try to connect to the server.\n", username);

    /**
     * Program creates a TCP socket.
     * If it fails, error out with message:
     * "Error: Failed to create socket. %s.\n"
     * 
     * All other errors following this one will have a %s corresponding
     * to the strerror(errno).
     */
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    /**
     * Program attempts to connect to the server.
     */
    if (connect(client_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        fprintf(stderr, "Error: Failed to connect to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    /**
     * Program attempts to receive message from server.
     * 
     * 1. If client fails to welcome receive message from server.
     * 2. If bytes recieved is 0, then server closed connection on client.
     */
    if ((bytes_recvd = recv(client_socket, buf, BUFLEN - 1, 0)) < 0) {
        fprintf(stderr, "Error: Failed to receive message from server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    } else if (bytes_recvd == 0) {
        fprintf(stderr, "All connections are busy. Try again later.\n");
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    buf[bytes_recvd] = '\0'; // Null-terminating server's welcome message
    /**
     * Next, print a new line, the welcome message received 
     * from the server, and two more new lines.
     * 
     * Finally, send the username to the server. If it fails, 
     * error out with the message:
     * "Error: Failed to send username to server. %s.\n"
     */
    printf("\n%s\n\n", buf); // Printing the welcome message

    if (send(client_socket, username, strlen(username), 0) < 0) {
        fprintf(stderr, "Error: Failed to send username to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    /**
     * TODO:
     * Using fd_set and select (man 2 select), your program should now loop forever, 
     * prompting the user for input and determining if there is activity on one of 
     * the two file descriptors. 
     * 
     * Specifically, both the socket and STDIN_FILENO should be added to the fd_set. If there is
     * activity on
     * 
     * 1. STDIN_FILENO
     * 2. Client socket
     */
    while (true) {
        printf("[%s]: ", username); // User's prompt for msg input.
        fflush(stdout);
        ssize_t bytes_read = read(STDIN_FILENO, outbuf, MAX_MSG_LEN);
        int msg_status = get_string(outbuf, bytes_read);
        if (msg_status == NO_INPUT) {
            continue;
        } else if (msg_status == TOO_LONG) {
            printf("Sorry, limit your message to %d characters.\n", MAX_MSG_LEN);
            continue;
        } else {
            if (send(client_socket, outbuf, strlen(username), 0) < 0) {
                fprintf(stderr, "Error: Failed to send message to server. %s.\n",
                strerror(errno));
                retval = EXIT_FAILURE;
                goto EXIT;
            } else if (strcmp(outbuf, "bye") == 0) {
                printf("Goodbye.\n");
                break;
            }
        }
    }

EXIT:
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);
    }
    return retval;
}