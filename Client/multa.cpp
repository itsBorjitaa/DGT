#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include "multa.h"
#include "database.h"
#include "logger.h"

#include "multa.h"

// variable global para guardar el usuario actual
extern char usuarioActual[50];
// declaración externa de la base de datos
extern sqlite3 *db;

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
void consultarMultasAdmin() {
    const char* sql = "SELECT id, dni, concepto, fecha_delito, importe, fecha_limite_descuento, pagada, fecha_pago FROM multas";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cout << "Error al consultar multas." << std::endl;
        return;
    }
    
    std::cout << "\n--- Lista de Multas ---" << std::endl;
    std::cout << std::left << std::setw(4) << "ID" 
              << std::setw(15) << "DNI" 
              << std::setw(25) << "Concepto" 
              << std::setw(12) << "Fecha Delito" 
              << std::setw(10) << "Importe" 
              << std::setw(15) << "Fecha Lim Desc" 
              << std::setw(7) << "Pagada" 
              << std::setw(12) << "Fecha Pago" << std::endl;
    std::cout << std::string(96, '-') << std::endl;
    
    bool encontrados = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrados = true;
        int pagada = sqlite3_column_int(stmt, 6);
        
        std::cout << std::left << std::setw(4) << sqlite3_column_int(stmt, 0)
                  << std::setw(15) << (sqlite3_column_text(stmt, 1) ? 
                                      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) : "N/A")
                  << std::setw(25) << (sqlite3_column_text(stmt, 2) ? 
                                      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) : "N/A")
                  << std::setw(12) << (sqlite3_column_text(stmt, 3) ? 
                                      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "N/A")
                  << std::setw(10) << std::fixed << std::setprecision(2) << sqlite3_column_double(stmt, 4)
                  << std::setw(15) << (sqlite3_column_text(stmt, 5) ? 
                                      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)) : "N/A")
                  << std::setw(7) << (pagada ? "Si" : "No")
                  << std::setw(12) << (sqlite3_column_text(stmt, 7) ? 
                                      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)) : "N/A") << std::endl;
    }
    
    if (!encontrados) {
        std::cout << "No se encontraron multas registradas." << std::endl;
    }
    
    sqlite3_finalize(stmt);
}

void agregarMultaAdmin() {
    std::string dni;
    std::string concepto;
    std::string fecha_delito;
    double importe;
    std::string fecha_limite_descuento;
    
    std::cout << "\n--- Agregar Nueva Multa ---" << std::endl;
    
    std::cout << "Ingrese el DNI del infractor: ";
    std::cin >> dni;
    std::cin.ignore(); // Limpiar el buffer
    
    // verifica si el DNI existe
    std::string sql_check = "SELECT 1 FROM usuarios WHERE dni='" + dni + "'";
    
    sqlite3_stmt *stmt_check;
    bool dni_existe = false;
    
    if (sqlite3_prepare_v2(db, sql_check.c_str(), -1, &stmt_check, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt_check) == SQLITE_ROW) {
            dni_existe = true;
        }
        sqlite3_finalize(stmt_check);
    }
    
    if (!dni_existe) {
        std::cout << "El DNI no esta registrado en el sistema." << std::endl;
        return;
    }
    
    std::cout << "Concepto de la multa: ";
    std::getline(std::cin, concepto);
    
    std::cout << "Fecha del delito (YYYY-MM-DD): ";
    std::cin >> fecha_delito;
    
    std::cout << "Importe (EUR): ";
    std::cin >> importe;
    
    std::cout << "Fecha limite para descuento (YYYY-MM-DD): ";
    std::cin >> fecha_limite_descuento;
    
    if (agregarMulta(dni.c_str(), concepto.c_str(), fecha_delito.c_str(), importe, fecha_limite_descuento.c_str())) {
        std::cout << "Multa registrada correctamente." << std::endl;
    } else {
        std::cout << "Error al registrar la multa." << std::endl;
    }
}

void cambiarEstadoMulta() {
    int id_multa;
    std::string fecha_pago;
    
    std::cout << "\n--- Cambiar Estado de Multa ---" << std::endl;
    
    std::cout << "ID de la multa: ";
    std::cin >> id_multa;
    
    // verifica si la multa existe
    std::string sql_check = "SELECT pagada FROM multas WHERE id=" + std::to_string(id_multa);
    
    sqlite3_stmt *stmt_check;
    bool multa_existe = false;
    bool ya_pagada = false;
    
    if (sqlite3_prepare_v2(db, sql_check.c_str(), -1, &stmt_check, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt_check) == SQLITE_ROW) {
            multa_existe = true;
            ya_pagada = sqlite3_column_int(stmt_check, 0) != 0;
        }
        sqlite3_finalize(stmt_check);
    }
    
    if (!multa_existe) {
        std::cout << "La multa con ID " << id_multa << " no existe." << std::endl;
        return;
    }
    
    if (ya_pagada) {
        std::cout << "La multa ya esta marcada como pagada." << std::endl;
        return;
    }
    
    std::cout << "Fecha de pago (YYYY-MM-DD): ";
    std::cin >> fecha_pago;
    
    if (pagarMulta(id_multa, fecha_pago.c_str())) {
        std::cout << "Estado de la multa actualizado correctamente." << std::endl;
    } else {
        std::cout << "Error al actualizar el estado de la multa." << std::endl;
    }
}

void consultarMultasUsuario() {
    char dni_buffer[20];
    char *dni = obtenerDNI(usuarioActual, dni_buffer);
    
    if (dni == nullptr) {
        std::cout << "Error al obtener el DNI del usuario." << std::endl;
        return;
    }
    
    std::string sql = "SELECT id, concepto, fecha_delito, importe, fecha_limite_descuento, pagada, fecha_pago FROM multas WHERE dni='" + std::string(dni) + "'";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        std::cout << "Error al consultar multas." << std::endl;
        return;
    }
    
    std::cout << "\n--- Mis Multas ---" << std::endl;
    std::cout << std::left << std::setw(4) << "ID" 
              << std::setw(25) << "Concepto" 
              << std::setw(12) << "Fecha Delito" 
              << std::setw(10) << "Importe" 
              << std::setw(15) << "Fecha Lim Desc" 
              << std::setw(7) << "Pagada" 
              << std::setw(12) << "Fecha Pago" << std::endl;
    std::cout << std::string(86, '-') << std::endl;
    
    bool encontrados = false;
    char fecha_actual[15];
    time_t t = time(nullptr);
    struct tm *tm = localtime(&t);
    sprintf(fecha_actual, "%04d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrados = true;
        int id_multa = sqlite3_column_int(stmt, 0);
        double importe_original = sqlite3_column_double(stmt, 3);
        bool pagada = sqlite3_column_int(stmt, 5) != 0;
        const char *fecha_limite = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        double importe_actual = importe_original;
        std::string info_descuento = "";
        
        if (!pagada && fecha_limite && strcmp(fecha_actual, fecha_limite) <= 0) {
            importe_actual = importe_original * 0.5;
            info_descuento = " (" + std::to_string(importe_actual).substr(0, std::to_string(importe_actual).find('.') + 3) + " con desc.)";
        }
        
        std::cout << std::left << std::setw(4) << id_multa
                  << std::setw(25) << (sqlite3_column_text(stmt, 1) ? 
                                      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) : "N/A")
                  << std::setw(12) << (sqlite3_column_text(stmt, 2) ? 
                                      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) : "N/A")
                  << std::setw(10) << std::fixed << std::setprecision(2) << importe_original << info_descuento
                  << std::setw(15) << (fecha_limite ? fecha_limite : "N/A")
                  << std::setw(7) << (pagada ? "Si" : "No")
                  << std::setw(12) << (sqlite3_column_text(stmt, 6) ? 
                                      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)) : "N/A") << std::endl;
    }
    
    if (!encontrados) {
        std::cout << "No tienes multas registradas." << std::endl;
    }
    
    sqlite3_finalize(stmt);
    
    registrarAccion(usuarioActual, "Consulto sus multas.");
}

void pagarMultaUsuario() {
    int id_multa;
    char dni_buffer[20];
    char *dni = obtenerDNI(usuarioActual, dni_buffer);
    
    if (dni == nullptr) {
        std::cout << "Error al obtener el DNI del usuario." << std::endl;
        return;
    }
    
    std::cout << "\n--- Pagar Multa ---" << std::endl;
    
    std::cout << "ID de la multa a pagar: ";
    std::cin >> id_multa;
    
    // verifica si la multa existe y pertenece al usuario
    std::string sql_check = "SELECT pagada, importe, fecha_limite_descuento FROM multas WHERE id=" + 
                           std::to_string(id_multa) + " AND dni='" + std::string(dni) + "'";
    
    sqlite3_stmt *stmt_check;
    bool multa_existe = false;
    bool ya_pagada = false;
    double importe = 0.0;
    std::string fecha_limite;
    
    if (sqlite3_prepare_v2(db, sql_check.c_str(), -1, &stmt_check, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt_check) == SQLITE_ROW) {
            multa_existe = true;
            ya_pagada = sqlite3_column_int(stmt_check, 0) != 0;
            importe = sqlite3_column_double(stmt_check, 1);
            if (sqlite3_column_text(stmt_check, 2))
                fecha_limite = reinterpret_cast<const char*>(sqlite3_column_text(stmt_check, 2));
        }
        sqlite3_finalize(stmt_check);
    }
    
    if (!multa_existe) {
        std::cout << "La multa con ID " << id_multa << " no existe o no te pertenece." << std::endl;
        return;
    }
    
    if (ya_pagada) {
        std::cout << "Esta multa ya está pagada." << std::endl;
        return;
    }
    
    // obtiene fecha actual para calcular descuento
    char fecha_actual[15];
    time_t t = time(nullptr);
    struct tm *tm = localtime(&t);
    sprintf(fecha_actual, "%04d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    
    // calcula el importe con posible descuento teniendo en cuenta la fecha
    double importe_a_pagar = importe;
    if (!fecha_limite.empty() && strcmp(fecha_actual, fecha_limite.c_str()) <= 0) {
        importe_a_pagar = importe * 0.5;
        std::cout << "Buenas noticias! Tienes un descuento del 50% por pronto pago." << std::endl;
    }
    
    std::cout << "Importe a pagar: " << std::fixed << std::setprecision(2) << importe_a_pagar << " EUR" << std::endl;
    std::cout << "Desea proceder con el pago? (1: Si / 0: No): ";
    
    int confirmar;
    std::cin >> confirmar;
    
    if (confirmar) {
        if (pagarMulta(id_multa, fecha_actual)) {
            std::cout << "Multa pagada correctamente. Se ha registrado el pago con fecha " << fecha_actual << "." << std::endl;
            
            // Registrar la acción en el log
            registrarAccion(usuarioActual, "Pago una multa.");
        } else {
            std::cout << "Error al procesar el pago de la multa." << std::endl;
        }
    } else {
        std::cout << "Operacion cancelada." << std::endl;
    }
}