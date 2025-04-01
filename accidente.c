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

void consultarDatosAccidente(int idAccidente) {
    printf("Consultando datos del accidente con ID %d...\n", idAccidente);
    registrarAccion(usuarioActual, "Consulto un accidente específico");

    char sql[200];
    sprintf(sql, "SELECT fecha, descripcion FROM accidentes WHERE id = ?");

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        printf("Error preparando consulta de accidente: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, idAccidente);
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        printf("\n--- Datos del Accidente ---\n");
        printf("Fecha: %s\n", sqlite3_column_text(stmt, 0));
        printf("Descripcion: %s\n", sqlite3_column_text(stmt, 1));
    } else {
        printf("No se encontro el accidente con ID %d.\n", idAccidente);
    }

    sqlite3_finalize(stmt);
}

void consultarAccidentesUsuario(char *usuario) {
    if (usuario == NULL) {
        printf("Error: Usuario inválido.\n");
        return;
    }
    
    registrarAccion(usuario, "Consultó su historial de accidentes");

    char sql[300];
    sprintf(sql, "SELECT id, fecha, descripcion FROM accidentes WHERE usuario='%s'", usuario);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al consultar accidentes.\n");
        return;
    }

    printf("\n--- Historial de Accidentes ---\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *fecha = (const char *)sqlite3_column_text(stmt, 1);
        const char *descripcion = (const char *)sqlite3_column_text(stmt, 2);
        printf("ID: %d | Fecha: %s | Descripcion: %s\n", id, fecha, descripcion);
    }

    sqlite3_finalize(stmt);
}