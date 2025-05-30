#ifndef VEHICULO_H
#define VEHICULO_H

class Vehiculo
{
private:
	char* matricula;
    char* marca;
    char* modelo;
    int anio;
    char* color;
	char* tipo_vehiculo;
public:
	Vehiculo();
	Vehiculo(const char*, const char*, const char*, int, const char*, const char*);
	//Vehiculo(const Vehiculo&);      // NO QUEREMOS QUE SE PUEDA COPIAR UN USUARIO
	~Vehiculo();

	char* getMatricula() const;
	void setMatricula(const char*);
	char* getMarca() const;
	void setMarca(const char*);
	char* getModelo() const;
	void setModelo(const char*);
    int getAnio() const;
	void setAnio(int);
	char* getColor() const;
	void setColor(const char*);
    char* getTipo_vehiculo() const;
	void setTipo_vehiculo(const char*);


};

#endif /* VEHICULO_H_ */