#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include "multa.h"

// variable global para guardar el usuario actual
extern char usuarioActual[50];

Multa::Multa()
{
	this->id = new char[1];
	this->id[0] = '\0';
    this->dni = new char[1];
	this->dni[0] = '\0';
    this->concepto = new char[1];
	this->concepto[0] = '\0';
    this->fecha_delito = new char[1];
	this->fecha_delito[0] = '\0';
    this->importe = 0;
    this->fecha_delito_desc = new char[1];
	this->fecha_delito_desc[0] = '\0';
    this->pagada = 0;
    this->fecha_pago = new char[1];
	this->fecha_pago[0] = '\0';
}
Multa::Multa(const char* id, const char* dni, const char* concepto, const char* fecha_delito, double importe, const char* fecha_delito_desc, int pagada, const char* fecha_pago)
{
	this->id = new char[strlen(id) + 1];
	strcpy(this->id, id);
    this->dni = new char[strlen(dni) + 1];
	strcpy(this->dni, dni);
    this->concepto = new char[strlen(concepto) + 1];
	strcpy(this->concepto, concepto);
    this->fecha_delito = new char[strlen(fecha_delito) + 1];
	strcpy(this->fecha_delito, fecha_delito);
	this->importe = importe;
    this->fecha_delito_desc = new char[strlen(fecha_delito_desc) + 1];
	strcpy(this->fecha_delito_desc, fecha_delito_desc);
    this->pagada = pagada;
    this->fecha_pago = new char[strlen(fecha_pago) + 1];
	strcpy(this->fecha_pago, fecha_pago);
}
Multa::~Multa()
{
    delete[] this->id;
	delete[] this->dni;
    delete[] this->concepto;
    delete[] this->fecha_delito;
    delete[] this->fecha_delito_desc;
    delete[] this->fecha_pago;
}
char* Multa::getId() const
{
	return this->id;
}
void Multa::setId(const char* id)
{
	delete[] this->id;
	this->id = new char[strlen(id) + 1];
	strcpy(this->id, id);
}
char* Multa::getDni() const
{
	return this->dni;
}
void Multa::setDni(const char* dni)
{
	delete[] this->dni;
	this->dni = new char[strlen(dni) + 1];
	strcpy(this->dni, dni);
}
char* Multa::getConcepto() const
{
	return this->concepto;
}
void Multa::setConcepto(const char* concepto)
{
	delete[] this->concepto;
	this->concepto = new char[strlen(concepto) + 1];
	strcpy(this->concepto, concepto);
}
char* Multa::getFecha_delito() const
{
	return this->fecha_delito;
}
void Multa::setFecha_delito(const char* fecha_delito)
{
	delete[] this->fecha_delito;
	this->fecha_delito = new char[strlen(fecha_delito) + 1];
	strcpy(this->fecha_delito, fecha_delito);
}
char* Multa::getFecha_pago() const
{
	return this->fecha_pago;
}
void Multa::setFecha_pago(const char* fecha_pago)
{
	delete[] this->fecha_pago;
	this->fecha_pago = new char[strlen(fecha_pago) + 1];
	strcpy(this->fecha_pago, fecha_pago);
}
double Multa::getImporte() const
{
	return this->importe;
}
void Multa::setImporte(double importe)
{
	this->importe = importe;
}
char* Multa::getFecha_delito_desc() const
{
	return this->fecha_delito_desc;
}
void Multa::setFecha_delito_desc(const char* fecha_delito_desc)
{
	delete[] this->fecha_delito_desc;
	this->fecha_delito_desc = new char[strlen(fecha_delito_desc) + 1];
	strcpy(this->fecha_delito_desc, fecha_delito_desc);
}
int Multa::getPagada() const
{
	return this->pagada;
}
void Multa::setPagada(int pagada)
{
	this->pagada = pagada;
}