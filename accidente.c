#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "database.h"
#include <time.h>
#include "logger.h"
#include <stdlib.h>


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

int esAnioBisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

int validarFecha(const char *fecha) {
    int anio, mes, dia;

    if (sscanf(fecha, "%4d-%2d-%2d", &anio, &mes, &dia) != 3) {
        return 0; 
    }

    // Año de 4 dígitos y dentro de un rango válido
    if (anio < 1900 || anio > 2100) {
        return 0;
    }

    // Mes válido (01-12)
    if (mes < 1 || mes > 12) {
        return 0;
    }

    // Días máximos del mes
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Si el año es bisiesto, febrero tiene 29 días
    if (mes == 2 && esAnioBisiesto(anio)) {
        diasPorMes[1] = 29;
    }

    // Verifica que el día sea válido según el mes
    if (dia < 1 || dia > diasPorMes[mes - 1]) {
        return 0;
    }
    return 1; 
}

void registrarAccidenteConValidacion() {
    char fecha[20], descripcion[200];

    do {
        printf("Fecha del accidente (YYYY-MM-DD): ");
        scanf("%19s", fecha);
        getchar();

        if (!validarFecha(fecha)) {
            printf("Error: Fecha invalida. Debe estar en formato YYYY-MM-DD con valores correctos.\n");
        }
    } while (!validarFecha(fecha));

    printf("Descripcion del accidente: ");
    fgets(descripcion, sizeof(descripcion), stdin);
    descripcion[strcspn(descripcion, "\n")] = 0;

    registrarAccidente(usuarioActual, fecha, descripcion);
    registrarAccion(usuarioActual, "Usuario registró un nuevo accidente");
}
