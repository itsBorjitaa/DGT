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
