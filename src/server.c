#include "server.h"

int main(unsigned int argc, char *argv[]) {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT); // Port number
    server_address.sin_addr.s_addr = INADDR_ANY; // Bind to any address

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        return 1;
    }

    printf("Server is listening on port %d ...\n", PORT);

    // Accept incoming connections (this is a blocking call)
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
    if (client_socket < 0) {
        perror("Accept failed");
        return 1;
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    // Close the sockets
    close(client_socket);
    close(server_socket);

    return 0;
}