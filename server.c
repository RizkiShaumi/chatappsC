#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include <pthread.h>  // For threads

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

#define PORT 8080
char key[16];
SOCKET new_socket;
int exit_flag = 0;  // Global flag for closing the connection

void* receive_handler(void* arg) {
    char buffer[1024];
    while (!exit_flag) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        int bytesReceived = recv(new_socket, buffer, 1024, 0);
        if (bytesReceived > 0) {
            printf("\nclient: %s\n", buffer);

            // If the client sends 'exit', break out and terminate
            if (strcmp(buffer, "exit") == 0) {
                printf("Client disconnected.\n");
                exit_flag = 1;  // Set flag to close connection
                break;
            }
        } else if (bytesReceived == 0) {
            printf("Client closed the connection.\n");
            exit_flag = 1;
            break;
        } else {
            printf("recv failed. Error Code: %d\n", WSAGetLastError());
            exit_flag = 1;
            break;
        }
    }
    return NULL;
}

void* send_handler(void* arg) {
    char pesan[100];
    while (!exit_flag) {
        printf("Masukkan Pesan: ");
        scanf("%s", pesan);

        send(new_socket, pesan, strlen(pesan), 0);

        if (strcmp(pesan, "exit") == 0) {
            printf("Server is shutting down.\n");
            exit_flag = 1;
            break;
        }
    }
    return NULL;
}

int main() {
    WSADATA wsa;
    SOCKET server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t receive_thread, send_thread;  // Declare threads

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Set up the sockaddr_in structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Waiting for a connection...\n");
    

    // Accept a connection from a client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Connection accepted.\n");

    // Create a thread for receiving messages from the client
    pthread_create(&receive_thread, NULL, receive_handler, NULL);

    // Create a thread for sending messages to the client
    pthread_create(&send_thread, NULL, send_handler, NULL);

    // Wait for both threads to finish
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    // Close the socket and clean up
    closesocket(new_socket);
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
