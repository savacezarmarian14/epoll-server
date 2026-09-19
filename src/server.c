#include "server.h"
#include <errno.h>
#include <string.h>
int echo_loop(int client_socket) {
    char buffer[1024];
    ssize_t bytes_read;

    while (1) {
        bytes_read = read(client_socket, buffer, sizeof(buffer));
        if (bytes_read < 0 && errno == EINTR) {
            continue; // Interrupted by signal, retry
        } else if (bytes_read < 0) {
            perror("Read failed");
            return -1;
        } else if (bytes_read == 0) {
            printf("Client disconnected\n");
            return 0;
        }

        ssize_t bytes_to_write = bytes_read;
        while (bytes_to_write > 0) {
            ssize_t bytes_written = write(client_socket, buffer + (bytes_read - bytes_to_write), bytes_to_write);
            if (bytes_written < 0 && errno == EINTR) {
                continue; // Interrupted by signal, retry
            } else if (bytes_written < 0) {
                perror("Write failed");
                return -1;
            }
            bytes_to_write -= bytes_written;
        }
    }
}

int main(int argc, char *argv[]) {
    int server_socket;
    int client_socket;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_len;

    (void)argc;
    (void)argv;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("setsockopt failed");
        close(server_socket);
        return 1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }

    printf("Server is listening on port %d ...\n", PORT);

    client_len = sizeof(client_address);
    client_socket = -1;
    while (client_socket < 0) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket < 0 && errno != EINTR) {
            perror("Accept failed");
            close(server_socket);
            return 1;
        }
    }

    printf("Connection accepted from %s:%d\n",
           inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    while(1) {
        int cdisconnect = 0;
        echo_loop(client_socket, &cdisconnect);
        if (cdisconnect) {
            break;
        }
    }

    close(client_socket);
    close(server_socket);

    return 0;
}