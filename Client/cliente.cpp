#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "usuario.h"
#include "vehiculo.h"
#include "multa.h"
#include "accidente.h"

// para ejecutar: g++ -o cliente.exe cliente.cpp usuario.cpp vehiculo.cpp multa.cpp accidente.cpp -lws2_32

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define MAX_BUFFER 1024
#define SERVER_IP "127.0.0.1"  // localhost

// Variables globales para sesión
Usuario* usuarioActual = new Usuario();
char* rolActual = new char[10]{};
int sesionActiva = 0;
char nombreArchivo[100] = "";  

// Función para mostrar el menú inicial
void mostrarMenuInicial() {
    std::cout << "\n=== CLIENTE DGT ===\n";
    std::cout << "1. Iniciar sesion\n";
    std::cout << "2. Registrarse\n";
    std::cout << "3. Ping al servidor\n";
    std::cout << "4. Estado del servidor\n";
    std::cout << "0. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

// Función para mostrar el menú de usuario
void mostrarMenuUsuario() {
    std::cout << "\n=== MENU USUARIO ===\n";
    std::cout << "Usuario: " << usuarioActual->getUsuario() << std::endl;
    std::cout << "1. Consultar mis datos personales\n";
    std::cout << "2. Consultar mis vehiculos\n";
    std::cout << "3. Consultar mis multas\n";
    std::cout << "4. Pagar multa\n";
    std::cout << "5. Registrar accidente\n";
    std::cout << "6. Consultar mis accidentes\n";
    std::cout << "7. Exportar mis datos\n";
    std::cout << "8. Agregar vehiculo\n";
    std::cout << "9. Modificar vehiculo\n";
    std::cout << "0. Cerrar sesion\n";
    std::cout << "Seleccione una opcion: ";
}

// Función para mostrar el menú de administrador
void mostrarMenuAdmin() {
    std::cout << "\n=== MENU ADMINISTRADOR ===\n";
    std::cout << "Usuario: " << usuarioActual->getUsuario() << std::endl;
    std::cout << "1. Consultar todos los usuarios\n";
    std::cout << "2. Consultar todos los vehiculos\n";
    std::cout << "3. Consultar todas las multas\n";
    std::cout << "4. Agregar multa\n";
    std::cout << "5. Cambiar estado de multa\n";
    std::cout << "6. Consultar accidente por ID\n";
    std::cout << "0. Cerrar sesion\n";
    std::cout << "Seleccione una opcion: ";
}

// Función para enviar mensaje y recibir respuesta
int enviarMensaje(SOCKET sock, const char* mensaje) {
    // Enviar mensaje
    int send_result = send(sock, mensaje, (int)strlen(mensaje), 0);
    if (send_result == SOCKET_ERROR) {
        std::cout << "Error enviando mensaje: " << WSAGetLastError() << std::endl;
        return -1;
    }
    
    // Recibir respuesta
    char buffer[MAX_BUFFER] = {0};
    int valread = recv(sock, buffer, MAX_BUFFER - 1, 0);
    if (valread == SOCKET_ERROR) {
        std::cout << "Error recibiendo respuesta: " << WSAGetLastError() << std::endl;
        return -1;
    }
    
    if (valread == 0) {
        std::cout << "El servidor cerró la conexión\n";
        return -1;
    }
    
    buffer[valread] = '\0';
    std::cout << "Respuesta del servidor:\n" << buffer << std::endl;
    return 0;
}

int manejarLogin(SOCKET sock) {
    char usuario[50], contrasena[50];
    char mensaje[MAX_BUFFER];
    
    std::cout << "\n--- Inicio de Sesion ---\n";
    std::cout << "Usuario: ";
    std::cin >> usuario;
    std::cout << "Contrasena: ";
    std::cin >> contrasena;
    
    // Crear mensaje de login
    snprintf(mensaje, sizeof(mensaje), "LOGIN:%s:%s", usuario, contrasena);
    
    // Enviar mensaje
    int send_result = send(sock, mensaje, (int)strlen(mensaje), 0);
    if (send_result == SOCKET_ERROR) {
        std::cout << "Error enviando mensaje de login: " << WSAGetLastError() << std::endl;
        return 0;
    }
    
    char buffer[MAX_BUFFER] = {0};
    int valread = recv(sock, buffer, MAX_BUFFER - 1, 0);
    if (valread <= 0) {
        std::cout << "Error recibiendo respuesta de login\n";
        return 0;
    }
    
    buffer[valread] = '\0';
    
    if (strncmp(buffer, "LOGIN_OK:", 9) == 0) {
        strcpy(rolActual, buffer + 9);
        usuarioActual->setUsuario(usuario);
        sesionActiva = 1;
        std::cout << "Login exitoso! Rol: " << rolActual << std::endl;
        return 1;
    } else {
        std::cout << "Login fallido: " << buffer << std::endl;
        return 0;
    }
}

int manejarRegistro(SOCKET sock) {
    char usuario[50], contrasena[50], nombre[100], apellidos[100];
    char dni[20], email[100], telefono[20], rol[20];
    char mensaje[MAX_BUFFER];
    int tipoRol;

    std::cout << "\n--- Registro de Usuario ---\n";
    std::cout << "Usuario: ";
    std::cin >> usuario;
    std::cout << "Contrasena: ";
    std::cin >> contrasena;

    std::cout << "Nombre: ";
    std::cin.ignore();
    std::cin.getline(nombre, sizeof(nombre));
    
    std::cout << "Apellidos: ";
    std::cin.getline(apellidos, sizeof(apellidos));
    
    std::cout << "DNI: ";
    std::cin >> dni;
    std::cout << "Email: ";
    std::cin >> email;
    std::cout << "Telefono: ";
    std::cin >> telefono;

    do {
        std::cout << "Seleccione Rol:\n";
        std::cout << "0. Administrador\n";
        std::cout << "1. Usuario\n";
        std::cout << "Opcion: ";
        std::cin >> tipoRol;

        if (tipoRol == 0) {
            strcpy(rol, "admin");
        } else if (tipoRol == 1) {
            strcpy(rol, "usuario");
        } else {
            std::cout << "Opcion no valida. Intente de nuevo.\n";
        }
    } while (tipoRol != 0 && tipoRol != 1);

    snprintf(mensaje, sizeof(mensaje), "REGISTER:%s:%s:%s:%s:%s:%s:%s:%s", 
             usuario, contrasena, rol, nombre, apellidos, dni, email, telefono);

    int send_result = send(sock, mensaje, (int)strlen(mensaje), 0);
    if (send_result == SOCKET_ERROR) {
        std::cout << "Error enviando mensaje de registro: " << WSAGetLastError() << std::endl;
        return 0;
    }

    char buffer[MAX_BUFFER] = {0};
    int valread = recv(sock, buffer, MAX_BUFFER - 1, 0);
    if (valread <= 0) {
        std::cout << "Error recibiendo respuesta de registro\n";
        return 0;
    }
    
    buffer[valread] = '\0';
    std::cout << "Resultado del registro: " << buffer << std::endl;

    return (strncmp(buffer, "REGISTER_OK", 11) == 0);
}

void mostrarEstadosMulta() {
    std::cout << "\nEstados disponibles:\n";
    std::cout << "1. PENDIENTE\n";
    std::cout << "2. PAGADA\n";
    std::cout << "3. ANULADA\n";
    std::cout << "4. VENCIDA\n";
    std::cout << "Seleccione el nuevo estado: ";
}

const char* obtenerTextoEstado(int opcionEstado) {
    switch(opcionEstado) {
        case 1: return "PENDIENTE";
        case 2: return "PAGADA";
        case 3: return "ANULADA";
        case 4: return "VENCIDA";
        default: return "PENDIENTE";
    }
}

void manejarComandosUsuario(SOCKET sock) {
    int opcion;
    char mensaje[MAX_BUFFER];
    char input[200];

    do {
        mostrarMenuUsuario();
        std::cin >> opcion;
        std::cin.ignore(); // Limpiar buffer

        switch (opcion) {
            case 1:
                snprintf(mensaje, sizeof(mensaje), "USER_DATA:%s", usuarioActual->getUsuario());
                enviarMensaje(sock, mensaje);
                break;

            case 2:
                snprintf(mensaje, sizeof(mensaje), "USER_VEHICLES:%s", usuarioActual->getUsuario());
                enviarMensaje(sock, mensaje);
                break;

            case 3:
                snprintf(mensaje, sizeof(mensaje), "USER_FINES:%s", usuarioActual->getUsuario());
                enviarMensaje(sock, mensaje);
                break;

            case 4:
                std::cout << "Ingrese ID de la multa a pagar: ";
                std::cin.getline(input, sizeof(input));
                snprintf(mensaje, sizeof(mensaje), "PAY_FINE:%s:%s", usuarioActual->getUsuario(), input);
                enviarMensaje(sock, mensaje);
                break;

            case 5:
                std::cout << "Ingrese descripcion del accidente: ";
                std::cin.getline(input, sizeof(input));
                snprintf(mensaje, sizeof(mensaje), "ADD_ACCIDENT:%s:%s", usuarioActual->getUsuario(), input);
                enviarMensaje(sock, mensaje);
                break;

            case 6:
                snprintf(mensaje, sizeof(mensaje), "USER_ACCIDENTS:%s", usuarioActual->getUsuario());
                enviarMensaje(sock, mensaje);
                break;

            case 7:
                std::cout << "Introduzca el nombre con el que desea guardar el archivo (sin extension): ";
                std::cin.getline(nombreArchivo, sizeof(nombreArchivo));
                snprintf(mensaje, sizeof(mensaje), "EXPORT_DATA:%s:%s", usuarioActual->getUsuario(), nombreArchivo);
                enviarMensaje(sock, mensaje);
                break;

            case 8:
                std::cout << "Ingrese datos del vehiculo (matricula,marca,modelo,año,color,tipo): ";
                std::cin.getline(input, sizeof(input));
                snprintf(mensaje, sizeof(mensaje), "ADD_VEHICLE:%s:%s", usuarioActual->getUsuario(), input);
                enviarMensaje(sock, mensaje);
                break;

            case 9: {
                std::string matricula;
                std::string nuevosDatos;

                std::cout << "Ingrese matricula del vehiculo a modificar: ";
                std::getline(std::cin, matricula);

                char mensaje[MAX_BUFFER];
                snprintf(mensaje, sizeof(mensaje), "MODIFY_VEHICLE:%s:%s", usuarioActual, matricula.c_str());
                if (enviarMensaje(sock, mensaje) == -1) {
                std::cerr << "Error comunicándose con el servidor.\n";
                break;
            }

                std::cout << "Introduce los nuevos datos separados por coma (marca,modelo,anio,color,tipo): ";
                std::getline(std::cin, nuevosDatos);

                snprintf(mensaje, sizeof(mensaje), "MODIFY_VEHICLE_DATA:%s:%s:%s", usuarioActual, matricula.c_str(), nuevosDatos.c_str());
                if (enviarMensaje(sock, mensaje) == -1) {
                    std::cerr << "Error comunicándose con el servidor.\n";
                    break;
                }

                break;
        }

            case 0:
                std::cout << "Cerrando sesion...\n";
                sesionActiva = 0;
                usuarioActual->setUsuario("");
                strcpy(rolActual, "");
                break;

            default:
                std::cout << "Opcion no valida\n";
                break;
        }
    } while (opcion != 0);
}

void manejarComandosAdmin(SOCKET sock) {
    int opcion;
    char mensaje[MAX_BUFFER];
    char input[500];
    char idMulta[50];
    int opcionEstado;

    do {
        mostrarMenuAdmin();
        std::cin >> opcion;
        std::cin.ignore(); // Limpiar buffer

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
                std::cout << "Ingrese datos de la multa (dni,concepto,fecha_delito,importe,fecha_limite): ";
                std::cin.getline(input, sizeof(input));
                snprintf(mensaje, sizeof(mensaje), "ADD_FINE:%s", input);
                enviarMensaje(sock, mensaje);
                break;

            case 5: {
                std::string input;
                int id_multa;

                std::cout << "Ingrese ID de la multa a actualizar: ";
                std::getline(std::cin, input);
                id_multa = std::stoi(input);

                char mensaje[MAX_BUFFER];
                snprintf(mensaje, sizeof(mensaje), "CHECK_FINE_EXISTS:%d", id_multa);

                char respuesta[MAX_BUFFER] = {0};

                int send_result = send(sock, mensaje, (int)strlen(mensaje), 0);
                if (send_result == SOCKET_ERROR) {
                    std::cerr << "Error enviando mensaje: " << WSAGetLastError() << "\n";
                    break;
                }

                int valread = recv(sock, respuesta, MAX_BUFFER - 1, 0);
                if (valread <= 0) {
                    std::cerr << "Error recibiendo respuesta o conexión cerrada.\n";
                    break;
                }
                respuesta[valread] = '\0';

                if (strncmp(respuesta, "OK", 2) == 0) {
                    std::cout << "Ingrese nuevo estado (1 = pagada, 0 = no pagada): ";
                    std::getline(std::cin, input);
                    int nuevo_estado = std::stoi(input);

                    snprintf(mensaje, sizeof(mensaje), "UPDATE_FINE_STATE:%d:%d", id_multa, nuevo_estado);
                    enviarMensaje(sock, mensaje); 
                } else {
                    std::cout << "Respuesta del servidor:\n" << respuesta << "\n";
                }

                break;
            }

            case 6:
                std::cout << "Ingrese ID del accidente: ";
                std::cin.getline(input, sizeof(input));
                snprintf(mensaje, sizeof(mensaje), "GET_ACCIDENT:%s", input);
                enviarMensaje(sock, mensaje);
                break;

            case 0:
                std::cout << "Cerrando sesion...\n";
                sesionActiva = 0;
                usuarioActual->setUsuario("");
                strcpy(rolActual, "");
                break;

            default:
                std::cout << "Opcion no valida\n";
                break;
        }
    } while (opcion != 0);
}

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;

    std::cout << "=== CLIENTE DGT ===\n";
    std::cout << "Conectando al servidor " << SERVER_IP << ":" << PORT << std::endl;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Error al inicializar Winsock" << std::endl;
        return -1;
    }

    // Crear socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cout << "Error creando socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    // Configurar dirección del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (serv_addr.sin_addr.s_addr == INADDR_NONE) {
        std::cout << "Dirección IP inválida" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cout << "Fallo al conectar: " << WSAGetLastError() << std::endl;
        std::cout << "Asegurate de que el servidor esté ejecutándose" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    std::cout << "Conectado al servidor exitosamente!\n";

    while (1) {
        if (!sesionActiva) {
            mostrarMenuInicial();
            int opcion;
            std::cin >> opcion;

            switch (opcion) {
                case 1:
                    if (manejarLogin(sock)) {
                        std::cout << "Sesion iniciada correctamente\n";
                    }
                    break;

                case 2:
                    manejarRegistro(sock);
                    break;

                case 3:
                    enviarMensaje(sock, "ping");
                    break;

                case 4:
                    enviarMensaje(sock, "estado");
                    break;

                case 0:
                    std::cout << "Saliendo...\n";
                    enviarMensaje(sock, "Desconectado");
                    goto cleanup;

                default:
                    std::cout << "Opcion no valida\n";
                    break;
            }
        } else {
            if (strcmp(rolActual, "usuario") == 0) {
                manejarComandosUsuario(sock);
            } else if (strcmp(rolActual, "admin") == 0) {
                manejarComandosAdmin(sock);
            }
        }
    }

cleanup:
    delete usuarioActual;
    delete[] rolActual;   

    closesocket(sock);
    WSACleanup();
    std::cout << "Cliente desconectado\n";
    return 0;
}