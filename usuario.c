#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "database.h"

#define MAX_INPUT 100

// variable global para guardar el usuario actual
extern char usuarioActual[50];
// declaración externa de la base de datos
extern sqlite3 *db;

void consultarDatosUsuario() {
    printf("Consultando datos del usuario...\n");
    
    char sql[200];
    sprintf(sql, "SELECT nombre, apellidos, dni, email, telefono FROM usuarios WHERE usuario = ?");
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        printf("Error preparando consulta de datos personales: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    rc = sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        printf("Error vinculando parametro de usuario: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        printf("\n--- Datos Personales ---\n");
        printf("Nombre: %s\n", sqlite3_column_text(stmt, 0) ? (char*)sqlite3_column_text(stmt, 0) : "N/A");
        printf("Apellidos: %s\n", sqlite3_column_text(stmt, 1) ? (char*)sqlite3_column_text(stmt, 1) : "N/A");
        printf("DNI: %s\n", sqlite3_column_text(stmt, 2) ? (char*)sqlite3_column_text(stmt, 2) : "N/A");
        printf("Email: %s\n", sqlite3_column_text(stmt, 3) ? (char*)sqlite3_column_text(stmt, 3) : "N/A");
        printf("Telefono: %s\n", sqlite3_column_text(stmt, 4) ? (char*)sqlite3_column_text(stmt, 4) : "N/A");
    } else if (rc == SQLITE_DONE) {
        printf("No se encontraron datos personales para este usuario.\n");
    } else {
        printf("Error al consultar datos personales: %s\n", sqlite3_errmsg(db));
    }
    
    sqlite3_finalize(stmt);
}

void consultarDatosVehiculos() {
    char sql[200];
    sprintf(sql, "SELECT matricula, marca, modelo, anio, color, tipo_vehiculo FROM vehiculos WHERE usuario='%s'", usuarioActual);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al consultar vehiculos.\n");
        return;
    }
    
    printf("\n--- Mis Vehiculos ---\n");
    printf("%-15s %-15s %-15s %-6s %-10s %-15s\n", 
           "Matricula", "Marca", "Modelo", "Anio", "Color", "Tipo Vehiculo");
    printf("------------------------------------------------------------\n");
    
    int encontrados = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrados = 1;
        printf("%-15s %-15s %-15s %-6d %-10s %-15s\n",
               sqlite3_column_text(stmt, 0) ? (char*)sqlite3_column_text(stmt, 0) : "N/A",
               sqlite3_column_text(stmt, 1) ? (char*)sqlite3_column_text(stmt, 1) : "N/A",
               sqlite3_column_text(stmt, 2) ? (char*)sqlite3_column_text(stmt, 2) : "N/A",
               sqlite3_column_int(stmt, 3),
               sqlite3_column_text(stmt, 4) ? (char*)sqlite3_column_text(stmt, 4) : "N/A",
               sqlite3_column_text(stmt, 5) ? (char*)sqlite3_column_text(stmt, 5) : "N/A");
    }
    
    if (!encontrados) {
        printf("No se encontraron vehiculos registrados.\n");
    }
    
    sqlite3_finalize(stmt);
}

void modificarInformacionVehiculo() {
    printf("Modificando informacion del vehiculo...\n");
}

void agregarNuevoVehiculo() {
    char matricula[MAX_INPUT];
    char marca[MAX_INPUT];
    char modelo[MAX_INPUT];
    char color[MAX_INPUT];
    char tipo_vehiculo[MAX_INPUT];
    int anio;

    printf("\n--- Agregar Nuevo Vehiculo ---\n");
    
    // verifica si el usuario ya tiene 3 vehiculos (límite)
    char sql_count[200];
    sprintf(sql_count, "SELECT COUNT(*) FROM vehiculos WHERE usuario='%s'", usuarioActual);
    
    sqlite3_stmt *stmt_count;
    int vehiculo_count = 0;
    
    if (sqlite3_prepare_v2(db, sql_count, -1, &stmt_count, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt_count) == SQLITE_ROW) {
            vehiculo_count = sqlite3_column_int(stmt_count, 0);
        }
        sqlite3_finalize(stmt_count);
    }
    
    if (vehiculo_count >= 3) {
        printf("Ha alcanzado el limite maximo de vehiculos (3).\n");
        return;
    }

    // solicita datos del vehiculo
    printf("Matricula: ");
    scanf("%s", matricula);
    
    // verifica si la matricula ya existe
    char sql_check[200];
    sprintf(sql_check, "SELECT 1 FROM vehiculos WHERE matricula='%s'", matricula);
    
    sqlite3_stmt *stmt_check;
    int matricula_existe = 0;
    
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt_check) == SQLITE_ROW) {
            matricula_existe = 1;
        }
        sqlite3_finalize(stmt_check);
    }
    
    if (matricula_existe) {
        printf("La matricula ya existe en el sistema.\n");
        return;
    }

    getchar(); // limpia buffer
    
    printf("Marca: ");
    fgets(marca, MAX_INPUT, stdin);
    marca[strcspn(marca, "\n")] = 0;
    
    printf("Modelo: ");
    fgets(modelo, MAX_INPUT, stdin);
    modelo[strcspn(modelo, "\n")] = 0;
    
    printf("Anio: ");
    scanf("%d", &anio);
    
    getchar(); // limpia buffer
    
    printf("Color: ");
    fgets(color, MAX_INPUT, stdin);
    color[strcspn(color, "\n")] = 0;
    
    printf("Tipo de Vehiculo (coche/moto/camion): ");
    fgets(tipo_vehiculo, MAX_INPUT, stdin);
    tipo_vehiculo[strcspn(tipo_vehiculo, "\n")] = 0;

    // prepara consulta de inserción
    char sql[500];
    char *errMsg = 0;
    
    sprintf(sql, "INSERT INTO vehiculos (usuario, matricula, marca, modelo, anio, color, tipo_vehiculo) "
                 "VALUES ('%s', '%s', '%s', '%s', %d, '%s', '%s');", 
            usuarioActual, matricula, marca, modelo, anio, color, tipo_vehiculo);
    
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("Error al registrar vehiculo: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Vehiculo registrado correctamente.\n");
    }
}

void consultarTodosLosUsuarios() {
    printf("Consultando todos los usuarios...\n");
    
    char sql[] = "SELECT usuario, nombre, apellidos, dni, email, telefono FROM usuarios";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al consultar usuarios.\n");
        return;
    }
    
    printf("\n--- Lista de Usuarios ---\n");
    printf("%-15s %-15s %-20s %-10s %-25s %-15s\n", 
           "Usuario", "Nombre", "Apellidos", "DNI", "Email", "Telefono");
    printf("---------------------------------------------------------------------------------\n");
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-15s %-15s %-20s %-10s %-25s %-15s\n",
               sqlite3_column_text(stmt, 0) ? (char*)sqlite3_column_text(stmt, 0) : "N/A",
               sqlite3_column_text(stmt, 1) ? (char*)sqlite3_column_text(stmt, 1) : "N/A",
               sqlite3_column_text(stmt, 2) ? (char*)sqlite3_column_text(stmt, 2) : "N/A",
               sqlite3_column_text(stmt, 3) ? (char*)sqlite3_column_text(stmt, 3) : "N/A",
               sqlite3_column_text(stmt, 4) ? (char*)sqlite3_column_text(stmt, 4) : "N/A",
               sqlite3_column_text(stmt, 5) ? (char*)sqlite3_column_text(stmt, 5) : "N/A");
    }
    
    sqlite3_finalize(stmt);
}

void consultarTodosLosVehiculos() {
    char sql[] = "SELECT usuario, matricula, marca, modelo, anio, color, tipo_vehiculo FROM vehiculos";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al consultar vehiculos.\n");
        return;
    }
    
    printf("\n--- Lista de Vehiculos ---\n");
    printf("%-15s %-15s %-15s %-15s %-6s %-10s %-15s\n", 
           "Usuario", "Matricula", "Marca", "Modelo", "Anio", "Color", "Tipo Vehiculo");
    printf("--------------------------------------------------------------------------\n");
    
    int encontrados = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrados = 1;
        printf("%-15s %-15s %-15s %-15s %-6d %-10s %-15s\n",
               sqlite3_column_text(stmt, 0) ? (char*)sqlite3_column_text(stmt, 0) : "N/A",
               sqlite3_column_text(stmt, 1) ? (char*)sqlite3_column_text(stmt, 1) : "N/A",
               sqlite3_column_text(stmt, 2) ? (char*)sqlite3_column_text(stmt, 2) : "N/A",
               sqlite3_column_text(stmt, 3) ? (char*)sqlite3_column_text(stmt, 3) : "N/A",
               sqlite3_column_int(stmt, 4),
               sqlite3_column_text(stmt, 5) ? (char*)sqlite3_column_text(stmt, 5) : "N/A",
               sqlite3_column_text(stmt, 6) ? (char*)sqlite3_column_text(stmt, 6) : "N/A");
    }
    
    if (!encontrados) {
        printf("No se encontraron vehiculos registrados.\n");
    }
    
    sqlite3_finalize(stmt);
}

void obtenerDatosPersonales(char *usuario) {
    // esta funcion ya no es necesaria ya que su funcionalidad esta incorporada en consultarDatosUsuario
    // se mantiene la declaración para evitar errores de compilación
}