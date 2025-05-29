#ifndef ACCIDENTE_H
#define ACCIDENTE_H

void consultarDatosAccidente();
void consultarAccidentesUsuario(char *usuario);
void registrarAccidenteConValidacion();

class accidente
{
private:
	Miembro* conyugeH;
	Miembro* conyugeM;
public:
	Familia();
	Familia(Miembro*, Miembro*);
	Familia(const Familia&);
	virtual ~Familia();

	void setConyugeH(Miembro*);
	Miembro* getConyugeH() const;
	void setConyugeM(Miembro*);
	Miembro* getConyugeM() const;

	char* getNombre() const;
	virtual void imprimir();
	virtual int contarMenoresEdad(int);
};


#endif