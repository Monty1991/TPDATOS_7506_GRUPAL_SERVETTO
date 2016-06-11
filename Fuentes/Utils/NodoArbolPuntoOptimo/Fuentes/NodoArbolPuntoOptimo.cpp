#include "../Headers/NodoArbolPuntoOptimo.h"
#include "../../../Exceptions/ExceptionFactory.h"
#include "../../StringUtils/Headers/StringUtils.h"

NodoArbolPuntoOptimoNodoHoja::NodoArbolPuntoOptimoNodoHoja(iRegistroPtr *listaRegistros, size_t cantidadRegistros)
{
	this->tamanioTablaRegistros = 16;
	// con esto nos aseguramos de que la tabla sea potencia de 2
	while (this->tamanioTablaRegistros < cantidadRegistros)
		this->tamanioTablaRegistros *= 2;

	this->cantidadRegistros = cantidadRegistros;

	this->tablaRegistros = new iRegistroPtr[this->tamanioTablaRegistros];

	for (size_t i = 0; i < cantidadRegistros; i++)
		this->tablaRegistros[i] = listaRegistros[i]->Copiar();
}

NodoArbolPuntoOptimoNodoHoja::~NodoArbolPuntoOptimoNodoHoja()
{
	if (this->tablaRegistros)
	{
		for (size_t i = 0; i < this->cantidadRegistros; i++)
		{
			if (this->tablaRegistros[i])
			{
				this->tablaRegistros[i]->Dispose();
				this->tablaRegistros[i] = NULL;
			}
		}
	}

	delete[] this->tablaRegistros;
	this->tablaRegistros = NULL;
}

iNodoArbolPuntoOptimo *NodoArbolPuntoOptimoNodoHoja::Clone()
{
	return new NodoArbolPuntoOptimoNodoHoja(this->tablaRegistros, this->cantidadRegistros);
}

void NodoArbolPuntoOptimoNodoHoja::Dispose()
{
	delete this;
}

eTipoArbol NodoArbolPuntoOptimoNodoHoja::ObtenerTipoArbol()
{
	return eTipoArbol::eTipoArbol_ArbolPuntoOptimo;
}

eNodoArbolPuntoOptimo NodoArbolPuntoOptimoNodoHoja::ObtenerTipoNodo()
{
	return eNodoArbolPuntoOptimo::eNodoArbolPuntoOptimo_Hoja;
}

size_t NodoArbolPuntoOptimoNodoHoja::ObtenerCantidadRegistros()
{
	return this->cantidadRegistros;
}

iRegistroPtr NodoArbolPuntoOptimoNodoHoja::ObtenerRegistro(size_t pos)
{
	if (pos >= this->cantidadRegistros)
	{
		char msg[20];
		StringUtils_Concatenar(msg, "%lu", pos);
		Throw(ExceptionType_ArrayIndexOutOfBounds, msg);
	}

	return this->tablaRegistros[pos];
}

iRegistroPtr NodoArbolPuntoOptimoNodoHoja::AgregarRegistro(iRegistroPtr reg)
{
	iRegistroPtr old = this->QuitarRegistro(reg->GetFeature(0)->AsNumber().entero.enteroSinSigno.entero32SinSigno);

	this->tablaRegistros[this->cantidadRegistros++] = reg;
	if (this->cantidadRegistros >= this->tamanioTablaRegistros)
		this->RedimensionarTabla(this->tamanioTablaRegistros * 2);

	return old;
}

iRegistroPtr NodoArbolPuntoOptimoNodoHoja::QuitarRegistro(size_t pos)
{
	if (pos >= this->cantidadRegistros)
		return NULL;

	iRegistroPtr reg = reg = this->tablaRegistros[pos];
	this->cantidadRegistros--;
	for (size_t i = pos; pos < this->cantidadRegistros; i++)
		this->tablaRegistros[i] = this->tablaRegistros[i + 1];

	if (this->cantidadRegistros <= this->tamanioTablaRegistros * 4)
		this->RedimensionarTabla(this->tamanioTablaRegistros / 2);

	return reg;
}

iRegistroPtr NodoArbolPuntoOptimoNodoHoja::QuitarRegistro()
{
	return this->QuitarRegistro(0);
}

void NodoArbolPuntoOptimoNodoHoja::RedimensionarTabla(size_t nuevoTamanio)
{
	iRegistroPtr *nuevaTabla = new iRegistroPtr[nuevoTamanio];

	for (size_t i = 0; i < this->cantidadRegistros; i++)
		nuevaTabla[i] = this->tablaRegistros[i];

	delete [] this->tablaRegistros;
	this->tablaRegistros = nuevaTabla;
	this->tamanioTablaRegistros = nuevoTamanio;
}

NodoArbolPuntoOptimoNodoInterno::NodoArbolPuntoOptimoNodoInterno(iRegistroPtr *listaRegistros, size_t cantidadRegistros): pivote(NULL), radio(0), hijoIzquierdo(0), hijoDerecho(0)
{
	this->nodoHoja = new NodoArbolPuntoOptimoNodoHoja(listaRegistros, cantidadRegistros);
}

NodoArbolPuntoOptimoNodoInterno::~NodoArbolPuntoOptimoNodoInterno()
{
	if (this->nodoHoja)
		this->nodoHoja->Dispose();

	if (this->pivote)
		this->pivote->Dispose();
}

iNodoArbolPuntoOptimo *NodoArbolPuntoOptimoNodoInterno::Clone()
{
	iNodoArbolPuntoOptimoNodoInternoPtr copia = new NodoArbolPuntoOptimoNodoInterno(NULL, 0);
	for (size_t i = 0; i < this->ObtenerCantidadRegistros(); i++)
		copia->AgregarRegistro(this->ObtenerRegistro(i));

	copia->EstablecerPivote(this->ObtenerPivote());
	copia->EstablecerRadio(this->ObtenerRadio());
	copia->EstablecerHijoIzquierdo(this->ObtenerHijoIzquierdo());
	copia->EstablecerHijoDerecho(this->ObtenerHijoDerecho());
	
	return copia;
}

void NodoArbolPuntoOptimoNodoInterno::Dispose()
{
	delete this;
}

eTipoArbol NodoArbolPuntoOptimoNodoInterno::ObtenerTipoArbol()
{
	return this->nodoHoja->ObtenerTipoArbol();
}

eNodoArbolPuntoOptimo NodoArbolPuntoOptimoNodoInterno::ObtenerTipoNodo()
{
	return eNodoArbolPuntoOptimo::eNodoArbolPuntoOptimo_Interno;
}

iFeaturePtr NodoArbolPuntoOptimoNodoInterno::ObtenerPivote()
{
	return this->pivote;
}

void NodoArbolPuntoOptimoNodoInterno::EstablecerPivote(iFeaturePtr pivote)
{
	// Si es el mismo pivote, no tiene sentido reescribir
	if (this->pivote == pivote)
		return;

	if (this->pivote)
	{
		this->pivote->Dispose();
		this->pivote = NULL;
	}

	if (pivote)
		this->pivote = pivote->Copiar();
}

float NodoArbolPuntoOptimoNodoInterno::ObtenerRadio()
{
	return this->radio;
}

void NodoArbolPuntoOptimoNodoInterno::EstablecerRadio(float radio)
{
	this->radio = radio;
}

void NodoArbolPuntoOptimoNodoInterno::EstablecerHijoIzquierdo(size_t nodo)
{
	this->hijoIzquierdo = nodo;
}

size_t NodoArbolPuntoOptimoNodoInterno::ObtenerHijoIzquierdo()
{
	return this->hijoIzquierdo;
}

void NodoArbolPuntoOptimoNodoInterno::EstablecerHijoDerecho(size_t nodo)
{
	this->hijoDerecho = nodo;
}

size_t NodoArbolPuntoOptimoNodoInterno::ObtenerHijoDerecho()
{
	return this->hijoDerecho;
}

size_t NodoArbolPuntoOptimoNodoInterno::ObtenerCantidadRegistros()
{
	return this->nodoHoja->ObtenerCantidadRegistros();	
}

iRegistroPtr NodoArbolPuntoOptimoNodoInterno::ObtenerRegistro(size_t pos)
{
	return this->nodoHoja->ObtenerRegistro(pos);	
}

iRegistroPtr NodoArbolPuntoOptimoNodoInterno::AgregarRegistro(iRegistroPtr reg)
{
	return this->nodoHoja->AgregarRegistro(reg);
}

iRegistroPtr NodoArbolPuntoOptimoNodoInterno::QuitarRegistro(size_t pos)
{
	return this->nodoHoja->QuitarRegistro(pos);
}

iRegistroPtr NodoArbolPuntoOptimoNodoInterno::QuitarRegistro()
{
	return this->nodoHoja->QuitarRegistro();
}
