#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 4096
#define PORT 6006

bool msgs_too_long(int argc, char **argv) {
    int cumulative_len = 0;
    for (int i = 1; i < argc; i++) {
        cumulative_len += strlen(argv[i]);
        if (i < argc - 1) {
            cumulative_len++; // +1 for space
        }
        if (cumulative_len >= BUFLEN) {
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv) {
    int client_socket = -1, retval = EXIT_SUCCESS, bytes_recvd, ip_conversion;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    char buf[BUFLEN];
    char *addr_str = "127.0.0.1";

    if (argc < 2) {
        fprintf(stderr, "Error. Must provide at least 1 string to sort.\n");
        return EXIT_FAILURE;
    }

    if (msgs_too_long(argc, argv)) {
        fprintf(stderr,
                "Error: At most %d characters can be sent, including '\\0'!\n",
                BUFLEN);
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    // Create a reliable, stream socket using TCP.
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    // Construct the server address structure.
    memset(&server_addr, 0, addrlen);   // Zero out structure
    server_addr.sin_family = AF_INET;   // Internet address family
    server_addr.sin_port = htons(PORT); // Server port, 16 bits

    // Convert character string into a network address.
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

    // Establish the connection to the sorting server.
    if (connect(client_socket, (struct sockaddr *)&server_addr,
            sizeof(struct sockaddr_in)) < 0) {
        fprintf(stderr, "Error: Failed to connect to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    memset(buf, 0, BUFLEN);
    for (int i = 1; i < argc; i++) {
        if (strlen(buf) + strlen(argv[i]) + 1 >= BUFLEN) {
            break;
        }
        strncat(buf, argv[i], BUFLEN-1);
        if (i != argc - 1) {
            strncat(buf, " ", BUFLEN-1);
        }
    }

    printf("Sending string to server: %s\n", buf);
    if (send(client_socket, buf, strlen(buf), 0) < 0) {
        fprintf(stderr, "Error: Failed to send message to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    if ((bytes_recvd = recv(client_socket, buf, BUFLEN-1, 0)) < 0) {
        fprintf(stderr, "Error: Failed to receive message from server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    buf[bytes_recvd] = '\0';
    printf("Sorted string received from server: %s\n", buf);

EXIT:
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);
    }
    return retval;
}