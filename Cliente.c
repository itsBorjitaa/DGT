#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

//hola
// para ejecutar: gcc -o cliente.exe cliente.c -lws2_32

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define MAX_BUFFER 1024
#define SERVER_IP "127.0.0.1"  // localhost

// Variables globales para sesión
char usuarioActual[50] = "";
char rolActual[20] = "";
int sesionActiva = 0;

// Función para mostrar el menú inicial
void mostrarMenuInicial() {
    printf("\n=== CLIENTE DGT ===\n");
    printf("1. Iniciar sesion\n");
    printf("2. Registrarse\n");
    printf("3. Ping al servidor\n");
    printf("4. Estado del servidor\n");
    printf("0. Salir\n");
    printf("Seleccione una opcion: ");
}

// Función para mostrar el menú de usuario
void mostrarMenuUsuario() {
    printf("\n=== MENU USUARIO ===\n");
    printf("Usuario: %s\n", usuarioActual);
    printf("1. Consultar mis datos personales\n");
    printf("2. Consultar mis vehiculos\n");
    printf("3. Consultar mis multas\n");
    printf("4. Pagar multa\n");
    printf("5. Registrar accidente\n");
    printf("6. Consultar mis accidentes\n");
    printf("7. Exportar mis datos\n");
    printf("8. Agregar vehiculo\n");
    printf("9. Modificar vehiculo\n");
    printf("0. Cerrar sesion\n");
    printf("Seleccione una opcion: ");
}

// Función para mostrar el menú de administrador
void mostrarMenuAdmin() {
    printf("\n=== MENU ADMINISTRADOR ===\n");
    printf("Usuario: %s\n", usuarioActual);
    printf("1. Consultar todos los usuarios\n");
    printf("2. Consultar todos los vehiculos\n");
    printf("3. Consultar todas las multas\n");
    printf("4. Agregar multa\n");
    printf("5. Cambiar estado de multa\n");
    printf("6. Consultar accidente por ID\n");
    printf("0. Cerrar sesion\n");
    printf("Seleccione una opcion: ");
}

// Función para enviar mensaje y recibir respuesta
int enviarMensaje(SOCKET sock, const char* mensaje) {
    // Enviar mensaje
    int send_result = send(sock, mensaje, (int)strlen(mensaje), 0);
    if (send_result == SOCKET_ERROR) {
        printf("Error enviando mensaje: %d\n", WSAGetLastError());
        return -1;
    }
    
    // Recibir respuesta
    char buffer[MAX_BUFFER] = {0};
    int valread = recv(sock, buffer, MAX_BUFFER - 1, 0);
    if (valread == SOCKET_ERROR) {
        printf("Error recibiendo respuesta: %d\n", WSAGetLastError());
        return -1;
    }
    
    if (valread == 0) {
        printf("El servidor cerró la conexión\n");
        return -1;
    }
    
    buffer[valread] = '\0';
    printf("Respuesta del servidor:\n%s\n", buffer);
    return 0;
}

// Función para manejar login
int manejarLogin(SOCKET sock) {
    char usuario[50], contrasena[50];
    char mensaje[MAX_BUFFER];
    
    printf("\n--- Inicio de Sesion ---\n");
    printf("Usuario: ");
    scanf("%49s", usuario);
    printf("Contrasena: ");
    scanf("%49s", contrasena);
    
    // Crear mensaje de login
    snprintf(mensaje, sizeof(mensaje), "LOGIN:%s:%s", usuario, contrasena);
    
    // Enviar mensaje
    int send_result = send(sock, mensaje, (int)strlen(mensaje), 0);
    if (send_result == SOCKET_ERROR) {
        printf("Error enviando mensaje de login: %d\n", WSAGetLastError());
        return 0;
    }
    
    // Recibir respuesta
    char buffer[MAX_BUFFER] = {0};
    int valread = recv(sock, buffer, MAX_BUFFER - 1, 0);
    if (valread <= 0) {
        printf("Error recibiendo respuesta de login\n");
        return 0;
    }
    
    buffer[valread] = '\0';
    
    // Procesar respuesta
    if (strncmp(buffer, "LOGIN_OK:", 9) == 0) {
        // Extraer rol de la respuesta
        strcpy(rolActual, buffer + 9);
        strcpy(usuarioActual, usuario);
        sesionActiva = 1;
        printf("Login exitoso! Rol: %s\n", rolActual);
        return 1;
    } else {
        printf("Login fallido: %s\n", buffer);
        return 0;
    }
}

// Función para manejar registro
int manejarRegistro(SOCKET sock) {
    char usuario[50], contrasena[50], nombre[100], apellidos[100];
    char dni[20], email[100], telefono[20], rol[20];
    char mensaje[MAX_BUFFER];
    int tipoRol;
    
    printf("\n--- Registro de Usuario ---\n");
    printf("Usuario: ");
    scanf("%49s", usuario);
    printf("Contrasena: ");
    scanf("%49s", contrasena);
    
    getchar(); // Limpiar buffer
    
    printf("Nombre: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0; // Quitar salto de línea
    
    printf("Apellidos: ");
    fgets(apellidos, sizeof(apellidos), stdin);
    apellidos[strcspn(apellidos, "\n")] = 0;
    
    printf("DNI: ");
    scanf("%19s", dni);
    printf("Email: ");
    scanf("%99s", email);
    printf("Telefono: ");
    scanf("%19s", telefono);
    
    do {
        printf("Seleccione Rol:\n");
        printf("0. Administrador\n");
        printf("1. Usuario\n");
        printf("Opcion: ");
        scanf("%d", &tipoRol);
        
        if (tipoRol == 0) {
            strcpy(rol, "admin");
        } else if (tipoRol == 1) {
            strcpy(rol, "usuario");
        } else {
            printf("Opcion no valida. Intente de nuevo.\n");
        }
    } while (tipoRol != 0 && tipoRol != 1);
    
    // Crear mensaje de registro
    snprintf(mensaje, sizeof(mensaje), "REGISTER:%s:%s:%s:%s:%s:%s:%s:%s", 
             usuario, contrasena, rol, nombre, apellidos, dni, email, telefono);
    
    // Enviar mensaje
    int send_result = send(sock, mensaje, (int)strlen(mensaje), 0);
    if (send_result == SOCKET_ERROR) {
        printf("Error enviando mensaje de registro: %d\n", WSAGetLastError());
        return 0;
    }
    
    // Recibir respuesta
    char buffer[MAX_BUFFER] = {0};
    int valread = recv(sock, buffer, MAX_BUFFER - 1, 0);
    if (valread <= 0) {
        printf("Error recibiendo respuesta de registro\n");
        return 0;
    }
    
    buffer[valread] = '\0';
    printf("Resultado del registro: %s\n", buffer);
    
    return (strncmp(buffer, "REGISTER_OK", 11) == 0);
}

// Función para manejar comandos de usuario
void manejarComandosUsuario(SOCKET sock) {
    int opcion;
    char mensaje[MAX_BUFFER];
    char input[200];
    
    do {
        mostrarMenuUsuario();
        scanf("%d", &opcion);
        getchar(); // Limpiar buffer
        
        switch (opcion) {
            case 1:
                snprintf(mensaje, sizeof(mensaje), "USER_DATA:%s", usuarioActual);
                enviarMensaje(sock, mensaje);
                break;
                
            case 2:
                snprintf(mensaje, sizeof(mensaje), "USER_VEHICLES:%s", usuarioActual);
                enviarMensaje(sock, mensaje);
                break;
                
            case 3:
                snprintf(mensaje, sizeof(mensaje), "USER_FINES:%s", usuarioActual);
                enviarMensaje(sock, mensaje);
                break;
                
            case 4:
                printf("Ingrese ID de la multa a pagar: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                snprintf(mensaje, sizeof(mensaje), "PAY_FINE:%s:%s", usuarioActual, input);
                enviarMensaje(sock, mensaje);
                break;
                
            case 5:
                printf("Ingrese descripcion del accidente: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                snprintf(mensaje, sizeof(mensaje), "ADD_ACCIDENT:%s:%s", usuarioActual, input);
                enviarMensaje(sock, mensaje);
                break;
                
            case 6:
                snprintf(mensaje, sizeof(mensaje), "USER_ACCIDENTS:%s", usuarioActual);
                enviarMensaje(sock, mensaje);
                break;
                
            case 7:
                snprintf(mensaje, sizeof(mensaje), "EXPORT_DATA:%s", usuarioActual);
                enviarMensaje(sock, mensaje);
                break;
                
            case 8:
                printf("Ingrese datos del vehiculo (matricula,marca,modelo,año,color,tipo): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                snprintf(mensaje, sizeof(mensaje), "ADD_VEHICLE:%s:%s", usuarioActual, input);
                enviarMensaje(sock, mensaje);
                break;
                
            case 9:
                printf("Ingrese matricula del vehiculo a modificar: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                snprintf(mensaje, sizeof(mensaje), "MODIFY_VEHICLE:%s:%s", usuarioActual, input);
                enviarMensaje(sock, mensaje);
                break;
                
            case 0:
                printf("Cerrando sesion...\n");
                sesionActiva = 0;
                strcpy(usuarioActual, "");
                strcpy(rolActual, "");
                break;
                
            default:
                printf("Opcion no valida\n");
                break;
        }
    } while (opcion != 0);
}

// Función para manejar comandos de administrador
void manejarComandosAdmin(SOCKET sock) {
    int opcion;
    char mensaje[MAX_BUFFER];
    char input[500];
    
    do {
        mostrarMenuAdmin();
        scanf("%d", &opcion);
        getchar(); // Limpiar buffer
        
        switch (opcion) {
            case 1:
                enviarMensaje(sock, "ADMIN_ALL_USERS");
                break;
                
            case 2:
                enviarMensaje(sock, "ADMIN_ALL_VEHICLES");
                break;
                
            case 3:
                enviarMensaje(sock, "ADMIN_ALL_FINES");
                break;
                
            case 4:
                printf("Ingrese datos de la multa (dni,concepto,fecha_delito,importe,fecha_limite): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                snprintf(mensaje, sizeof(mensaje), "ADD_FINE:%s", input);
                enviarMensaje(sock, mensaje);
                break;
                
            case 5:
                printf("Ingrese ID de multa y nuevo estado (id,estado): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                snprintf(mensaje, sizeof(mensaje), "CHANGE_FINE_STATUS:%s", input);
                enviarMensaje(sock, mensaje);
                break;
                
            case 6:
                printf("Ingrese ID del accidente: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                snprintf(mensaje, sizeof(mensaje), "GET_ACCIDENT:%s", input);
                enviarMensaje(sock, mensaje);
                break;
                
            case 0:
                printf("Cerrando sesion...\n");
                sesionActiva = 0;
                strcpy(usuarioActual, "");
                strcpy(rolActual, "");
                break;
                
            default:
                printf("Opcion no valida\n");
                break;
        }
    } while (opcion != 0);
}

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    
    printf("=== CLIENTE DGT ===\n");
    printf("Conectando al servidor %s:%d\n", SERVER_IP, PORT);
    
    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock\n");
        return -1;
    }
    
    // Crear socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Error creando socket: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    
    // Configurar dirección del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    if (serv_addr.sin_addr.s_addr == INADDR_NONE) {
        printf("Dirección IP inválida\n");
        closesocket(sock);
        WSACleanup();
        return -1;
    }
    
    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Fallo al conectar: %d\n", WSAGetLastError());
        printf("Asegurate de que el servidor esté ejecutándose\n");
        closesocket(sock);
        WSACleanup();
        return -1;
    }
    
    printf("Conectado al servidor exitosamente!\n");
    
    // Loop principal del cliente
    int opcion;
    
    while (1) {
        if (!sesionActiva) {
            // Menú inicial - sin sesión activa
            mostrarMenuInicial();
            scanf("%d", &opcion);
            
            switch (opcion) {
                case 1:
                    if (manejarLogin(sock)) {
                        printf("Sesion iniciada correctamente\n");
                    }
                    break;
                    
                case 2:
                    manejarRegistro(sock);
                    break;
                    
                case 3:
                    if (enviarMensaje(sock, "ping") != 0) {
                        goto cleanup;
                    }
                    break;
                    
                case 4:
                    if (enviarMensaje(sock, "estado") != 0) {
                        goto cleanup;
                    }
                    break;
                    
                case 0:
                    printf("Saliendo...\n");
                    enviarMensaje(sock, "adios");
                    goto cleanup;
                    
                default:
                    printf("Opcion no valida\n");
                    break;
            }
        } else {
            // Menú según el rol del usuario
            if (strcmp(rolActual, "usuario") == 0) {
                manejarComandosUsuario(sock);
            } else if (strcmp(rolActual, "admin") == 0) {
                manejarComandosAdmin(sock);
            }
        }
    }
    
cleanup:
    closesocket(sock);
    WSACleanup();
    printf("Cliente desconectado\n");
    return 0;
}