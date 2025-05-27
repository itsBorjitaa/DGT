#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define MAX_BUFFER 1024

// Compilar con: gcc Cliente.c -o cliente.exe -lws2_32

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUFFER] = {0};

    if (argc < 2) {
        printf("Error. El programa no tiene argumentos.\n");
        return -1;
    }

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("Error inicializando Winsock\n");
        return -1;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Error creando socket: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // En lugar de inet_pton usa inet_addr que devuelve un ulong
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (serv_addr.sin_addr.s_addr == INADDR_NONE) {
        printf("Direccion IP invalida\n");
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Fallo la conexion: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    send(sock, argv[1], (int)strlen(argv[1]), 0);
    printf("Mensaje enviado: %s\n", argv[1]);

    int bytesRecibidos = recv(sock, buffer, MAX_BUFFER - 1, 0);
    if (bytesRecibidos > 0) {
        buffer[bytesRecibidos] = '\0';
        printf("Respuesta del servidor: %s\n", buffer);
    } else {
        printf("Error al recibir datos: %d\n", WSAGetLastError());
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
