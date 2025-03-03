#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "database.h"

// variable global para guardar el usuario actual
extern char usuarioActual[50];
// declaración externa de la base de datos
extern sqlite3 *db;

void consultarDatosUsuario() {
    printf("Consultando datos del usuario...\n");
    
    char sql[200];
    sprintf(sql, "SELECT nombre, apellidos, dni, email, telefono FROM usuarios WHERE usuario='%s'", usuarioActual);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al consultar datos personales.\n");
        return;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("\n--- Datos Personales ---\n");
        printf("Nombre: %s\n", sqlite3_column_text(stmt, 0) ? (char*)sqlite3_column_text(stmt, 0) : "N/A");
        printf("Apellidos: %s\n", sqlite3_column_text(stmt, 1) ? (char*)sqlite3_column_text(stmt, 1) : "N/A");
        printf("DNI: %s\n", sqlite3_column_text(stmt, 2) ? (char*)sqlite3_column_text(stmt, 2) : "N/A");
        printf("Email: %s\n", sqlite3_column_text(stmt, 3) ? (char*)sqlite3_column_text(stmt, 3) : "N/A");
        printf("Telefono: %s\n", sqlite3_column_text(stmt, 4) ? (char*)sqlite3_column_text(stmt, 4) : "N/A");
    } else {
        printf("No se encontraron datos personales para este usuario.\n");
    }
    
    sqlite3_finalize(stmt);
}

void consultarDatosVehiculos() {
    printf("Consultando datos de los vehiculos...\n");
}

void modificarInformacionVehiculo() {
    printf("Modificando informacion del vehiculo...\n");
}

void agregarNuevoVehiculo() {
    printf("Anadiendo nuevo vehiculo...\n");
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
    printf("Consultando todos los vehiculos...\n");
}

void obtenerDatosPersonales(char *usuario) {
    // esta funcion ya no es necesaria ya que su funcionalidad esta incorporada en consultarDatosUsuario
    // se mantiene la declaración para evitar errores de compilación
}