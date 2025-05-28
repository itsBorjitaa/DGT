#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "database.h"
#include <time.h>
#include "logger.h"

// variable global para guardar el usuario actual
extern char usuarioActual[50];
// declaración externa de la base de datos
extern sqlite3 *db;

void consultarDatosUsuario() {
    printf("Consultando datos del usuario...\n");
    registrarAccion(usuarioActual, "Consulto sus datos personales");
    
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
    registrarAccion(usuarioActual, "Consulto la lista de todos los usuarios");
    
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

    registrarAccion(usuarioActual, "Exportó sus datos a un archivo TXT");

    // Datos personales
    char sql[200];
    sprintf(sql, "SELECT nombre, apellidos, dni, email, telefono FROM usuarios WHERE usuario = ?");

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        printf("Error preparando consulta de datos personales: %s\n", sqlite3_errmsg(db));
        fclose(archivo);
        return;
    }

    sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);

    char dniUsuario[20] = "";
    if (rc == SQLITE_ROW) {
        strcpy(dniUsuario, sqlite3_column_text(stmt, 2));
        fprintf(archivo, "--- Datos Personales ---\n");
        fprintf(archivo, "Nombre: %s\n", sqlite3_column_text(stmt, 0));
        fprintf(archivo, "Apellidos: %s\n", sqlite3_column_text(stmt, 1));
        fprintf(archivo, "DNI: %s\n", dniUsuario);
        fprintf(archivo, "Email: %s\n", sqlite3_column_text(stmt, 3));
        fprintf(archivo, "Telefono: %s\n\n", sqlite3_column_text(stmt, 4));
    } else {
        printf("No se encontraron datos personales para este usuario.\n");
    }
    sqlite3_finalize(stmt);

    // Vehículos
    fprintf(archivo, "--- Vehículos Registrados ---\n");
    sprintf(sql, "SELECT matricula, marca, modelo, anio, color, tipo_vehiculo FROM vehiculos WHERE usuario = ?");
    
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);

    int vehiculosEncontrados = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        vehiculosEncontrados = 1;
        fprintf(archivo, "Matrícula: %s\n", sqlite3_column_text(stmt, 0));
        fprintf(archivo, "Marca: %s\n", sqlite3_column_text(stmt, 1));
        fprintf(archivo, "Modelo: %s\n", sqlite3_column_text(stmt, 2));
        fprintf(archivo, "Año: %d\n", sqlite3_column_int(stmt, 3));
        fprintf(archivo, "Color: %s\n", sqlite3_column_text(stmt, 4));
        fprintf(archivo, "Tipo de Vehículo: %s\n\n", sqlite3_column_text(stmt, 5));
    }
    sqlite3_finalize(stmt);
    if (!vehiculosEncontrados) {
        fprintf(archivo, "No tiene vehículos registrados.\n\n");
    }

    // Multas
    fprintf(archivo, "--- Multas Asociadas ---\n");
    sprintf(sql, "SELECT concepto, fecha_delito, importe, fecha_limite_descuento, pagada, fecha_pago "
                 "FROM multas WHERE dni = ?");
    
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, dniUsuario, -1, SQLITE_STATIC);

    int multasEncontradas = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        multasEncontradas = 1;
        fprintf(archivo, "Concepto: %s\n", sqlite3_column_text(stmt, 0));
        fprintf(archivo, "Fecha del Delito: %s\n", sqlite3_column_text(stmt, 1));
        fprintf(archivo, "Importe: %.2f€\n", sqlite3_column_double(stmt, 2));
        fprintf(archivo, "Fecha Límite Descuento: %s\n", sqlite3_column_text(stmt, 3));
        fprintf(archivo, "Pagada: %s\n", sqlite3_column_int(stmt, 4) ? "Sí" : "No");
        const char *fechaPago = (const char *)sqlite3_column_text(stmt, 5);
        fprintf(archivo, "Fecha de Pago: %s\n\n", fechaPago ? fechaPago : "No aplicable");
    }
    sqlite3_finalize(stmt);
    if (!multasEncontradas) {
        fprintf(archivo, "No tiene multas registradas.\n\n");
    }

    // Accidentes
    fprintf(archivo, "--- Historial de Accidentes ---\n");
    sprintf(sql, "SELECT id, fecha, descripcion FROM accidentes WHERE usuario = ?");
    
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);

    int accidentesEncontrados = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        accidentesEncontrados = 1;
        fprintf(archivo, "ID: %d\n", sqlite3_column_int(stmt, 0));
        fprintf(archivo, "Fecha: %s\n", sqlite3_column_text(stmt, 1));
        fprintf(archivo, "Descripción: %s\n\n", sqlite3_column_text(stmt, 2));
    }
    sqlite3_finalize(stmt);
    if (!accidentesEncontrados) {
        fprintf(archivo, "No tiene accidentes registrados.\n\n");
    }

    fclose(archivo);
    printf("Datos exportados correctamente a %s\n", nombreArchivo);
}
