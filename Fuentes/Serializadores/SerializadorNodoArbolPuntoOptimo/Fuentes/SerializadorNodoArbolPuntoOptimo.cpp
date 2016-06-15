#include "../Headers/SerializadorNodoArbolPuntoOptimo.h"
#include "../../SerializadorRegistro/SerializadorRegistroFactory.h"
#include "../../SerializadorFeature/SerializadorFeatureFactory.h"
#include "../../SerializadorNumerico/SerializadorNumericoFactory.h"
#include "../../../Sistema/Sistema/Headers/Sistema.h"
#include "../../../Utils/NodoArbolPuntoOptimo/Headers/iNodoArbolPuntoOptimo.h"

SerializadorNodoArbolPuntoOptimo::SerializadorNodoArbolPuntoOptimo()
{
	this->serializadorNumerico = SerializadorNumericoFactory_Nuevo();
	this->serializadorFeature = SerializadorFeatureFactory_Nuevo();
	this->serializadorRegistro = SerializadorRegistroFactory_Nuevo();
}

SerializadorNodoArbolPuntoOptimo::~SerializadorNodoArbolPuntoOptimo()
{
	if (this->serializadorNumerico)
		this->serializadorNumerico->Dispose();

	if (this->serializadorFeature)
		this->serializadorFeature->Dispose();

	if (this->serializadorRegistro)
		this->serializadorRegistro->Dispose();	
}

size_t SerializadorNodoArbolPuntoOptimo::CalcularEspacioSerializacion(iNodoPtr nodo)
{
	size_t espacio = 0;
	size_t cantidadRegistros = nodo->ObtenerCantidadRegistros();

	iNodoArbolPuntoOptimoNodoHojaPtr nodoArbolPuntoOptimoNodoHoja = (iNodoArbolPuntoOptimoNodoHojaPtr)nodo;

	eNodoArbolPuntoOptimo tipoNodo = nodoArbolPuntoOptimoNodoHoja->ObtenerTipoNodo();
	uNumber number;
	number.entero.enteroSinSigno.entero8SinSigno = tipoNodo;
	espacio += this->serializadorNumerico->CalcularEspacio(number, eValueType::eValueType_U1);

	if (tipoNodo == eNodoArbolPuntoOptimo::eNodoArbolPuntoOptimo_Interno)
	{
		iNodoArbolPuntoOptimoNodoInternoPtr nodoArbolPuntoOptimoNodoInterno = (iNodoArbolPuntoOptimoNodoInternoPtr)nodoArbolPuntoOptimoNodoHoja;

		iFeaturePtr pivote = nodoArbolPuntoOptimoNodoInterno->ObtenerPivote();
		espacio += this->serializadorFeature->CalcularEspacio(pivote);

		number.flotante.flotante32 = nodoArbolPuntoOptimoNodoInterno->ObtenerRadio();
		espacio += this->serializadorNumerico->CalcularEspacio(number, eValueType::eValueType_F32);

		number.entero.enteroSinSigno.entero32SinSigno = nodoArbolPuntoOptimoNodoInterno->ObtenerHijoIzquierdo();
		espacio += this->serializadorNumerico->CalcularEspacio(number, eValueType::eValueType_U4);

		number.entero.enteroSinSigno.entero32SinSigno = nodoArbolPuntoOptimoNodoInterno->ObtenerHijoDerecho();
		espacio += this->serializadorNumerico->CalcularEspacio(number, eValueType::eValueType_U4);
	}

	number.entero.enteroSinSigno.entero8SinSigno = cantidadRegistros;
	espacio += this->serializadorNumerico->CalcularEspacio(number, eValueType::eValueType_U1);

	for (size_t i = 0; i < cantidadRegistros; i++)
		Sistema_Execute(espacio += this->serializadorRegistro->CalcularEspacio(nodo->ObtenerRegistro(i)););

	return espacio;
}

size_t SerializadorNodoArbolPuntoOptimo::Serializar(char *buff, iNodoPtr nodo)
{
	size_t espacio = 0;
	size_t cantidadRegistros = nodo->ObtenerCantidadRegistros();

	iNodoArbolPuntoOptimoNodoHojaPtr nodoArbolPuntoOptimoNodoHoja = (iNodoArbolPuntoOptimoNodoHojaPtr)nodo;

	eNodoArbolPuntoOptimo tipoNodo = nodoArbolPuntoOptimoNodoHoja->ObtenerTipoNodo();
	uNumber number;
	number.entero.enteroSinSigno.entero8SinSigno = tipoNodo;
	espacio += this->serializadorNumerico->Serializar(buff + espacio, number, eValueType::eValueType_U1);

	if (tipoNodo == eNodoArbolPuntoOptimo::eNodoArbolPuntoOptimo_Interno)
	{
		iNodoArbolPuntoOptimoNodoInternoPtr nodoArbolPuntoOptimoNodoInterno = (iNodoArbolPuntoOptimoNodoInternoPtr)nodoArbolPuntoOptimoNodoHoja;

		iFeaturePtr pivote = nodoArbolPuntoOptimoNodoInterno->ObtenerPivote();
		espacio += this->serializadorFeature->Serializar(buff + espacio, pivote);

		number.flotante.flotante32 = nodoArbolPuntoOptimoNodoInterno->ObtenerRadio();
		espacio += this->serializadorNumerico->Serializar(buff + espacio, number, eValueType::eValueType_F32);

		number.entero.enteroSinSigno.entero32SinSigno = nodoArbolPuntoOptimoNodoInterno->ObtenerHijoIzquierdo();
		espacio += this->serializadorNumerico->Serializar(buff + espacio, number, eValueType::eValueType_U4);

		number.entero.enteroSinSigno.entero32SinSigno = nodoArbolPuntoOptimoNodoInterno->ObtenerHijoDerecho();
		espacio += this->serializadorNumerico->Serializar(buff + espacio, number, eValueType::eValueType_U4);
	}

	number.entero.enteroSinSigno.entero8SinSigno = cantidadRegistros;
	espacio += this->serializadorNumerico->Serializar(buff + espacio, number, eValueType::eValueType_U1);

	for (size_t i = 0; i < cantidadRegistros; i++)
		Sistema_Execute(espacio += this->serializadorRegistro->Serializar(buff + espacio, nodo->ObtenerRegistro(i)););

	return espacio;
}

void SerializadorNodoArbolPuntoOptimo::Dispose()
{
	delete this;
}
