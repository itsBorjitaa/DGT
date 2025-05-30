#ifndef USUARIO_H
#define USUARIO_H

#include "sqlite3.h"
class Usuario
{
private:
	char* nombre;
    char* apellidos;
    char* dni;
    char* email;
    char* telefono;
	char* usuario;
	char* contrasena;
	char* rol;
public:
	Usuario();
	Usuario(const char*, const char*, const char*, const char*, const char*, const char*, const char*, const char*);
	//Usuario(const Usuario&);      // NO QUEREMOS QUE SE PUEDA COPIAR UN USUARIO
	~Usuario();

	const char* getNombre() const;
	void setNombre(const char*);
	const char* getApellidos() const;
	void setApellidos(const char*);
	const char* getDni() const;
	void setDni(const char*);
	const char* getEmail() const;
	void setEmail(const char*);
	const char* getTelefono() const;
	void setTelefono(const char*);
	const char* getUsuario() const;
	void setUsuario(const char*);
	const char* getContrasena() const;
	void setContrasena(const char*);
	const char* getRol() const;
    std::string obtenerTexto(sqlite3_stmt *stmt, int columna) const;
    void setRol(const char *);

    void consultarDatosUsuario() const;
	void consultarTodosLosUsuarios() const;
    void guardarDatosUsuarioEnTXT() const;
};

#endif /* USUARIO_H_ */