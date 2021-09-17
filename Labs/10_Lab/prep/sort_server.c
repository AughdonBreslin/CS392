#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFLEN 4096
#define PORT 6006

int sort_string(char *input);

int main() {
    int server_socket = -1, client_socket = -1, retval = EXIT_SUCCESS,
        bytes_recvd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    char buf[BUFLEN];

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n",
        strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    memset(&server_addr, 0, addrlen);          // Zero out structure
    server_addr.sin_family = AF_INET;          // Internet address family
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Internet address, 32 bits
    server_addr.sin_port = htons(PORT);        // Server port, 16 bits

    // Bind to the local address.
    if (bind(server_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        fprintf(stderr, "Error: Failed to bind socket to port %d. %s.\n", PORT,
        strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    // Mark the socket so it will listen for incoming connections.
    if (listen(server_socket, SOMAXCONN) < 0) {
        fprintf(stderr,
        "Error: Failed to listen for incoming connections. %s.\n",
        strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    printf("Sorting server is up and running on port %d...\n",
            ntohs(server_addr.sin_port));
    
    while (true) {
        // Wait for the client to connect.
        if ((client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr,
        &addrlen)) < 0) {
            fprintf(stderr,
            "Error: Failed to accept incoming connection. %s.\n",
            strerror(errno));
            retval = EXIT_FAILURE;
            goto EXIT;
        }

        if ((bytes_recvd = recv(client_socket, buf, BUFLEN - 1, 0)) < 0) {
            fprintf(stderr,
            "Error: Failed to receive message from client %s. %s.\n",
            inet_ntoa(client_addr.sin_addr), strerror(errno));
            retval = EXIT_FAILURE;
            goto EXIT;
        }
        buf[bytes_recvd] = '\0';
        printf("Received following from client: %s\n", buf);

        if (sort_string(buf) == EXIT_FAILURE) {
            fprintf(stderr, "Could not execute program 'sort'. Shutting down.\n");
            retval = EXIT_FAILURE;
            goto EXIT;
        }

        printf("Sending following sorted string to client: %s\n", buf);

        if (send(client_socket, buf, BUFLEN, 0) < 0) {
            fprintf(stderr, "Error: Failed to send message to client %s. %s.\n",
            inet_ntoa(client_addr.sin_addr), strerror(errno));
            retval = EXIT_FAILURE;
            goto EXIT;
        }
        memset(buf, 0, BUFLEN);
        close(client_socket);
    }
EXIT:
    // F_GETFD - Return the file descriptor flags.
    if (fcntl(server_socket, F_GETFD) >= 0) {
        close(server_socket);
    }
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);
    }
    return retval;
}

/**
* Sorts words in string, placing them in `input`. Assumed `input` is not a
* string literal, and that strlen(input) < 4096. Returns EXIT_FAILURE and no
* changes made to input if exec fails. Does no error checking beyond exec.
*/
int sort_string(char *input) {
    int parent2sort[2];
    int sort2parent[2];

    pipe(parent2sort);
    pipe(sort2parent);

    if (fork() == 0) {
        // sort
        close(parent2sort[1]);
        close(sort2parent[0]);

        dup2(parent2sort[0], STDIN_FILENO);
        dup2(sort2parent[1], STDOUT_FILENO);

        execlp("sort", "sort", NULL);
        fprintf(stderr, "sort failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    char buf[BUFLEN];
    int bytes_read;

    close(parent2sort[0]);
    close(sort2parent[1]);

    for (int i = 0; i < (int)strlen(input); i++) {
        if (input[i] == ' ') {
            write(parent2sort[1], "\n", 1);
        } else {
            write(parent2sort[1], &input[i], 1);
        }
    }

    close(parent2sort[1]);

    bytes_read = read(sort2parent[0], buf, 4095);

    buf[bytes_read] = '\0';

    close(sort2parent[0]);

    wait(NULL);
    wait(NULL);

    strcpy(input, buf);

    for (int i = 0; i < (int)strlen(input); i++) {
        if (input[i] == '\n') {
            input[i] = ' ';
        }
    }

    return EXIT_SUCCESS;
}