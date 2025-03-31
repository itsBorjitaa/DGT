#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "database.h"
#include <time.h>

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

void guardarDatosUsuarioEnTXT() {
    char nombreArchivo[100];
    printf("Introduzca el nombre con el que desea guardar el archivo (sin extension): ");
    scanf("%99s", nombreArchivo);
    strcat(nombreArchivo, ".txt");

    FILE *archivo = fopen(nombreArchivo, "w");
    if (archivo == NULL) {
        printf("Error al crear el archivo.\n");
        return;
    }

    char sql[200];
    sprintf(sql, "SELECT nombre, apellidos, dni, email, telefono FROM usuarios WHERE usuario = ?");
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        printf("Error preparando consulta de datos personales: %s\n", sqlite3_errmsg(db));
        fclose(archivo);
        return;
    }
    
    rc = sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        printf("Error vinculando parametro de usuario: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        fclose(archivo);
        return;
    }
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        fprintf(archivo, "--- Datos Personales ---\n");
        fprintf(archivo, "Nombre: %s\n", sqlite3_column_text(stmt, 0) ? (char*)sqlite3_column_text(stmt, 0) : "N/A");
        fprintf(archivo, "Apellidos: %s\n", sqlite3_column_text(stmt, 1) ? (char*)sqlite3_column_text(stmt, 1) : "N/A");
        fprintf(archivo, "DNI: %s\n", sqlite3_column_text(stmt, 2) ? (char*)sqlite3_column_text(stmt, 2) : "N/A");
        fprintf(archivo, "Email: %s\n", sqlite3_column_text(stmt, 3) ? (char*)sqlite3_column_text(stmt, 3) : "N/A");
        fprintf(archivo, "Telefono: %s\n", sqlite3_column_text(stmt, 4) ? (char*)sqlite3_column_text(stmt, 4) : "N/A");
        printf("Datos guardados en %s\n", nombreArchivo);
    } else {
        printf("No se encontraron datos personales para este usuario.\n");
    }
    
    sqlite3_finalize(stmt);
    fclose(archivo);
}