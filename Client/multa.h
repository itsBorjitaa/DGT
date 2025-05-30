#ifndef MULTA_H
#define MULTA_H

class Multa
{
private:
	char* id;
    char* dni;
    char* concepto;
    char* fecha_delito;
    double importe;
    char* fecha_delito_desc;
    int pagada;
    char* fecha_pago;
public:
	Multa();
	Multa(const char*, const char*, const char*, const char*, double, const char*, int , const char*);
	//Multa(const Multa&);      // NO QUEREMOS QUE SE PUEDA COPIAR
	~Multa();

	char* getId() const;
	void setId(const char*);
	char* getDni() const;
	void setDni(const char*);
	char* getConcepto() const;
	void setConcepto(const char*);
    char* getFecha_delito() const;
	void setFecha_delito(const char*);
    double getImporte() const;
	void setImporte(double);
    int getPagada() const;
	void setPagada(int);
	char* getFecha_delito_desc() const;
	void setFecha_delito_desc(const char*);
    char* getFecha_pago() const;
	void setFecha_pago(const char*);

};

#endif /* MULTA_H_ */