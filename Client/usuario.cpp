#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "database.h"
#include "logger.h"
#include "usuario.h"
#include <iomanip>
#include <fstream>

// variable global para guardar el usuario actual
extern char usuarioActual[50];
// declaración externa de la base de datos
extern sqlite3 *db;

Usuario::Usuario()
{
	this->nombre = new char[1];
	this->nombre[0] = '\0';
	this->apellidos = new char[1];
	this->apellidos[0] = '\0';
	this->dni = new char[1];
	this->dni[0] = '\0';
	this->email = new char[1];
	this->email[0] = '\0';
	this->telefono = new char[1];
	this->telefono[0] = '\0';
    *this->usuario = new char[50];
	this->usuario[50] = '\0';
    *this->contrasena = new char[50];
	this->contrasena[50] = '\0';
    *this->rol = new char[10];
	this->rol[10] = '\0';
}
Usuario::Usuario(const char* nombre, const char* apellidos, const char* dni, const char* email, const char* telefono, const char* usuario, const char* contrasena, const char* rol)
{
	this->nombre = new char[strlen(nombre) + 1];
	strcpy(this->nombre, nombre);
    this->apellidos = new char[strlen(apellidos) + 1];
	strcpy(this->apellidos, apellidos);
    this->dni = new char[strlen(dni) + 1];
	strcpy(this->dni, dni);
    this->email = new char[strlen(email) + 1];
	strcpy(this->email, email);
    this->telefono = new char[strlen(telefono) + 1];
	strcpy(this->telefono, telefono);
    *this->usuario = new char[strlen(usuario) + 1];
	strcpy(*this->usuario, usuario);
    *this->contrasena = new char[strlen(contrasena) + 1];
	strcpy(*this->contrasena, contrasena);
    *this->rol = new char[strlen(rol) + 1];
	strcpy(*this->rol, rol);
}
Usuario::~Usuario()
{
    delete[] this->nombre;
    delete[] this->apellidos;
    delete[] this->dni;
    delete[] this->email;
    delete[] this->telefono;
    delete[] this->usuario;
    delete[] this->contrasena;
    delete[] this->rol;
}
void Usuario::setNombre(const char* nombre)
{
	delete[] this->nombre;
	this->nombre = new char[strlen(nombre) + 1];
	strcpy(this->nombre, nombre);
}
char* Usuario::getNombre() const
{
	return this->nombre;
}
void Usuario::setApellidos(const char* apellidos)
{
	delete[] this->apellidos;
	this->apellidos = new char[strlen(apellidos) + 1];
	strcpy(this->apellidos, apellidos);
}
char* Usuario::getApellidos() const
{
	return this->apellidos;
}
void Usuario::setDni(const char* dni)
{
	delete[] this->dni;
	this->dni = new char[strlen(dni) + 1];
	strcpy(this->dni, dni);
}
char* Usuario::getDni() const
{
	return this->dni;
}
void Usuario::setEmail(const char* email)
{
	delete[] this->email;
	this->email = new char[strlen(email) + 1];
	strcpy(this->email, email);
}
char* Usuario::getEmail() const
{
	return this->email;
}
void Usuario::setTelefono(const char* telefono)
{
	delete[] this->telefono;
	this->telefono = new char[strlen(telefono) + 1];
	strcpy(this->telefono, telefono);
}
char* Usuario::getTelefono() const
{
	return this->telefono;
}
void Usuario::setUsuario(const char* usuario)
{
	delete[] this->usuario;
	*this->usuario = new char[strlen(usuario) + 1];
	strcpy(*this->usuario, usuario);
}
char* Usuario::getUsuario() const
{
	return *this->usuario;
}
void Usuario::setContrasena(const char* contrasena)
{
	delete[] this->contrasena;
	*this->contrasena = new char[strlen(contrasena) + 1];
	strcpy(*this->contrasena, contrasena);
}
char* Usuario::getContrasena() const
{
	return *this->contrasena;
}
void Usuario::setRol(const char* rol)
{
	delete[] this->rol;
	*this->rol = new char[strlen(rol) + 1];
	strcpy(*this->rol, rol);
}
char* Usuario::getRol() const
{
	return *this->rol;
}
void Usuario::consultarDatosUsuario() const
{

    std::cout << "Consultando datos del usuario...\n";
    registrarAccion(usuarioActual, "Consulto sus datos personales");
    
    const std::string sql = "SELECT nombre, apellidos, dni, email, telefono FROM usuarios WHERE usuario = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Error preparando consulta de datos personales: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    
    rc = sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cout << "Error vinculando parametro de usuario: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return;
    }
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        std::cout << "\n--- Datos Personales ---\n";
        
        // Función auxiliar para obtener texto de columna de forma segura
        auto obtenerTexto = [&](int columna) -> std::string {
            const unsigned char* texto = sqlite3_column_text(stmt, columna);
            return texto ? std::string(reinterpret_cast<const char*>(texto)) : "N/A";
        };
        
        std::cout << "Nombre: " << obtenerTexto(0) << std::endl;
        std::cout << "Apellidos: " << obtenerTexto(1) << std::endl;
        std::cout << "DNI: " << obtenerTexto(2) << std::endl;
        std::cout << "Email: " << obtenerTexto(3) << std::endl;
        std::cout << "Telefono: " << obtenerTexto(4) << std::endl;
        
    } else if (rc == SQLITE_DONE) {
        std::cout << "No se encontraron datos personales para este usuario.\n";
    } else {
        std::cout << "Error al consultar datos personales: " << sqlite3_errmsg(db) << std::endl;
    }
    
    sqlite3_finalize(stmt);

}
void Usuario::consultarTodosLosUsuarios() const
{
	std::cout << "Consultando todos los usuarios...\n";
    registrarAccion(usuarioActual, "Consulto la lista de todos los usuarios");
    
    const std::string sql = "SELECT usuario, nombre, apellidos, dni, email, telefono FROM usuarios";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "Error al consultar usuarios.\n";
        return;
    }
    
    std::cout << "\n--- Lista de Usuarios ---\n";
    
    // Configurar formato de columnas con setw y left
    std::cout << std::left 
              << std::setw(15) << "Usuario"
              << std::setw(15) << "Nombre" 
              << std::setw(20) << "Apellidos"
              << std::setw(10) << "DNI"
              << std::setw(25) << "Email"
              << std::setw(15) << "Telefono" << std::endl;
    
    // Línea separadora
    std::cout << std::string(100, '-') << std::endl;
    
    // Función auxiliar para obtener texto de columna de forma segura
    auto obtenerTexto = [&](int columna) -> std::string {
        const unsigned char* texto = sqlite3_column_text(stmt, columna);
        return texto ? std::string(reinterpret_cast<const char*>(texto)) : "N/A";
    };
    
    // Mostrar cada fila
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::cout << std::left
                  << std::setw(15) << obtenerTexto(0)  // Usuario
                  << std::setw(15) << obtenerTexto(1)  // Nombre
                  << std::setw(20) << obtenerTexto(2)  // Apellidos
                  << std::setw(10) << obtenerTexto(3)  // DNI
                  << std::setw(25) << obtenerTexto(4)  // Email
                  << std::setw(15) << obtenerTexto(5)  // Telefono
                  << std::endl;
    }
    
    sqlite3_finalize(stmt);
}
void Usuario::guardarDatosUsuarioEnTXT() const
{
	std::string nombreArchivo;
    std::cout << "Introduzca el nombre con el que desea guardar el archivo (sin extension): ";
    std::getline(std::cin, nombreArchivo);
    nombreArchivo += ".txt";

    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cout << "Error al crear el archivo.\n";
        return;
    }

    registrarAccion(usuarioActual, "Exportó sus datos a un archivo TXT");

    // Función auxiliar para obtener texto de columna de forma segura
    auto obtenerTexto = [](sqlite3_stmt* stmt, int columna) -> std::string {
        const unsigned char* texto = sqlite3_column_text(stmt, columna);
        return texto ? std::string(reinterpret_cast<const char*>(texto)) : "N/A";
    };

    // Datos personales
    const std::string sqlPersonal = "SELECT nombre, apellidos, dni, email, telefono FROM usuarios WHERE usuario = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sqlPersonal.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Error preparando consulta de datos personales: " 
                  << sqlite3_errmsg(db) << std::endl;
        archivo.close();
        return;
    }

    sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);

    std::string dniUsuario;
    if (rc == SQLITE_ROW) {
        dniUsuario = obtenerTexto(stmt, 2);
        archivo << "--- Datos Personales ---\n";
        archivo << "Nombre: " << obtenerTexto(stmt, 0) << "\n";
        archivo << "Apellidos: " << obtenerTexto(stmt, 1) << "\n";
        archivo << "DNI: " << dniUsuario << "\n";
        archivo << "Email: " << obtenerTexto(stmt, 3) << "\n";
        archivo << "Telefono: " << obtenerTexto(stmt, 4) << "\n\n";
    } else {
        std::cout << "No se encontraron datos personales para este usuario.\n";
    }
    sqlite3_finalize(stmt);

    // Vehículos
    archivo << "--- Vehículos Registrados ---\n";
    const std::string sqlVehiculos = "SELECT matricula, marca, modelo, anio, color, tipo_vehiculo FROM vehiculos WHERE usuario = ?";
    
    rc = sqlite3_prepare_v2(db, sqlVehiculos.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);

    bool vehiculosEncontrados = false;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        vehiculosEncontrados = true;
        archivo << "Matrícula: " << obtenerTexto(stmt, 0) << "\n";
        archivo << "Marca: " << obtenerTexto(stmt, 1) << "\n";
        archivo << "Modelo: " << obtenerTexto(stmt, 2) << "\n";
        archivo << "Año: " << sqlite3_column_int(stmt, 3) << "\n";
        archivo << "Color: " << obtenerTexto(stmt, 4) << "\n";
        archivo << "Tipo de Vehículo: " << obtenerTexto(stmt, 5) << "\n\n";
    }
    sqlite3_finalize(stmt);
    if (!vehiculosEncontrados) {
        archivo << "No tiene vehículos registrados.\n\n";
    }

    // Multas
    archivo << "--- Multas Asociadas ---\n";
    const std::string sqlMultas = "SELECT concepto, fecha_delito, importe, fecha_limite_descuento, pagada, fecha_pago "
                                  "FROM multas WHERE dni = ?";
    
    rc = sqlite3_prepare_v2(db, sqlMultas.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, dniUsuario.c_str(), -1, SQLITE_STATIC);

    bool multasEncontradas = false;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        multasEncontradas = true;
        archivo << "Concepto: " << obtenerTexto(stmt, 0) << "\n";
        archivo << "Fecha del Delito: " << obtenerTexto(stmt, 1) << "\n";
        archivo << "Importe: " << std::fixed << std::setprecision(2) 
                << sqlite3_column_double(stmt, 2) << "€\n";
        archivo << "Fecha Límite Descuento: " << obtenerTexto(stmt, 3) << "\n";
        archivo << "Pagada: " << (sqlite3_column_int(stmt, 4) ? "Sí" : "No") << "\n";
        
        std::string fechaPago = obtenerTexto(stmt, 5);
        archivo << "Fecha de Pago: " << (fechaPago != "N/A" ? fechaPago : "No aplicable") << "\n\n";
    }
    sqlite3_finalize(stmt);
    if (!multasEncontradas) {
        archivo << "No tiene multas registradas.\n\n";
    }

    // Accidentes
    archivo << "--- Historial de Accidentes ---\n";
    const std::string sqlAccidentes = "SELECT id, fecha, descripcion FROM accidentes WHERE usuario = ?";
    
    rc = sqlite3_prepare_v2(db, sqlAccidentes.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, usuarioActual, -1, SQLITE_STATIC);

    bool accidentesEncontrados = false;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        accidentesEncontrados = true;
        archivo << "ID: " << sqlite3_column_int(stmt, 0) << "\n";
        archivo << "Fecha: " << obtenerTexto(stmt, 1) << "\n";
        archivo << "Descripción: " << obtenerTexto(stmt, 2) << "\n\n";
    }
    sqlite3_finalize(stmt);
    if (!accidentesEncontrados) {
        archivo << "No tiene accidentes registrados.\n\n";
    }

    archivo.close();
    std::cout << "Datos exportados correctamente a " << nombreArchivo << std::endl;

}