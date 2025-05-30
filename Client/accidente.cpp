#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include "usuario.h"
#include "accidente.h"

// variable global para guardar el usuario actual
extern char usuarioActual[50];
// declaración externa de la base de datos
extern sqlite3 *db;

Accidente::Accidente()
{
	this->usuario = new char[1];
	this->usuario[0] = '\0';
    this->fecha = new char[1];
	this->fecha[0] = '\0';
    this->descripcion = new char[1];
	this->descripcion[0] = '\0';
}
Accidente::Accidente(const char* usuario, const char* fecha, const char* descripcion)
{
	this->usuario = new char[strlen(usuario) + 1];
	strcpy(this->usuario, usuario);
    this->fecha = new char[strlen(fecha) + 1];
	strcpy(this->fecha, fecha);
    this->descripcion = new char[strlen(descripcion) + 1];
	strcpy(this->descripcion, descripcion);
}
Accidente::~Accidente()
{
    delete[] this->usuario;
	delete[] this->fecha;
    delete[] this->descripcion;
}
char* Accidente::getUsuario() const
{
	return this->usuario;
}
void Accidente::setUsuario(const char* usuario)
{
	delete[] this->usuario;
	this->usuario = new char[strlen(usuario) + 1];
	strcpy(this->usuario, usuario);
}
char* Accidente::getFecha() const
{
	return this->fecha;
}
void Accidente::setFecha(const char* fecha)
{
	delete[] this->fecha;
	this->fecha = new char[strlen(fecha) + 1];
	strcpy(this->fecha, fecha);
}
char* Accidente::getDescripcion() const
{
	return this->descripcion;
}
void Accidente::setDescripcion(const char* descripcion)
{
	delete[] this->descripcion;
	this->descripcion = new char[strlen(descripcion) + 1];
	strcpy(this->descripcion, descripcion);
}


int obtenerAnioActual() {
    time_t t = time(nullptr);
    struct tm tm = *localtime(&t);
    return tm.tm_year + 1900;
}

bool esAnioBisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

bool validarFecha(const std::string& fecha) {
    int anio, mes, dia;

    if (sscanf(fecha.c_str(), "%4d-%2d-%2d", &anio, &mes, &dia) != 3) {
        return false; 
    }

    int anioActual = obtenerAnioActual();

    // Año de 4 dígitos y dentro de un rango válido
    if (anio < 1900 || anio > anioActual) {
        return false;
    }

    // Mes válido (01-12)
    if (mes < 1 || mes > 12) {
        return false;
    }

    // Días máximos del mes
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Si el año es bisiesto, febrero tiene 29 días
    if (mes == 2 && esAnioBisiesto(anio)) {
        diasPorMes[1] = 29;
    }

    // Verifica que el día sea válido según el mes
    if (dia < 1 || dia > diasPorMes[mes - 1]) {
        return false;
    }
    return true; 
}
