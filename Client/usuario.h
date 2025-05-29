#ifndef USUARIO_H
#define USUARIO_H

class Usuario
{
private:
	char* nombre;
    char* apellidos;
    char* dni;
    char* email;
    char* telefono;
	char* usuario[50];
	char* contrasena[50];
	char* rol[10];
public:
	Usuario();
	Usuario(const char*, const char*, const char*, const char*, const char*, const char*, const char*, const char*);
	//Usuario(const Usuario&);      // NO QUEREMOS QUE SE PUEDA COPIAR UN USUARIO
	~Usuario();

	char* getNombre() const;
	void setNombre(const char*);
	char* getApellidos() const;
	void setApellidos(const char*);
	char* getDni() const;
	void setDni(const char*);
	char* getEmail() const;
	void setEmail(const char*);
	char* getTelefono() const;
	void setTelefono(const char*);
	char* getUsuario() const;
	void setUsuario(const char*);
	char* getContrasena() const;
	void setContrasena(const char*);
	char* getRol() const;
	void setRol(const char*);

	void consultarDatosUsuario() const;
	void consultarTodosLosUsuarios() const;
    void guardarDatosUsuarioEnTXT() const;
};

#endif /* USUARIO_H_ */