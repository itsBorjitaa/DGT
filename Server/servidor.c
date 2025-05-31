#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <sqlite3.h>
#include "logger.h"

//para ejecutar: gcc -o servidor.exe servidor.c logger.c sqlite3.c -lws2_32

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "sqlite3.lib")

#define PORT 5000
#define MAX_BUFFER 1024
#define MAX_CLIENTS 10

sqlite3 *db;

typedef struct {
    SOCKET socket;
    struct sockaddr_in address;
    char ip[INET_ADDRSTRLEN];
    int port;
} ClientInfo;

void inicializarDB() {
    int rc = sqlite3_open("db.db", &db);
    if (rc) {
        printf("Error al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    
    char *sql_usuarios = "CREATE TABLE IF NOT EXISTS usuarios ("
                        "usuario TEXT PRIMARY KEY, "
                        "contrasena TEXT, "
                        "rol TEXT, "
                        "nombre TEXT, "
                        "apellidos TEXT, "
                        "dni TEXT, "
                        "email TEXT, "
                        "telefono TEXT"
                        ");";
    sqlite3_exec(db, sql_usuarios, 0, 0, 0);
    
    char *sql_vehiculos = "CREATE TABLE IF NOT EXISTS vehiculos ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "usuario TEXT, "
                         "matricula TEXT UNIQUE, "
                         "marca TEXT, "
                         "modelo TEXT, "
                         "anio INTEGER, "
                         "color TEXT, "
                         "tipo_vehiculo TEXT, "
                         "FOREIGN KEY(usuario) REFERENCES usuarios(usuario)"
                         ");";
    sqlite3_exec(db, sql_vehiculos, 0, 0, 0);
    
    char *sql_multas = "CREATE TABLE IF NOT EXISTS multas ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "dni TEXT, "
                      "concepto TEXT, "
                      "fecha_delito TEXT, "
                      "importe REAL, "
                      "fecha_limite_descuento TEXT, "
                      "pagada INTEGER DEFAULT 0, "
                      "fecha_pago TEXT, "
                      "FOREIGN KEY(dni) REFERENCES usuarios(dni)"
                      ");";
    sqlite3_exec(db, sql_multas, 0, 0, 0);
    
    char *sql_accidentes = "CREATE TABLE IF NOT EXISTS accidentes ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "usuario TEXT, "
                          "fecha TEXT, "
                          "descripcion TEXT, "
                          "FOREIGN KEY(usuario) REFERENCES usuarios(usuario)"
                          ");";
    sqlite3_exec(db, sql_accidentes, 0, 0, 0);
    
    char *sql_admin = "INSERT OR IGNORE INTO usuarios (usuario, contrasena, rol, nombre, apellidos, dni, email, telefono) "
                     "VALUES ('admin', 'admin123', 'admin', 'Administrador', 'Sistema', '00000000A', 'admin@dgt.es', '000000000');";
    sqlite3_exec(db, sql_admin, 0, 0, 0);
    
    printf("Base de datos inicializada correctamente\n");
}

void getClientIP(struct sockaddr_in *client_addr, char *ip_str) {
    char* ip_temp = inet_ntoa(client_addr->sin_addr);
    if (ip_temp != NULL) {
        strncpy(ip_str, ip_temp, INET_ADDRSTRLEN - 1);
        ip_str[INET_ADDRSTRLEN - 1] = '\0';
    } else {
        strcpy(ip_str, "unknown");
    }
}

void obtenerFechaActual(char* fecha) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(fecha, 20, "%Y-%m-%d", tm_info);
}

int verificarCredenciales(const char* usuario, const char* contrasena, char* rol) {
    char sql[300];
    sprintf(sql, "SELECT rol FROM usuarios WHERE usuario='%s' AND contrasena='%s'", usuario, contrasena);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        return 0;
    }
    
    int resultado = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(rol, (char *)sqlite3_column_text(stmt, 0));
        resultado = 1;
    }
    
    sqlite3_finalize(stmt);
    return resultado;
}

//registrar usuario
int registrarUsuario(const char* usuario, const char* contrasena, const char* rol, 
                    const char* nombre, const char* apellidos, const char* dni, 
                    const char* email, const char* telefono) {
    char sql[600];
    sprintf(sql, "INSERT INTO usuarios (usuario, contrasena, rol, nombre, apellidos, dni, email, telefono) "
                 "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", 
            usuario, contrasena, rol, nombre, apellidos, dni, email, telefono);
    
    return sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK;
}

//consultar datos de usuario
void consultarDatosUsuario(const char* usuario, char* response) {
    char sql[300];
    sprintf(sql, "SELECT nombre, apellidos, dni, email, telefono FROM usuarios WHERE usuario='%s'", usuario);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        strcpy(response, "ERROR: No se pudieron consultar los datos");
        return;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sprintf(response, "DATOS PERSONALES:\nNombre: %s\nApellidos: %s\nDNI: %s\nEmail: %s\nTelefono: %s", 
                sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1), 
                sqlite3_column_text(stmt, 2), sqlite3_column_text(stmt, 3), 
                sqlite3_column_text(stmt, 4));
    } else {
        strcpy(response, "ERROR: Usuario no encontrado");
    }
    
    sqlite3_finalize(stmt);
}

char* consultarVehiculosUsuario(const char* usuario) {
    char sql[300];
    sprintf(sql, "SELECT matricula, marca, modelo, anio, color, tipo_vehiculo FROM vehiculos WHERE usuario='%s'", usuario);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        char* errorMsg = malloc(50);
        if (errorMsg) strcpy(errorMsg, "ERROR: No se pudieron consultar los vehiculos");
        return errorMsg;
    }

    size_t buffer_size = 1024;
    char* response = malloc(buffer_size);
    if (!response) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    strcpy(response, "VEHICULOS REGISTRADOS:\n");

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char vehiculo[200];
        sprintf(vehiculo, "Matricula: %s | Marca: %s | Modelo: %s | Año: %d | Color: %s | Tipo: %s\n",
                sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2), sqlite3_column_int(stmt, 3),
                sqlite3_column_text(stmt, 4), sqlite3_column_text(stmt, 5));

        size_t needed = strlen(response) + strlen(vehiculo) + 1;
        if (needed > buffer_size) {
            buffer_size *= 2;
            char* tmp = realloc(response, buffer_size);
            if (!tmp) {
                free(response);
                sqlite3_finalize(stmt);
                return NULL;
            }
            response = tmp;
        }

        strcat(response, vehiculo);
        count++;
    }

    if (count == 0) {
        strcat(response, "No tiene vehiculos registrados");
    }

    sqlite3_finalize(stmt);
    return response; 
}


void consultarMultasUsuario(const char* usuario, char* response) {
    char sql[400];
    sprintf(sql, "SELECT m.id, m.concepto, m.fecha_delito, m.importe, m.fecha_limite_descuento, m.pagada "
                 "FROM multas m JOIN usuarios u ON m.dni = u.dni WHERE u.usuario='%s'", usuario);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        strcpy(response, "ERROR: No se pudieron consultar las multas");
        return;
    }
    
    strcpy(response, "MULTAS REGISTRADAS:\n");
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char multa[300];
        sprintf(multa, "ID: %d | Concepto: %s | Fecha: %s | Importe: %.2f€ | Limite descuento: %s | Pagada: %s\n",
                sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2), sqlite3_column_double(stmt, 3),
                sqlite3_column_text(stmt, 4), sqlite3_column_int(stmt, 5) ? "Si" : "No");
        strcat(response, multa);
        count++;
    }
    
    if (count == 0) {
        strcat(response, "No tiene multas registradas");
    }
    
    sqlite3_finalize(stmt);
}

int agregarAccidente(const char* usuario, const char* descripcion) {
    char fecha[20];
    obtenerFechaActual(fecha);
    
    char sql[500];
    sprintf(sql, "INSERT INTO accidentes (usuario, fecha, descripcion) VALUES ('%s', '%s', '%s')", 
            usuario, fecha, descripcion);
    
    return sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK;
}

void consultarAccidentesUsuario(const char* usuario, char* response) {
    char sql[300];
    sprintf(sql, "SELECT id, fecha, descripcion FROM accidentes WHERE usuario='%s'", usuario);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        strcpy(response, "ERROR: No se pudieron consultar los accidentes");
        return;
    }
    
    strcpy(response, "ACCIDENTES REGISTRADOS:\n");
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char accidente[300];
        sprintf(accidente, "ID: %d | Fecha: %s | Descripcion: %s\n",
                sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2));
        strcat(response, accidente);
        count++;
    }
    
    if (count == 0) {
        strcat(response, "No tiene accidentes registrados");
    }
    
    sqlite3_finalize(stmt);
}

void consultarAccidentePorId(int id_accidente, char* response) {
    char sql[300];
    sprintf(sql, "SELECT a.id, a.usuario, a.fecha, a.descripcion, u.nombre, u.apellidos "
                 "FROM accidentes a JOIN usuarios u ON a.usuario = u.usuario "
                 "WHERE a.id = %d", id_accidente);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        strcpy(response, "ERROR: No se pudo consultar el accidente");
        return;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sprintf(response, "ACCIDENTE ENCONTRADO:\n"
                         "ID: %d\n"
                         "Usuario: %s\n"
                         "Nombre completo: %s %s\n"
                         "Fecha: %s\n"
                         "Descripcion: %s",
                sqlite3_column_int(stmt, 0),
                sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 4),
                sqlite3_column_text(stmt, 5),
                sqlite3_column_text(stmt, 2),
                sqlite3_column_text(stmt, 3));
    } else {
        sprintf(response, "ERROR: No se encontro ningun accidente con ID %d", id_accidente);
    }
    
    sqlite3_finalize(stmt);
}

void consultarTodosUsuarios(char* response) {
    char sql[] = "SELECT usuario, nombre, apellidos, dni, email, telefono FROM usuarios";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        strcpy(response, "ERROR: No se pudieron consultar los usuarios");
        return;
    }
    
    strcpy(response, "TODOS LOS USUARIOS:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char usuario[200];
        sprintf(usuario, "Usuario: %s | Nombre: %s %s | DNI: %s | Email: %s | Tel: %s\n",
                sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2), sqlite3_column_text(stmt, 3),
                sqlite3_column_text(stmt, 4), sqlite3_column_text(stmt, 5));
        strcat(response, usuario);
    }
    
    sqlite3_finalize(stmt);
}

void consultarTodosVehiculos(char* response) {
    char sql[] = "SELECT v.usuario, v.matricula, v.marca, v.modelo, v.anio, v.color, v.tipo_vehiculo FROM vehiculos v";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        strcpy(response, "ERROR: No se pudieron consultar los vehiculos");
        return;
    }
    
    strcpy(response, "TODOS LOS VEHICULOS:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char vehiculo[300];
        sprintf(vehiculo, "Usuario: %s | Matricula: %s | Marca: %s | Modelo: %s | Año: %d | Color: %s | Tipo: %s\n",
                sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2), sqlite3_column_text(stmt, 3),
                sqlite3_column_int(stmt, 4), sqlite3_column_text(stmt, 5),
                sqlite3_column_text(stmt, 6));
        strcat(response, vehiculo);
    }
    
    sqlite3_finalize(stmt);
}

void consultarTodasMultas(char* response) {
    char sql[] = "SELECT id, dni, concepto, fecha_delito, importe, fecha_limite_descuento, pagada FROM multas";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        strcpy(response, "ERROR: No se pudieron consultar las multas");
        return;
    }
    
    strcpy(response, "TODAS LAS MULTAS:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char multa[300];
        sprintf(multa, "ID: %d | DNI: %s | Concepto: %s | Fecha: %s | Importe: %.2f€ | Limite: %s | Pagada: %s\n",
                sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2), sqlite3_column_text(stmt, 3),
                sqlite3_column_double(stmt, 4), sqlite3_column_text(stmt, 5),
                sqlite3_column_int(stmt, 6) ? "Si" : "No");
        strcat(response, multa);
    }
    
    sqlite3_finalize(stmt);
}

int agregarVehiculo(const char* usuario, const char* matricula, const char* marca, 
                   const char* modelo, int anio, const char* color, const char* tipo) {
    char sql[500];
    sprintf(sql, "INSERT INTO vehiculos (usuario, matricula, marca, modelo, anio, color, tipo_vehiculo) "
                 "VALUES ('%s', '%s', '%s', '%s', %d, '%s', '%s')", 
            usuario, matricula, marca, modelo, anio, color, tipo);
    
    return sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK;
}

int modificarVehiculo(const char* usuario, const char* matricula, const char* marca, 
                     const char* modelo, int anio, const char* color, const char* tipo) {
    char sql[600];
    sprintf(sql, "UPDATE vehiculos SET marca='%s', modelo='%s', anio=%d, color='%s', tipo_vehiculo='%s' "
                 "WHERE matricula='%s' AND usuario='%s'", 
            marca, modelo, anio, color, tipo, matricula, usuario);
    
    int result = sqlite3_exec(db, sql, 0, 0, 0);
    
    if (result == SQLITE_OK && sqlite3_changes(db) > 0) {
        return 1;
    }
    return 0;
}

void obtenerDatosVehiculo(const char* usuario, const char* matricula, char* response) {
    char sql[400];
    sprintf(sql, "SELECT matricula, marca, modelo, anio, color, tipo_vehiculo "
                 "FROM vehiculos WHERE usuario='%s' AND matricula='%s'", usuario, matricula);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        strcpy(response, "ERROR: No se pudo consultar el vehiculo");
        return;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sprintf(response, "DATOS ACTUALES DEL VEHICULO:\n"
                         "Matricula: %s\n"
                         "Marca: %s\n"
                         "Modelo: %s\n"
                         "Año: %d\n"
                         "Color: %s\n"
                         "Tipo: %s\n\n"
                         "Introduce los nuevos datos (marca,modelo,año,color,tipo):",
                sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2), sqlite3_column_int(stmt, 3),
                sqlite3_column_text(stmt, 4), sqlite3_column_text(stmt, 5));
    } else {
        strcpy(response, "ERROR: No se encontro el vehiculo con esa matricula o no le pertenece");
    }
    
    sqlite3_finalize(stmt);
}

void guardarDatosUsuarioEnTXT(const char* usuario, const char* nombreArchivo) {

    char nombreArchivoCompleto[120];
    snprintf(nombreArchivoCompleto, sizeof(nombreArchivoCompleto), "%s.txt", nombreArchivo);


    FILE *archivo = fopen(nombreArchivoCompleto, "w");
    if (archivo == NULL) {
        printf("Error al crear el archivo.");
        return;
    }

    // --- Datos personales ---
    char sql[300];
    sqlite3_stmt *stmt;
    sprintf(sql, "SELECT nombre, apellidos, dni, email, telefono FROM usuarios WHERE usuario = ?");
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Error en consulta SQL: %s\n", sqlite3_errmsg(db));
        fclose(archivo);
        return;
    }

    sqlite3_bind_text(stmt, 1, usuario, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        fprintf(archivo, "----- DATOS PERSONALES -----\n");
        fprintf(archivo, "Nombre: %s\n", sqlite3_column_text(stmt, 0));
        fprintf(archivo, "Apellidos: %s\n", sqlite3_column_text(stmt, 1));
        fprintf(archivo, "DNI: %s\n", sqlite3_column_text(stmt, 2));
        fprintf(archivo, "Email: %s\n", sqlite3_column_text(stmt, 3));
        fprintf(archivo, "Teléfono: %s\n\n", sqlite3_column_text(stmt, 4));
    } else {
        fprintf(archivo, "No se encontraron datos personales.\n\n");
    }
    sqlite3_finalize(stmt);

    // --- Vehículos ---
    sprintf(sql, "SELECT matricula, marca, modelo, anio, color, tipo_vehiculo FROM vehiculos WHERE usuario = ?");
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, usuario, -1, SQLITE_STATIC);
        fprintf(archivo, "----- VEHÍCULOS REGISTRADOS -----\n");
        int encontrado = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            encontrado = 1;
            fprintf(archivo, "Matrícula: %s\n", sqlite3_column_text(stmt, 0));
            fprintf(archivo, "Marca: %s\n", sqlite3_column_text(stmt, 1));
            fprintf(archivo, "Modelo: %s\n", sqlite3_column_text(stmt, 2));
            fprintf(archivo, "Año: %d\n", sqlite3_column_int(stmt, 3));
            fprintf(archivo, "Color: %s\n", sqlite3_column_text(stmt, 4));
            fprintf(archivo, "Tipo: %s\n\n", sqlite3_column_text(stmt, 5));
        }
        if (!encontrado) {
            fprintf(archivo, "No tiene vehículos registrados.\n\n");
        }
        sqlite3_finalize(stmt);
    }

    // --- Multas ---
    sprintf(sql, "SELECT m.id, m.concepto, m.fecha_delito, m.importe, m.fecha_limite_descuento, m.pagada "
                 "FROM multas m JOIN usuarios u ON m.dni = u.dni WHERE u.usuario = ?");
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, usuario, -1, SQLITE_STATIC);
        fprintf(archivo, "----- MULTAS -----\n");
        int encontrado = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            encontrado = 1;
            fprintf(archivo, "ID: %d\n", sqlite3_column_int(stmt, 0));
            fprintf(archivo, "Concepto: %s\n", sqlite3_column_text(stmt, 1));
            fprintf(archivo, "Fecha Delito: %s\n", sqlite3_column_text(stmt, 2));
            fprintf(archivo, "Importe: %.2f€\n", sqlite3_column_double(stmt, 3));
            fprintf(archivo, "Límite Descuento: %s\n", sqlite3_column_text(stmt, 4));
            fprintf(archivo, "Pagada: %s\n\n", sqlite3_column_int(stmt, 5) ? "Sí" : "No");
        }
        if (!encontrado) {
            fprintf(archivo, "No tiene multas registradas.\n\n");
        }
        sqlite3_finalize(stmt);
    }

    // --- Accidentes ---
    sprintf(sql, "SELECT fecha, descripcion FROM accidentes WHERE usuario = ?");
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, usuario, -1, SQLITE_STATIC);
        fprintf(archivo, "----- ACCIDENTES -----\n");
        int encontrado = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            encontrado = 1;
            fprintf(archivo, "Fecha: %s\n", sqlite3_column_text(stmt, 0));
            fprintf(archivo, "Descripción: %s\n\n", sqlite3_column_text(stmt, 1));
        }
        if (!encontrado) {
            fprintf(archivo, "No tiene accidentes registrados.\n\n");
        }
        sqlite3_finalize(stmt);
    }

  fclose(archivo);
}

int agregarMulta(const char* dni, const char* concepto, const char* fecha_delito, 
                double importe, const char* fecha_limite) {
    char sql[500];
    sprintf(sql, "INSERT INTO multas (dni, concepto, fecha_delito, importe, fecha_limite_descuento, pagada) "
                 "VALUES ('%s', '%s', '%s', %.2f, '%s', 0)", 
            dni, concepto, fecha_delito, importe, fecha_limite);
    
    return sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK;
}

int pagarMulta(const char* usuario, int id_multa) {
    char fecha[20];
    obtenerFechaActual(fecha);
    
    char sql[300];
    sprintf(sql, "UPDATE multas SET pagada = 1, fecha_pago = '%s' WHERE id = %d AND dni IN "
                 "(SELECT dni FROM usuarios WHERE usuario = '%s')", fecha, id_multa, usuario);
    
    return sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK;
}

void cambiarEstadoMulta() {
    int id_multa;
    char usuario[50]; 

    printf("\n--- Cambiar Estado de Multa ---\n");
    printf("Usuario: ");
    scanf("%49s", usuario);

    printf("ID de la multa: ");
    scanf("%d", &id_multa);

    char sql_check[300];
    sprintf(sql_check, "SELECT pagada FROM multas WHERE id=%d AND dni IN (SELECT dni FROM usuarios WHERE usuario='%s')", id_multa, usuario);

    sqlite3_stmt *stmt_check;
    int multa_existe = 0;
    int ya_pagada = 0;

    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt_check) == SQLITE_ROW) {
            multa_existe = 1;
            ya_pagada = sqlite3_column_int(stmt_check, 0);
        }
        sqlite3_finalize(stmt_check);
    }

    if (!multa_existe) {
        printf("La multa con ID %d no existe para el usuario %s.\n", id_multa, usuario);
        return;
    }

    if (ya_pagada) {
        printf("La multa ya está marcada como pagada.\n");
        return;
    }

    if (pagarMulta(usuario, id_multa)) {
        printf("Estado de la multa actualizado correctamente.\n");
    } else {
        printf("Error al actualizar el estado de la multa.\n");
    }
}

void processClientMessage(const char* message, char* response, size_t response_size) {
    char *token;
    char message_copy[MAX_BUFFER];
    strcpy(message_copy, message);

    token = strtok(message_copy, ":");

    if (strcmp(token, "LOGIN") == 0) {
        char *usuario = strtok(NULL, ":");
        char *contrasena = strtok(NULL, ":");
        char rol[20];

        if (verificarCredenciales(usuario, contrasena, rol)) {
            snprintf(response, response_size, "LOGIN_OK:%s", rol);
            registrarAccion(usuario, "Inicio de sesión exitoso");
        } else {
            snprintf(response, response_size, "LOGIN_FAILED:Credenciales incorrectas");
            registrarAccion(usuario, "Intento fallido de inicio de sesión");
        }
    }
    else if (strcmp(token, "REGISTER") == 0) {
        char *usuario = strtok(NULL, ":");
        char *contrasena = strtok(NULL, ":");
        char *rol = strtok(NULL, ":");
        char *nombre = strtok(NULL, ":");
        char *apellidos = strtok(NULL, ":");
        char *dni = strtok(NULL, ":");
        char *email = strtok(NULL, ":");
        char *telefono = strtok(NULL, ":");

        if (registrarUsuario(usuario, contrasena, rol, nombre, apellidos, dni, email, telefono)) {
            snprintf(response, response_size, "REGISTER_OK:Usuario registrado correctamente");
            registrarAccion(usuario, "Registro exitoso");
        } else {
            snprintf(response, response_size, "REGISTER_FAILED:Error al registrar usuario");
            registrarAccion(usuario, "Error al registrar usuario");
        }
    }
    else if (strcmp(token, "USER_DATA") == 0) {
        char *usuario = strtok(NULL, ":");
        consultarDatosUsuario(usuario, response);
        registrarAccion(usuario, "Consulta de datos personales");
    }
    else if (strcmp(token, "USER_VEHICLES") == 0) {
        char *usuario = strtok(NULL, ":");
        char *vehiculos_info = consultarVehiculosUsuario(usuario);
        if (vehiculos_info) {
            strcpy(response, vehiculos_info); 
            free(vehiculos_info);              
        } else {
            strcpy(response, "ERROR: No se pudo consultar los vehículos.");
        }
        registrarAccion(usuario, "Consulta de vehículos personales");
    }   
    else if (strcmp(token, "USER_FINES") == 0) {
        char *usuario = strtok(NULL, ":");
        consultarMultasUsuario(usuario, response);
        registrarAccion(usuario, "Consulta de multas personales");
    }
    else if (strcmp(token, "USER_ACCIDENTS") == 0) {
        char *usuario = strtok(NULL, ":");
        consultarAccidentesUsuario(usuario, response);
        registrarAccion(usuario, "Consulta de accidentes personales");
    }
    else if (strcmp(token, "ADD_ACCIDENT") == 0) {
        char *usuario = strtok(NULL, ":");
        char *descripcion = strtok(NULL, ":");

        if (agregarAccidente(usuario, descripcion)) {
            snprintf(response, response_size, "ACCIDENT_ADDED:Accidente registrado correctamente");
            registrarAccion(usuario, "Accidente registrado");
        } else {
            snprintf(response, response_size, "ACCIDENT_FAILED:Error al registrar accidente");
            registrarAccion(usuario, "Error al registrar accidente");
        }
    }
    else if (strcmp(token, "ADD_VEHICLE") == 0) {
        char *usuario = strtok(NULL, ":");
        char *datos = strtok(NULL, ":");


        char *matricula = strtok(datos, ",");
        char *marca = strtok(NULL, ",");
        char *modelo = strtok(NULL, ",");
        char *anio_str = strtok(NULL, ",");
        char *color = strtok(NULL, ",");
        char *tipo = strtok(NULL, ",");

        if (matricula && marca && modelo && anio_str && color && tipo) {
            int anio = atoi(anio_str);
            if (agregarVehiculo(usuario, matricula, marca, modelo, anio, color, tipo)) {
                snprintf(response, response_size, "VEHICLE_ADDED:Vehiculo agregado correctamente");
                registrarAccion(usuario, "Vehículo agregado");
            } else {
                snprintf(response, response_size, "VEHICLE_FAILED:Error al agregar vehiculo");
                registrarAccion(usuario, "Error al agregar vehículo");
            }
        } else {
            snprintf(response, response_size, "VEHICLE_FAILED:Datos incompletos");
            registrarAccion(usuario, "Datos incompletos al agregar vehículo");
        }
    }
    else if (strcmp(token, "PAY_FINE") == 0) {
        char *usuario = strtok(NULL, ":");
        char *id_str = strtok(NULL, ":");

        if (id_str) {
            int id_multa = atoi(id_str);
            if (pagarMulta(usuario, id_multa)) {
                snprintf(response, response_size, "FINE_PAID:Multa pagada correctamente");
                registrarAccion(usuario, "Multa pagada");
            } else {
                snprintf(response, response_size, "FINE_FAILED:Error al pagar la multa");
                registrarAccion(usuario, "Error al pagar multa");
            }
        } else {
            snprintf(response, response_size, "FINE_FAILED:ID de multa inválido");
            registrarAccion(usuario, "ID de multa inválido al intentar pagar");
        }
    }
    else if (strcmp(token, "ADMIN_ALL_USERS") == 0) {
        consultarTodosUsuarios(response);
        registrarAccion("ADMIN", "Consulta de todos los usuarios");
    }
    else if (strcmp(token, "ADMIN_ALL_VEHICLES") == 0) {
        consultarTodosVehiculos(response);
        registrarAccion("ADMIN", "Consulta de todos los vehículos");
    }
    else if (strcmp(token, "ADMIN_ALL_FINES") == 0) {
        consultarTodasMultas(response);
        registrarAccion("ADMIN", "Consulta de todas las multas");
    }
    else if (strcmp(token, "ADD_FINE") == 0) {
        char *datos = strtok(NULL, ":");


        char *dni = strtok(datos, ",");
        char *concepto = strtok(NULL, ",");
        char *fecha_delito = strtok(NULL, ",");
        char *importe_str = strtok(NULL, ",");
        char *fecha_limite = strtok(NULL, ",");

        if (dni && concepto && fecha_delito && importe_str && fecha_limite) {
            double importe = atof(importe_str);
            if (agregarMulta(dni, concepto, fecha_delito, importe, fecha_limite)) {
                snprintf(response, response_size, "FINE_ADDED:Multa agregada correctamente");
                registrarAccion("ADMIN", "Multa agregada");
            } else {
                snprintf(response, response_size, "FINE_FAILED:Error al agregar multa");
                registrarAccion("ADMIN", "Error al agregar multa");
            }
        } else {
            snprintf(response, response_size, "FINE_FAILED:Datos incompletos");
            registrarAccion("ADMIN", "Datos incompletos al agregar multa");
        }
    }
    else if (strcmp(token, "ping") == 0) {
        snprintf(response, response_size, "PONG");
    }
    else if (strcmp(token, "estado") == 0) {
        snprintf(response, response_size, "Servidor DGT - Puerto %d - Base de datos activa", PORT);
    }
    else if (strcmp(token, "Desconectado") == 0) {
        snprintf(response, response_size, "Hasta luego - Conexion cerrada");
    }
    else if (strcmp(token, "GET_ACCIDENT") == 0) {
        char *id_str = strtok(NULL, ":");

        if (id_str) {
            int id_accidente = atoi(id_str);
            consultarAccidentePorId(id_accidente, response);
            registrarAccion("ADMIN", "Consulta de accidente por ID");
        } else {
            snprintf(response, response_size, "ERROR: ID de accidente no proporcionado");
        }
    }
    else if (strcmp(token, "EXPORT_DATA") == 0) {
        char *usuario = strtok(NULL, ":");
        char *nombreArchivo = strtok(NULL, ":");

        if (usuario && nombreArchivo) {
            guardarDatosUsuarioEnTXT(usuario, nombreArchivo);
            snprintf(response, response_size, "Datos exportados correctamente en el servidor.\n");
            registrarAccion(usuario, "Datos exportados a archivo");
        } else {
            snprintf(response, response_size, "ERROR: Formato incorrecto, use EXPORT_DATA:<usuario>:<nombreArchivo>\n");
        }
    } else if (strcmp(token, "MODIFY_VEHICLE") == 0) {
        char *usuario = strtok(NULL, ":");
        char *matricula = strtok(NULL, ":");

        if (usuario && matricula) {
            obtenerDatosVehiculo(usuario, matricula, response);
        } else {
            snprintf(response, response_size, "ERROR: Faltan datos (usuario o matrícula)");
        }
    } else if (strcmp(token, "MODIFY_VEHICLE_DATA") == 0) {
        char *usuario = strtok(NULL, ":");
        char *matricula = strtok(NULL, ":");
        char *datos = strtok(NULL, ":");

        if (usuario && matricula && datos) {
            char *marca = strtok(datos, ",");
            char *modelo = strtok(NULL, ",");
            char *anio_str = strtok(NULL, ",");
            char *color = strtok(NULL, ",");
            char *tipo = strtok(NULL, ",");

            if (marca && modelo && anio_str && color && tipo) {
                int anio = atoi(anio_str);
                if (modificarVehiculo(usuario, matricula, marca, modelo, anio, color, tipo)) {
                    snprintf(response, response_size, "VEHICLE_MODIFIED:Vehiculo modificado correctamente");
                    char log_msg[256];
                    snprintf(log_msg, sizeof(log_msg), "Modificó el vehículo %s", matricula);
                    registrarAccion(usuario, log_msg);
                } else {
                    snprintf(response, response_size, "VEHICLE_MODIFY_FAILED:No se modifico el vehiculo");
                }
        } else {
            snprintf(response, response_size, "ERROR: Datos incompletos para modificacion");
        }
    } else {
        snprintf(response, response_size, "ERROR: Faltan argumentos para modificar vehiculo");
    }
} else if (strcmp(token, "CHECK_FINE_EXISTS") == 0) {
    char *id_str = strtok(NULL, ":");
    int id = atoi(id_str);

    char sql[100];
    sprintf(sql, "SELECT 1 FROM multas WHERE id=%d", id);

    sqlite3_stmt *stmt;
    int existe = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            existe = 1;
        }
        sqlite3_finalize(stmt);
    }

    if (existe) {
        snprintf(response, response_size, "OK: Multa encontrada.");
    } else {
        snprintf(response, response_size, "ERROR: La multa con ID %d no existe.", id);
    }
} else if (strcmp(token, "UPDATE_FINE_STATE") == 0) {
    char *id_str = strtok(NULL, ":");
    char *estado_str = strtok(NULL, ":");

    if (id_str == NULL || estado_str == NULL) {
        snprintf(response, response_size, "ERROR: Faltan datos para actualizar multa.");
    } else {
        int id = atoi(id_str);
        int estado = atoi(estado_str);

        char sql_check[100];
        sprintf(sql_check, "SELECT 1 FROM multas WHERE id=%d", id);

        sqlite3_stmt *stmt_check;
        int existe = 0;

        if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) == SQLITE_OK) {
            if (sqlite3_step(stmt_check) == SQLITE_ROW) {
                existe = 1;
            }
            sqlite3_finalize(stmt_check);
        }

        if (!existe) {
            snprintf(response, response_size, "ERROR: La multa con ID %d no existe.", id);
        } else {
            char sql_update[200];
            sprintf(sql_update, "UPDATE multas SET pagada = %d WHERE id = %d", estado, id);

            if (sqlite3_exec(db, sql_update, NULL, NULL, NULL) == SQLITE_OK) {
                snprintf(response, response_size, "Multa con ID %d actualizada correctamente.", id);
                
                registrarAccion("admin", estado == 1 ? 
                    "Multa marcada como pagada" : "Multa marcada como no pagada");
            } else {
                snprintf(response, response_size, "ERROR: No se pudo actualizar la multa.");
            }
        }
    }
} else {
        snprintf(response, response_size, "UNKNOWN_COMMAND:Comando no reconocido");
    }
}  

    DWORD WINAPI handleClient(LPVOID lpParam) {
    ClientInfo *client = (ClientInfo*)lpParam;
    char buffer[MAX_BUFFER] = {0};
    char response[MAX_BUFFER * 2] = {0};

    printf("Cliente conectado desde %s:%d\n", client->ip, client->port);

    while (1) {
        memset(buffer, 0, MAX_BUFFER);
        memset(response, 0, sizeof(response));

        int valread = recv(client->socket, buffer, MAX_BUFFER - 1, 0);

        if (valread <= 0) {
            printf("Cliente %s:%d desconectado\n", client->ip, client->port);
            break;
        }

        buffer[valread] = '\0';
        printf("Mensaje de %s:%d -> %s\n", client->ip, client->port, buffer);

        processClientMessage(buffer, response, sizeof(response));

        send(client->socket, response, (int)strlen(response), 0);
    }

    closesocket(client->socket);
    free(client);
    remove("log.txt");
    return 0;
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    printf("=== SERVIDOR DGT ===\n");
    printf("Iniciando servidor en puerto %d...\n", PORT);
    
    inicializarDB();
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock\n");
        return -1;
    }
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error al crear socket: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("Error en setsockopt: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Error en bind: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }
    
    if (listen(server_fd, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Error en listen: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }
    
    printf("Servidor escuchando en puerto %d\n", PORT);
    printf("Esperando conexiones de clientes...\n\n");
    
    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        
        new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (new_socket == INVALID_SOCKET) {
            printf("Error en accept: %d\n", WSAGetLastError());
            continue;
        }
        
        ClientInfo *client = malloc(sizeof(ClientInfo));
        client->socket = new_socket;
        client->address = client_addr;
        client->port = ntohs(client_addr.sin_port);
        getClientIP(&client_addr, client->ip);
        
        HANDLE thread = CreateThread(NULL, 0, handleClient, client, 0, NULL);
        if (thread == NULL) {
            printf("Error creando hilo para cliente\n");
            closesocket(new_socket);
            free(client);
        } else {
            CloseHandle(thread);
        }
    }

    closesocket(server_fd);
    sqlite3_close(db);
    WSACleanup();
    return 0;
}