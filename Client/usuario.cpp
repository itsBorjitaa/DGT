#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include "usuario.h"

// Variable global para guardar el usuario actual
extern char usuarioActual[50];

Usuario::Usuario() 
    : nombre(nullptr), apellidos(nullptr), dni(nullptr), email(nullptr), telefono(nullptr), rol(nullptr), usuario(nullptr), contrasena(nullptr) {
    setNombre("");
    setApellidos("");
    setDni("");
    setEmail("");
    setTelefono("");
    setUsuario("");
    setContrasena("");
    setRol("usuario"); // Rol por defecto
}

Usuario::Usuario(const char* nombre, const char* apellidos, const char* dni, const char* email, const char* telefono, const char* usuario,const char* contrasena, const char* rol) {
    setNombre(nombre);
    setApellidos(apellidos);
    setDni(dni);
    setEmail(email);
    setTelefono(telefono);
    setUsuario(usuario);
    setContrasena(contrasena);
    setRol(rol);
}

Usuario::~Usuario() {
    delete[] nombre;
    delete[] apellidos;
    delete[] dni;
    delete[] email;
    delete[] telefono;
    delete[] usuario;
    delete[] contrasena;
    delete[] rol;
}

void Usuario::setNombre(const char* nombre) {
    delete[] this->nombre;
    this->nombre = new char[strlen(nombre) + 1];
    strcpy(this->nombre, nombre);
}

const char* Usuario::getNombre() const {
    return this->nombre;
}

void Usuario::setApellidos(const char* apellidos) {
    delete[] this->apellidos;
    this->apellidos = new char[strlen(apellidos) + 1];
    strcpy(this->apellidos, apellidos);
}

const char* Usuario::getApellidos() const {
    return this->apellidos;
}

void Usuario::setDni(const char* dni) {
    delete[] this->dni;
    this->dni = new char[strlen(dni) + 1];
    strcpy(this->dni, dni);
}

const char* Usuario::getDni() const {
    return this->dni;
}

void Usuario::setEmail(const char* email) {
    delete[] this->email;
    this->email = new char[strlen(email) + 1];
    strcpy(this->email, email);
}

const char* Usuario::getEmail() const {
    return this->email;
}

void Usuario::setTelefono(const char* telefono) {
    delete[] this->telefono;
    this->telefono = new char[strlen(telefono) + 1];
    strcpy(this->telefono, telefono);
}

const char* Usuario::getTelefono() const {
    return this->telefono;
}

void Usuario::setUsuario(const char* usuario) {
    delete[] this->usuario;
    this->usuario = new char[strlen(usuario) + 1];
    strcpy(this->usuario, usuario);
}

const char* Usuario::getUsuario() const {
    return this->usuario;
}

void Usuario::setContrasena(const char* contrasena) {
    delete[] this->contrasena;
    this->contrasena = new char[strlen(contrasena) + 1];
    strcpy(this->contrasena, contrasena);
}

const char* Usuario::getContrasena() const {
    return this->contrasena;
}

void Usuario::setRol(const char* rol) {
    delete[] this->rol;
    this->rol = new char[strlen(rol) + 1];
    strcpy(this->rol, rol);
}

const char* Usuario::getRol() const {
    return this->rol;
}
