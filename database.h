#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"

void inicializarDB();
int verificarCredenciales(char *usuario, char *contrasena, char *rol);
int registrarUsuario(char *usuario, char *contrasena, char *nombre, char *apellidos, char *dni, char *email, char *telefono);
int existeUsuario(char *usuario);
int agregarMulta(char *dni, char *concepto, char *fecha_delito, double importe, char *fecha_limite_descuento);
char* obtenerDNI(char *usuario, char *dni_buffer);
int pagarMulta(int id_multa, char *fecha_pago);
double calcularImporte(int id_multa, char *fecha_actual);

#endif