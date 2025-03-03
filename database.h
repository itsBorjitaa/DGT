#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"

void inicializarDB();
int verificarCredenciales(char *usuario, char *contrasena, char *rol);
int registrarUsuario(char *usuario, char *contrasena, char *nombre, char *apellidos, char *dni, char *email, char *telefono);
int existeUsuario(char *usuario);

#endif