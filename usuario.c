#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "database.h"
#include <time.h>

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

void consultarMultasAdmin() {
    char sql[] = "SELECT id, dni, concepto, fecha_delito, importe, fecha_limite_descuento, pagada, fecha_pago FROM multas";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al consultar multas.\n");
        return;
    }
    
    printf("\n--- Lista de Multas ---\n");
    printf("%-4s %-15s %-25s %-12s %-10s %-15s %-7s %-12s\n", 
           "ID", "DNI", "Concepto", "Fecha Delito", "Importe", "Fecha Lim Desc", "Pagada", "Fecha Pago");
    printf("------------------------------------------------------------------------------------------------\n");
    
    int encontrados = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrados = 1;
        int pagada = sqlite3_column_int(stmt, 6);
        printf("%-4d %-15s %-25s %-12s %-10.2f %-15s %-7s %-12s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1) ? (char*)sqlite3_column_text(stmt, 1) : "N/A",
               sqlite3_column_text(stmt, 2) ? (char*)sqlite3_column_text(stmt, 2) : "N/A",
               sqlite3_column_text(stmt, 3) ? (char*)sqlite3_column_text(stmt, 3) : "N/A",
               sqlite3_column_double(stmt, 4),
               sqlite3_column_text(stmt, 5) ? (char*)sqlite3_column_text(stmt, 5) : "N/A",
               pagada ? "Si" : "No",
               sqlite3_column_text(stmt, 7) ? (char*)sqlite3_column_text(stmt, 7) : "N/A");
    }
    
    if (!encontrados) {
        printf("No se encontraron multas registradas.\n");
    }
    
    sqlite3_finalize(stmt);
}

void agregarMultaAdmin() {
    char dni[20];
    char concepto[100];
    char fecha_delito[15];
    double importe;
    char fecha_limite_descuento[15];
    
    printf("\n--- Agregar Nueva Multa ---\n");
    
    printf("Ingrese el DNI del infractor: ");
    scanf("%s", dni);
    getchar(); // limpiar buffer
    
    // verifica si el DNI existe
    char sql_check[200];
    sprintf(sql_check, "SELECT 1 FROM usuarios WHERE dni='%s'", dni);
    
    sqlite3_stmt *stmt_check;
    int dni_existe = 0;
    
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt_check) == SQLITE_ROW) {
            dni_existe = 1;
        }
        sqlite3_finalize(stmt_check);
    }
    
    if (!dni_existe) {
        printf("El DNI no esta registrado en el sistema.\n");
        return;
    }
    
    printf("Concepto de la multa: ");
    fgets(concepto, 100, stdin);
    concepto[strcspn(concepto, "\n")] = 0;
    
    printf("Fecha del delito (YYYY-MM-DD): ");
    scanf("%s", fecha_delito);
    
    printf("Importe (EUR): ");
    scanf("%lf", &importe);
    
    printf("Fecha limite para descuento (YYYY-MM-DD): ");
    scanf("%s", fecha_limite_descuento);
    
    if (agregarMulta(dni, concepto, fecha_delito, importe, fecha_limite_descuento)) {
        printf("Multa registrada correctamente.\n");
    } else {
        printf("Error al registrar la multa.\n");
    }
}

void cambiarEstadoMulta() {
    int id_multa;
    int nueva_pagada;
    char fecha_pago[15];
    
    printf("\n--- Cambiar Estado de Multa ---\n");
    
    printf("ID de la multa: ");
    scanf("%d", &id_multa);
    
    // Verificar si la multa existe
    char sql_check[200];
    sprintf(sql_check, "SELECT pagada FROM multas WHERE id=%d", id_multa);
    
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
        printf("La multa con ID %d no existe.\n", id_multa);
        return;
    }
    
    if (ya_pagada) {
        printf("La multa ya esta marcada como pagada.\n");
        return;
    }
    
    printf("Fecha de pago (YYYY-MM-DD): ");
    scanf("%s", fecha_pago);
    
    if (pagarMulta(id_multa, fecha_pago)) {
        printf("Estado de la multa actualizado correctamente.\n");
    } else {
        printf("Error al actualizar el estado de la multa.\n");
    }
}

void consultarMultasUsuario() {
    char dni_buffer[20];
    char *dni = obtenerDNI(usuarioActual, dni_buffer);
    
    if (dni == NULL) {
        printf("Error al obtener el DNI del usuario.\n");
        return;
    }
    
    char sql[200];
    sprintf(sql, "SELECT id, concepto, fecha_delito, importe, fecha_limite_descuento, pagada, fecha_pago FROM multas WHERE dni='%s'", dni);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al consultar multas.\n");
        return;
    }
    
    printf("\n--- Mis Multas ---\n");
    printf("%-4s %-25s %-12s %-10s %-15s %-7s %-12s\n", 
           "ID", "Concepto", "Fecha Delito", "Importe", "Fecha Lim Desc", "Pagada", "Fecha Pago");
    printf("--------------------------------------------------------------------------------------\n");
    
    int encontrados = 0;
    char fecha_actual[15];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(fecha_actual, "%04d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrados = 1;
        int id_multa = sqlite3_column_int(stmt, 0);
        double importe_original = sqlite3_column_double(stmt, 3);
        int pagada = sqlite3_column_int(stmt, 5);
        const char *fecha_limite = (char*)sqlite3_column_text(stmt, 4);
        
        double importe_actual = importe_original;
        char info_descuento[50] = "";
        
        // Si no está pagada y estamos dentro del plazo de descuento
        if (!pagada && strcmp(fecha_actual, fecha_limite) <= 0) {
            importe_actual = importe_original * 0.5;
            sprintf(info_descuento, " (%.2f con desc.)", importe_actual);
        }
        
        printf("%-4d %-25s %-12s %-10.2f%s %-15s %-7s %-12s\n",
               id_multa,
               sqlite3_column_text(stmt, 1) ? (char*)sqlite3_column_text(stmt, 1) : "N/A",
               sqlite3_column_text(stmt, 2) ? (char*)sqlite3_column_text(stmt, 2) : "N/A",
               importe_original,
               info_descuento,
               fecha_limite ? fecha_limite : "N/A",
               pagada ? "Si" : "No",
               sqlite3_column_text(stmt, 6) ? (char*)sqlite3_column_text(stmt, 6) : "N/A");
    }
    
    if (!encontrados) {
        printf("No tienes multas registradas.\n");
    }
    
    sqlite3_finalize(stmt);
}

void pagarMultaUsuario() {
    int id_multa;
    char dni_buffer[20];
    char *dni = obtenerDNI(usuarioActual, dni_buffer);
    
    if (dni == NULL) {
        printf("Error al obtener el DNI del usuario.\n");
        return;
    }
    
    printf("\n--- Pagar Multa ---\n");
    
    printf("ID de la multa a pagar: ");
    scanf("%d", &id_multa);
    
    // Verificar si la multa existe y pertenece al usuario
    char sql_check[200];
    sprintf(sql_check, "SELECT pagada, importe, fecha_limite_descuento FROM multas WHERE id=%d AND dni='%s'", id_multa, dni);
    
    sqlite3_stmt *stmt_check;
    int multa_existe = 0;
    int ya_pagada = 0;
    double importe = 0.0;
    char fecha_limite[15] = "";
    
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt_check) == SQLITE_ROW) {
            multa_existe = 1;
            ya_pagada = sqlite3_column_int(stmt_check, 0);
            importe = sqlite3_column_double(stmt_check, 1);
            if (sqlite3_column_text(stmt_check, 2))
                strcpy(fecha_limite, (char*)sqlite3_column_text(stmt_check, 2));
        }
        sqlite3_finalize(stmt_check);
    }
    
    if (!multa_existe) {
        printf("La multa con ID %d no existe o no te pertenece.\n", id_multa);
        return;
    }
    
    if (ya_pagada) {
        printf("Esta multa ya está pagada.\n");
        return;
    }
    
    // obtiene fecha actual para calcular descuento
    char fecha_actual[15];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(fecha_actual, "%04d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    
    // calcula el importe con posible descuento teniendo en cuenta la fecha
    double importe_a_pagar = importe;
    if (strcmp(fecha_actual, fecha_limite) <= 0) {
        importe_a_pagar = importe * 0.5;
        printf("Buenas noticias! Tienes un descuento del 50%% por pronto pago.\n");
    }
    
    printf("Importe a pagar: %.2f EUR\n", importe_a_pagar);
    printf("Desea proceder con el pago? (1: Si / 0: No): ");
    
    int confirmar;
    scanf("%d", &confirmar);
    
    if (confirmar) {
        if (pagarMulta(id_multa, fecha_actual)) {
            printf("Multa pagada correctamente. Se ha registrado el pago con fecha %s.\n", fecha_actual);
        } else {
            printf("Error al procesar el pago de la multa.\n");
        }
    } else {
        printf("Operacion cancelada.\n");
    }
}