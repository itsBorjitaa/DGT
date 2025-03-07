#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

sqlite3 *db;

void inicializarDB() {
    int rc_open = sqlite3_open("db.db", &db);
    if (rc_open) {
        printf("Error al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    // crear tabla de usuarios
    char *sql = "CREATE TABLE IF NOT EXISTS usuarios ("
                "usuario TEXT PRIMARY KEY, "
                "contrasena TEXT, "
                "rol TEXT, "
                "nombre TEXT, "
                "apellidos TEXT, "
                "dni TEXT, "
                "email TEXT, "
                "telefono TEXT"
                ");";
    
    char *errMsg_usuarios = 0;
    int rc_usuarios = sqlite3_exec(db, sql, 0, 0, &errMsg_usuarios);
    if (rc_usuarios != SQLITE_OK) {
        printf("Error SQL: %s\n", errMsg_usuarios);
        sqlite3_free(errMsg_usuarios);
    }
    
    // crear tabla de vehiculos
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

    char *errMsg_vehiculos = 0;
    int rc_vehiculos = sqlite3_exec(db, sql_vehiculos, 0, 0, &errMsg_vehiculos);
    if (rc_vehiculos != SQLITE_OK) {
        printf("Error creando tabla de vehiculos: %s\n", errMsg_vehiculos);
        sqlite3_free(errMsg_vehiculos);
    }

    // crear tabla de multas
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

    char *errMsg_multas = 0;
    int rc_multas = sqlite3_exec(db, sql_multas, 0, 0, &errMsg_multas);
    if (rc_multas != SQLITE_OK) {
        printf("Error creando tabla de multas: %s\n", errMsg_multas);
        sqlite3_free(errMsg_multas);
    }

    // añade el admin
    char *sql_admin = "INSERT OR IGNORE INTO usuarios (usuario, contrasena, rol) VALUES ('admin', 'admin123', 'admin');";
    char *errMsg_admin = 0;
    int rc_admin = sqlite3_exec(db, sql_admin, 0, 0, &errMsg_admin);
    if (rc_admin != SQLITE_OK) {
        printf("Error al insertar admin: %s\n", errMsg_admin);
        sqlite3_free(errMsg_admin);
    }
}

int verificarCredenciales(char *usuario, char *contrasena, char *rol) {
    char sql[200];
    sprintf(sql, "SELECT rol FROM usuarios WHERE usuario='%s' AND contrasena='%s'", usuario, contrasena);
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al verificar credenciales.\n");
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

int existeUsuario(char *usuario) {
    char sql[200];
    sprintf(sql, "SELECT 1 FROM usuarios WHERE usuario='%s'", usuario);
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al verificar existencia de usuario.\n");
        return 0;
    }

    int existe = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        existe = 1;
    }

    sqlite3_finalize(stmt);
    return existe;
}

int registrarUsuario(char *usuario, char *contrasena, char *nombre, char *apellidos, char *dni, char *email, char *telefono) {
    if (existeUsuario(usuario)) {
        printf("El usuario ya existe.\n");
        return 0;
    }

    char *errMsg = 0;
    char sql[500];
    
    sprintf(sql, "INSERT INTO usuarios (usuario, contrasena, rol, nombre, apellidos, dni, email, telefono) "
                 "VALUES ('%s', '%s', 'usuario', '%s', '%s', '%s', '%s', '%s');", 
            usuario, contrasena, nombre, apellidos, dni, email, telefono);
    
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("Error al registrar usuario: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 0;
    }
    
    return 1;
}

// funcion para añadir una multa (administradores)
int agregarMulta(char *dni, char *concepto, char *fecha_delito, double importe, char *fecha_limite_descuento) {
    char *errMsg = 0;
    char sql[500];
    
    sprintf(sql, "INSERT INTO multas (dni, concepto, fecha_delito, importe, fecha_limite_descuento, pagada) "
                 "VALUES ('%s', '%s', '%s', %.2f, '%s', 0);", 
            dni, concepto, fecha_delito, importe, fecha_limite_descuento);
    
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("Error al registrar multa: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 0;
    }
    
    return 1;
}

// obtener el DNI del usuario a partir de su nombre de usuario
char* obtenerDNI(char *usuario, char *dni_buffer) {
    char sql[200];
    sprintf(sql, "SELECT dni FROM usuarios WHERE usuario='%s'", usuario);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al consultar DNI.\n");
        return NULL;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(dni_buffer, (char*)sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return dni_buffer;
    }
    
    sqlite3_finalize(stmt);
    return NULL;
}

// funcion para marcar multa como pagada
int pagarMulta(int id_multa, char *fecha_pago) {
    char *errMsg = 0;
    char sql[200];
    
    sprintf(sql, "UPDATE multas SET pagada = 1, fecha_pago = '%s' WHERE id = %d;", 
            fecha_pago, id_multa);
    
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("Error al pagar multa: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 0;
    }
    
    return 1;
}

// funcion para obtener el importe a pagar (teniendo en cuenta descuento)
double calcularImporte(int id_multa, char *fecha_actual) {
    char sql[200];
    sprintf(sql, "SELECT importe, fecha_limite_descuento FROM multas WHERE id = %d", id_multa);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        printf("Error al calcular importe.\n");
        return 0.0;
    }
    
    double importe = 0.0;
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        double importe_original = sqlite3_column_double(stmt, 0);
        const char *fecha_limite = (char*)sqlite3_column_text(stmt, 1);
        
        // comparación de fechas
        if (strcmp(fecha_actual, fecha_limite) <= 0) {
            // descuento del 50%
            importe = importe_original * 0.5;
        } else {
            importe = importe_original;
        }
    }
    
    sqlite3_finalize(stmt);
    return importe;
}