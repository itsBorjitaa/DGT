#include <iostream>
#include <string.h>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <ctime>
#include "sqlite3.h"
#include "database.h"
#include "vehiculo.h"
#include "logger.h"

// variable global para guardar el usuario actual
extern char usuarioActual[50];
// declaración externa de la base de datos
extern sqlite3 *db;


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
// Función auxiliar para obtener texto de columna de forma segura
auto obtenerTexto = [](sqlite3_stmt* stmt, int columna) -> std::string {
    const unsigned char* texto = sqlite3_column_text(stmt, columna);
    return texto ? std::string(reinterpret_cast<const char*>(texto)) : "N/A";
};

void consultarDatosVehiculos() {
    const std::string sql = "SELECT matricula, marca, modelo, anio, color, tipo_vehiculo FROM vehiculos WHERE usuario=?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "Error al consultar vehiculos.\n";
        return;
    }
    
    sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);
    
    std::cout << "\n--- Mis Vehiculos ---\n";
    std::cout << std::left
              << std::setw(15) << "Matricula"
              << std::setw(15) << "Marca"
              << std::setw(15) << "Modelo"
              << std::setw(6) << "Anio"
              << std::setw(10) << "Color"
              << std::setw(15) << "Tipo Vehiculo" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    bool encontrados = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrados = true;
        std::cout << std::left
                  << std::setw(15) << obtenerTexto(stmt, 0)  // Matricula
                  << std::setw(15) << obtenerTexto(stmt, 1)  // Marca
                  << std::setw(15) << obtenerTexto(stmt, 2)  // Modelo
                  << std::setw(6) << sqlite3_column_int(stmt, 3)  // Anio
                  << std::setw(10) << obtenerTexto(stmt, 4)  // Color
                  << std::setw(15) << obtenerTexto(stmt, 5)  // Tipo Vehiculo
                  << std::endl;
    }
    
    if (!encontrados) {
        std::cout << "No se encontraron vehiculos registrados.\n";
    }
    
    sqlite3_finalize(stmt);
    registrarAccion(usuarioActual, "Consulto sus datos de vehículos.");
}

void modificarInformacionVehiculo() {
    std::cout << "Modificando informacion del vehiculo...\n";
    registrarAccion(usuarioActual, "Modifico la informacion de un vehículo.");
}

bool validarMatricula(const std::string& matricula) {
    if (matricula.length() != 7) return false; // Debe tener exactamente 7 caracteres
    
    for (int i = 0; i < 4; i++) {
        if (!std::isdigit(matricula[i])) return false; // Primeros 4 caracteres deben ser dígitos
    }
    
    for (int i = 4; i < 7; i++) {
        if (!std::isalpha(matricula[i]) || !std::isupper(matricula[i])) return false; // Últimos 3 deben ser letras mayúsculas
    }
    
    return true;
}

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void agregarNuevoVehiculo() {
    std::string matricula, marca, modelo, color, tipo_vehiculo;
    int anio;
    
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);
    int anio_actual = tm->tm_year + 1900; // Año actual

    std::cout << "\n--- Agregar Nuevo Vehiculo ---\n";

    // Verifica si el usuario ya tiene 3 vehículos
    const std::string sql_count = "SELECT COUNT(*) FROM vehiculos WHERE usuario=?";
    
    sqlite3_stmt* stmt_count;
    int vehiculo_count = 0;
    
    if (sqlite3_prepare_v2(db, sql_count.c_str(), -1, &stmt_count, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt_count, 1, usuarioActual, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt_count) == SQLITE_ROW) {
            vehiculo_count = sqlite3_column_int(stmt_count, 0);
        }
        sqlite3_finalize(stmt_count);
    }
    
    if (vehiculo_count >= 3) {
        std::cout << "Ha alcanzado el limite maximo de vehiculos (3).\n";
        return;
    }

    // Validar matrícula
    do {
        std::cout << "Matricula (formato 1234ABC): ";
        std::cin >> matricula;
        if (!validarMatricula(matricula)) {
            std::cout << "Error: La matricula debe tener 4 numeros y 3 letras mayusculas.\n";
        }
    } while (!validarMatricula(matricula));

    // Verifica si la matrícula ya existe
    const std::string sql_check = "SELECT 1 FROM vehiculos WHERE matricula=?";
    
    sqlite3_stmt* stmt_check;
    bool matricula_existe = false;
    
    if (sqlite3_prepare_v2(db, sql_check.c_str(), -1, &stmt_check, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt_check, 1, matricula.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt_check) == SQLITE_ROW) {
            matricula_existe = true;
        }
        sqlite3_finalize(stmt_check);
    }
    
    if (matricula_existe) {
        std::cout << "Error: La matricula ya existe en el sistema.\n";
        return;
    }

    std::cin.ignore(); // Limpiar buffer
    
    // Validar marca
    do {
        std::cout << "Marca: ";
        std::getline(std::cin, marca);
        if (marca.empty()) {
            std::cout << "Error: La marca no puede estar vacia.\n";
        }
    } while (marca.empty());

    // Validar modelo
    do {
        std::cout << "Modelo: ";
        std::getline(std::cin, modelo);
        if (modelo.empty()) {
            std::cout << "Error: El modelo no puede estar vacio.\n";
        }
    } while (modelo.empty());

    // Validar año
    do {
        std::cout << "Anio de fabricacion (entre 1900 y " << anio_actual << "): ";
        std::cin >> anio;
        if (anio < 1900 || anio > anio_actual) {
            std::cout << "Error: El anio debe estar entre 1900 y " << anio_actual << ".\n";
        }
    } while (anio < 1900 || anio > anio_actual);

    std::cin.ignore(); // Limpiar buffer

    // Validar color
    do {
        std::cout << "Color: ";
        std::getline(std::cin, color);
        if (color.empty()) {
            std::cout << "Error: El color no puede estar vacio.\n";
        }
    } while (color.empty());

    // Validar tipo de vehículo
    do {
        std::cout << "Tipo de vehiculo (coche/moto/camion): ";
        std::getline(std::cin, tipo_vehiculo);
        
        tipo_vehiculo = toLowerCase(tipo_vehiculo); // Convertir a minúsculas
        
        if (tipo_vehiculo != "coche" && tipo_vehiculo != "moto" && tipo_vehiculo != "camion") {
            std::cout << "Error: El tipo de vehículo debe ser 'coche', 'moto' o 'camion'.\n";
        }
    } while (tipo_vehiculo != "coche" && tipo_vehiculo != "moto" && tipo_vehiculo != "camion");

    // Guardar en la base de datos
    const std::string sql = "INSERT INTO vehiculos (usuario, matricula, marca, modelo, anio, color, tipo_vehiculo) "
                           "VALUES (?, ?, ?, ?, ?, ?, ?)";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, matricula.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, marca.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, modelo.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, anio);
        sqlite3_bind_text(stmt, 6, color.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, tipo_vehiculo.c_str(), -1, SQLITE_STATIC);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        if (rc == SQLITE_DONE) {
            std::cout << "Vehiculo registrado correctamente.\n";
            registrarAccion(usuarioActual,"Agrego un nuevo vehículo");
        } else {
            std::cout << "Error al registrar vehículo: " << sqlite3_errmsg(db) << std::endl;
        }
    } else {
        std::cout << "Error al preparar consulta: " << sqlite3_errmsg(db) << std::endl;
    }
}

void consultarTodosLosVehiculos() {
    const std::string sql = "SELECT usuario, matricula, marca, modelo, anio, color, tipo_vehiculo FROM vehiculos";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "Error al consultar vehiculos.\n";
        return;
    }
    
    std::cout << "\n--- Lista de Vehiculos ---\n";
    std::cout << std::left
              << std::setw(15) << "Usuario"
              << std::setw(15) << "Matricula"
              << std::setw(15) << "Marca"
              << std::setw(15) << "Modelo"
              << std::setw(6) << "Anio"
              << std::setw(10) << "Color"
              << std::setw(15) << "Tipo Vehiculo" << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    
    bool encontrados = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrados = true;
        std::cout << std::left
                  << std::setw(15) << obtenerTexto(stmt, 0)  // Usuario
                  << std::setw(15) << obtenerTexto(stmt, 1)  // Matricula
                  << std::setw(15) << obtenerTexto(stmt, 2)  // Marca
                  << std::setw(15) << obtenerTexto(stmt, 3)  // Modelo
                  << std::setw(6) << sqlite3_column_int(stmt, 4)  // Anio
                  << std::setw(10) << obtenerTexto(stmt, 5)  // Color
                  << std::setw(15) << obtenerTexto(stmt, 6)  // Tipo Vehiculo
                  << std::endl;
    }
    
    if (!encontrados) {
        std::cout << "No se encontraron vehiculos registrados.\n";
    }
    
    sqlite3_finalize(stmt);
}