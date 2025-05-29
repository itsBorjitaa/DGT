#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include "usuario.h"
#include "database.h"
#include "logger.h"

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
void consultarDatosAccidente(int idAccidente) {
    std::cout << "Consultando datos del accidente con ID " << idAccidente << "..." << std::endl;
    
    // Crear buffer para registrarAccion
    char usuario_buffer[50];
    strcpy(usuario_buffer, usuarioActual);
    registrarAccion(usuario_buffer, "Consulto un accidente específico");

    const char* sql = "SELECT fecha, descripcion FROM accidentes WHERE id = ?";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cout << "Error preparando consulta de accidente: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, idAccidente);
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        std::cout << "\n--- Datos del Accidente ---" << std::endl;
        std::cout << "Fecha: " << (sqlite3_column_text(stmt, 0) ? 
                     reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)) : "N/A") << std::endl;
        std::cout << "Descripcion: " << (sqlite3_column_text(stmt, 1) ? 
                     reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) : "N/A") << std::endl;
    } else {
        std::cout << "No se encontro el accidente con ID " << idAccidente << "." << std::endl;
    }

    sqlite3_finalize(stmt);
}

void consultarAccidentesUsuario(const std::string& usuario) {
    if (usuario.empty()) {
        std::cout << "Error: Usuario inválido." << std::endl;
        return;
    }
    
    // Crear buffer para registrarAccion
    char usuario_buffer[50];
    strcpy(usuario_buffer, usuario.c_str());
    registrarAccion(usuario_buffer, "Consultó su historial de accidentes");

    std::string sql = "SELECT id, fecha, descripcion FROM accidentes WHERE usuario='" + usuario + "'";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        std::cout << "Error al consultar accidentes." << std::endl;
        return;
    }

    std::cout << "\n--- Historial de Accidentes ---" << std::endl;
    bool encontrados = false;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrados = true;
        int id = sqlite3_column_int(stmt, 0);
        const char *fecha = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char *descripcion = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        std::cout << "ID: " << id 
                  << " | Fecha: " << (fecha ? fecha : "N/A")
                  << " | Descripcion: " << (descripcion ? descripcion : "N/A") << std::endl;
    }
    
    if (!encontrados) {
        std::cout << "No se encontraron accidentes registrados." << std::endl;
    }

    sqlite3_finalize(stmt);
}

// Sobrecarga para compatibilidad con char*
void consultarAccidentesUsuario(char *usuario) {
    if (usuario == nullptr) {
        consultarAccidentesUsuario(std::string(""));
        return;
    }
    consultarAccidentesUsuario(std::string(usuario));
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

void registrarAccidenteConValidacion() {
    std::string fecha, descripcion;

    do {
        std::cout << "Fecha del accidente (YYYY-MM-DD): ";
        std::cin >> fecha;
        std::cin.ignore(); // Limpiar el buffer

        if (!validarFecha(fecha)) {
            std::cout << "Error: Fecha invalida. Debe estar en formato YYYY-MM-DD con valores correctos." << std::endl;
        }
    } while (!validarFecha(fecha));

    std::cout << "Descripcion del accidente: ";
    std::getline(std::cin, descripcion);

    // Crear buffers para las funciones C
    char usuario_buffer[50];
    char fecha_buffer[20];
    char descripcion_buffer[200];
    
    strcpy(usuario_buffer, usuarioActual);
    strcpy(fecha_buffer, fecha.c_str());
    strcpy(descripcion_buffer, descripcion.c_str());

    registrarAccidente(usuario_buffer, fecha_buffer, descripcion_buffer);
    registrarAccion(usuario_buffer, "Usuario registró un nuevo accidente");
}