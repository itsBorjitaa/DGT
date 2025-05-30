#ifndef ACCIDENTE_H
#define ACCIDENTE_H

class Accidente
{
private:
	char* usuario;
    char* fecha;
    char* descripcion;
public:
	Accidente();
	Accidente(const char*, const char*, const char*);
	//Accidente(const Accidente&);      // NO QUEREMOS QUE SE PUEDA COPIAR
	~Accidente();

	char* getUsuario() const;
	void setUsuario(const char*);
	char* getFecha() const;
	void setFecha(const char*);
	char* getDescripcion() const;
	void setDescripcion(const char*);

};

#endif /* ACCIDENTE_H_ */