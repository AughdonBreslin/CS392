#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define IP_ADDR "127.0.0.1"
#define BUFLEN  128

int create_socket(struct timeval *tv) {
    int client_socket;

    // TODO: Create a client socket using TCP. If it fails, print the error
    // message: "Error: Failed to create socket. %s.\n", where %s is strerror of
    // errno and return -1.
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n",
                strerror(errno));
        return -1;
    }

    // TODO: Set the receive timeout option for the client socket. Use the
    // function's argument, struct timeval *tv, in the function call to
    // setsockopt. If setsockopt fails, print the error message:
    // "Error: Cannot set socket options. %s.\n", where %s is strerror of errno
    // and return -1.
    // Hint: Look up SO_RCVTIMEO.
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)tv, sizeof(struct timeval)) < 0) {
        fprintf(stderr, "Error: Cannot set socket options. %s.\n", strerror(errno));
        return -1;
    } 

    return client_socket;
}

int main() {
    int client_socket = -1, bytes_recvd, retval = EXIT_SUCCESS;
    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    char buf[BUFLEN];

    memset(&serv_addr, 0, addrlen);
    int ip_conversion = inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr);
    if (ip_conversion == 0) {
        fprintf(stderr, "Error: Invalid IP address '%s'.\n", IP_ADDR);
        return -1;
    } else if (ip_conversion < 0) {
        fprintf(stderr, "Error: Failed to convert IP address. %s.\n",
                strerror(errno));
        return -1;
    }
    serv_addr.sin_family = AF_INET;

    struct timeval tv;
    // TODO: Set the tv to be 0 seconds, 250 microseconds.
    tv.tv_sec  = 0;     // 0   seconds
    tv.tv_usec = 250;   // 250 microseconds

    // TODO:
    // Loop over ports 1024 up to and including 65535.
    //   Modify the sin_port field of serv_addr to the port you are trying.
    //   Create a client socket by calling create_socket() above. If it fails,
    //   set retval to EXIT_FAILURE and transfer execution to EXIT below.
    //   Attempt to connect to the server.
    //   If it does connect,
    //     Attempt to receive up to BUFLEN - 1 characters.
    //     If data is received,
    //       Null-terminate it.
    //       Print "Found server on port %d.\n"
    //             "Received message from server: %s\n"
    //       Transfer execution to EXIT below.
    //   Close the socket.
    for (int i = 1024; i <= 65535; i++) {
        serv_addr.sin_port = htons(i);
        if ((client_socket = create_socket(&tv)) < 0) {
            retval = EXIT_FAILURE;
            goto EXIT;
        }

        if (connect(client_socket, (struct sockaddr *)&serv_addr, addrlen) >= 0) {
            if ((bytes_recvd = recv(client_socket, buf, BUFLEN - 1, 0)) >= 0) {
                buf[bytes_recvd] = '\0';
                printf("Found server on port %d.\n"
                       "Received message from server: %s\n", i, buf);
                goto EXIT;
            }
        }
        close(client_socket);
    }

    printf("No server was found.\n");

EXIT:
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);
    }
    return retval;
}
