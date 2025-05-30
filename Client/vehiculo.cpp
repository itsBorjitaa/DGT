#include <iostream>
#include <string.h>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <ctime>
#include "vehiculo.h"


// variable global para guardar el usuario actual
extern char usuarioActual[50];


Vehiculo::Vehiculo()
{
	this->matricula = new char[1];
	this->matricula[0] = '\0';
	this->marca = new char[1];
	this->marca[0] = '\0';
	this->modelo = new char[1];
	this->modelo[0] = '\0';
	this->anio = 0;
	this->color = new char[1];
	this->color[0] = '\0';
    this->tipo_vehiculo = new char[1];
	this->tipo_vehiculo[0] = '\0';
}
Vehiculo::Vehiculo(const char* matricula, const char* marca, const char* modelo, int anio, const char* color, const char* tipo_vehiculo)
{
	this->matricula = new char[strlen(matricula) + 1];
	strcpy(this->matricula, matricula);
    this->marca = new char[strlen(marca) + 1];
	strcpy(this->marca, marca);
    this->modelo = new char[strlen(modelo) + 1];
	strcpy(this->modelo, modelo);
    this->anio = anio;
    this->color = new char[strlen(color) + 1];
	strcpy(this->color, color);
    this->tipo_vehiculo = new char[strlen(tipo_vehiculo) + 1];
	strcpy(this->tipo_vehiculo, tipo_vehiculo);
}
Vehiculo::~Vehiculo()
{
    delete[] this->matricula;
    delete[] this->marca;
    delete[] this->modelo;
    delete[] this->color;
    delete[] this->tipo_vehiculo;
}
void Vehiculo::setMatricula(const char* matricula)
{
	delete[] this->matricula;
	this->matricula = new char[strlen(matricula) + 1];
	strcpy(this->matricula, matricula);
}
char* Vehiculo::getMatricula() const
{
	return this->matricula;
}
void Vehiculo::setMarca(const char* marca)
{
	delete[] this->marca;
	this->marca = new char[strlen(marca) + 1];
	strcpy(this->marca, marca);
}
char* Vehiculo::getMarca() const
{
	return this->marca;
}
void Vehiculo::setModelo(const char* modelo)
{
	delete[] this->modelo;
	this->modelo = new char[strlen(modelo) + 1];
	strcpy(this->modelo, modelo);
}
char* Vehiculo::getModelo() const
{
	return this->modelo;
}
void Vehiculo::setAnio(int anio)
{
	this->anio = anio;
}
int Vehiculo::getAnio() const
{
	return this->anio;
}
void Vehiculo::setColor(const char* color)
{
	delete[] this->color;
	this->color = new char[strlen(color) + 1];
	strcpy(this->color, color);
}
char* Vehiculo::getColor() const
{
	return this->color;
}
void Vehiculo::setTipo_vehiculo(const char* tipo_vehiculo)
{
	delete[] this->tipo_vehiculo;
	this->tipo_vehiculo = new char[strlen(tipo_vehiculo) + 1];
	strcpy(this->tipo_vehiculo, tipo_vehiculo);
}
char* Vehiculo::getTipo_vehiculo() const
{
	return this->tipo_vehiculo;
}