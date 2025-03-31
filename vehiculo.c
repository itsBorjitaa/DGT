#include <stdio.h>
#include <string.h>
#include "database.h"
#include "vehiculo.h"
#include "logger.h"
#include <ctype.h>
#include <time.h>

#define MAX_INPUT 100
// variable global para guardar el usuario actual
extern char usuarioActual[50];
// declaración externa de la base de datos
extern sqlite3 *db;

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

    registrarAccion(usuarioActual, "Consultó sus datos de vehículos.");
}


void modificarInformacionVehiculo() {
    printf("Modificando informacion del vehiculo...\n");
    registrarAccion(usuarioActual, "Modificó la informacion de un vehículo.");
}

int validarMatricula(const char *matricula) {
    if (strlen(matricula) != 7) return 0; // Debe tener exactamente 7 caracteres
    for (int i = 0; i < 4; i++) {
        if (!isdigit(matricula[i])) return 0; // Primeros 4 caracteres deben ser dígitos
    }
    for (int i = 4; i < 7; i++) {
        if (!isalpha(matricula[i]) || !isupper(matricula[i])) return 0; // Últimos 3 deben ser letras mayúsculas
    }
    return 1;
}

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void agregarNuevoVehiculo() {
    char matricula[MAX_INPUT];
    char marca[MAX_INPUT];
    char modelo[MAX_INPUT];
    char color[MAX_INPUT];
    char tipo_vehiculo[MAX_INPUT];
    int anio;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int anio_actual = tm.tm_year + 1900; // Año actual

    printf("\n--- Agregar Nuevo Vehiculo ---\n");

    // Verifica si el usuario ya tiene 3 vehículos
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

    // Validar matrícula
    do {
        printf("Matricula (formato 1234ABC): ");
        scanf("%s", matricula);
        if (!validarMatricula(matricula)) {
            printf("Error: La matricula debe tener 4 numeros y 3 letras mayusculas.\n");
        }
    } while (!validarMatricula(matricula));

    // Verifica si la matrícula ya existe
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
        printf("Error: La matricula ya existe en el sistema.\n");
        return;
    }

    getchar();
    
    // Validar marca
    do {
        printf("Marca: ");
        fgets(marca, MAX_INPUT, stdin);
        marca[strcspn(marca, "\n")] = 0;
        if (strlen(marca) == 0) {
            printf("Error: La marca no puede estar vacia.\n");
        }
    } while (strlen(marca) == 0);

    // Validar modelo
    do {
        printf("Modelo: ");
        fgets(modelo, MAX_INPUT, stdin);
        modelo[strcspn(modelo, "\n")] = 0;
        if (strlen(modelo) == 0) {
            printf("Error: El modelo no puede estar vacio.\n");
        }
    } while (strlen(modelo) == 0);

    // Validar año
    do {
        printf("Anio de fabricacion (entre 1900 y %d): ", anio_actual);
        scanf("%d", &anio);
        if (anio < 1900 || anio > anio_actual) {
            printf("Error: El anio debe estar entre 1900 y %d.\n", anio_actual);
        }
    } while (anio < 1900 || anio > anio_actual);

    getchar();

    // Validar color
    do {
        printf("Color: ");
        fgets(color, MAX_INPUT, stdin);
        color[strcspn(color, "\n")] = 0;
        if (strlen(color) == 0) {
            printf("Error: El color no puede estar vacio.\n");
        }
    } while (strlen(color) == 0);

    // Validar tipo de vehículo
    do {
        printf("Tipo de vehiculo (coche/moto/camion): ");
        fgets(tipo_vehiculo, MAX_INPUT, stdin);
        tipo_vehiculo[strcspn(tipo_vehiculo, "\n")] = 0;
    
        toLowerCase(tipo_vehiculo); // Convertir a minúsculas
    
        if (strcmp(tipo_vehiculo, "coche") != 0 && 
            strcmp(tipo_vehiculo, "moto") != 0 && 
            strcmp(tipo_vehiculo, "camion") != 0) {
            printf("Error: El tipo de vehículo debe ser 'coche', 'moto' o 'camion'.\n");
        }
    } while (strcmp(tipo_vehiculo, "coche") != 0 &&
             strcmp(tipo_vehiculo, "moto") != 0 &&
             strcmp(tipo_vehiculo, "camion") != 0);

    // Guardar en la base de datos
    char sql[500];
    char *errMsg = 0;
    
    sprintf(sql, "INSERT INTO vehiculos (usuario, matricula, marca, modelo, anio, color, tipo_vehiculo) "
                 "VALUES ('%s', '%s', '%s', '%s', %d, '%s', '%s');", 
            usuarioActual, matricula, marca, modelo, anio, color, tipo_vehiculo);
    
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("Error al registrar vehculo: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Vehiculo registrado correctamente.\n");

        char mensaje_log[200];
        sprintf(mensaje_log, "Agregó un nuevo vehículo con matrícula %s", matricula);
        registrarAccion(usuarioActual, mensaje_log);
    }
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