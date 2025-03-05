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