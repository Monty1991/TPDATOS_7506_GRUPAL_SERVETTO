/* 
 * File:   SerializadorNodoArbolPuntoOptimo.h
 * Author: Monty
 *
 * Created on 24 de mayo de 2016, 00:55
 */

#ifndef SERIALIZADORNODOARBOLPUNTOOPTIMO_H
#define SERIALIZADORNODOARBOLPUNTOOPTIMO_H

#include "../../SerializadorNodo/Headers/iSerializadorNodo.h"
#include "../../SerializadorNumerico/Headers/iSerializadorNumerico.h"
#include "../../SerializadorRegistro/Headers/iSerializadorRegistro.h"

class SerializadorNodoArbolPuntoOptimo: public iSerializadorNodo
{
	private:
		iSerializadorNumericoPtr serializadorNumerico;
		iSerializadorRegistroPtr serializadorRegistro;

		virtual ~SerializadorNodoArbolPuntoOptimo();
	public:
		SerializadorNodoArbolPuntoOptimo();

		virtual size_t CalcularEspacioSerializacion(iNodoPtr nodo);

		virtual size_t Serializar(char *buff, iNodoPtr nodo);
		
		virtual void Dispose();
};

#endif /* SERIALIZADORNODOARBOLPUNTOOPTIMO_H */
