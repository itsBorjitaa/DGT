#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "logger.h"

#pragma comment(lib, "ws2_32.lib") 

#define PORT 5000
#define MAX_BUFFER 1024

// Compilar con: gcc SocketServer.c logger.c -o admin/server.exe -lws2_32

int main() {
    WSADATA wsaData;
    SOCKET server_fd = INVALID_SOCKET, new_socket = INVALID_SOCKET;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER] = {0};
    const char* response = "Mensaje recibido";

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock.\n");
        return -1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        printf("Fallo en socket: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Fallo en bind: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Fallo en listen: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    printf("Escuchando en el puerto %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket == INVALID_SOCKET) {
            printf("Fallo en accept: %d\n", WSAGetLastError());
            break;
        }

        int valread = recv(new_socket, buffer, MAX_BUFFER - 1, 0);
        if (valread == SOCKET_ERROR) {
            printf("Error en recv: %d\n", WSAGetLastError());
            closesocket(new_socket);
            continue;
        }

        buffer[valread] = '\0'; 
        printf("Mensaje del cliente: %s\n", buffer);

        registrarAccion("cliente", buffer); 

        send(new_socket, response, (int)strlen(response), 0);
        printf("Mensaje enviado al cliente\n");

        closesocket(new_socket);

        if (strcmp(buffer, "adios") == 0) {
            printf("Servidor finalizando por mensaje 'adios'.\n");
            break;
        }

        memset(buffer, 0, MAX_BUFFER);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
