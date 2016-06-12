/*
 * VpTree_ABM.cpp
 *
 *  Created on: 25/05/2016
 *      Author: cristian
 */

#include "../Headers/VpTree_ABM.h"
#include "../../../io/ArchivoArbol/ArchivoArbolFactory.h"
#include "../../../Utils/NodoArbolPuntoOptimo/NodoArbolPuntoOptimoFactory.h"
#include "../../../Memoria/Feature/FeatureFactory.h"
#include "../../../Utils/EspacioMetrico/EspacioMetricoFactory.h"
#include "../../../Exceptions/ExceptionFactory.h"
#include "../../Heap/HeapFactory.h"

VpTree_ABM::~VpTree_ABM() {

	if (raiz) {

		raiz->Dispose();
		raiz = NULL;
	}

	if (archivo) {

		archivo->Close();
		archivo = NULL;
	}

	if (this->espacioMetrico) {

		this->espacioMetrico->Dispose();
		this->espacioMetrico = NULL;
	}
}

void VpTree_ABM::Dispose() {

	delete this;
}

void VpTree_ABM::Escribir(size_t _nroNodo, iNodoArbolPuntoOptimoPtr _nodo) {
	if (!_nroNodo && (_nodo != raiz)) {
		iNodoPtr viejaRaiz = this->raiz;
		raiz = _nodo->Clone();

		if (viejaRaiz)
			viejaRaiz->Dispose();
	}

	archivo->EscribirNodo(_nroNodo, _nodo);
}

float VpTree_ABM::Distancia(iFeaturePtr _key1, iFeaturePtr _key2) {

	if (!_key1)
		Throw(ExceptionType_InvalidArg, "_key1 == NULL");
	if (!_key2)
		Throw(ExceptionType_InvalidArg, "_key2 == NULL");

	if (_key1->ObtenerTipo() != _key2->ObtenerTipo())
		Throw(ExceptionType_InvalidArg, "_key1->Tipo != _key2->Tipo");

	// numerico -> usar claves numericas
	if (_key1->ObtenerTipo() & Mascara_Numero)
		return this->espacioMetrico->CalcularDistancia(
				_key1->AsNumber().entero.enteroSinSigno.entero32SinSigno,
				_key2->AsNumber().entero.enteroSinSigno.entero32SinSigno);

	// no numerico -> usar claves de cadenas
	if (!(_key1->ObtenerTipo() & Mascara_Numero))
		return this->espacioMetrico->CalcularDistancia(_key1->AsCadenaANSI(),
				_key2->AsCadenaANSI());

	Throw(ExceptionType_InvalidArg, "_key1->Tipo not supported");
}

VpTree_ABM::VpTree_ABM(const char* _fileName, size_t _nroCampoClave,
		size_t _tamanioNodo, size_t _cargaMinima, size_t _tolerancia) :
		nroCampoClave(_nroCampoClave) {

	this->espacioMetrico = EspacioMetricoFactory_Nuevo();

	archivo = ArchivoArbolFactory_Nuevo(_fileName, _tamanioNodo, _cargaMinima,
			_tolerancia, eTipoArbol_ArbolPuntoOptimo);

	raiz = (iNodoArbolPuntoOptimoPtr) archivo->LeerNodo(0);

	if (raiz == NULL) {

		raiz = NodoArbolPuntoOptimoFactory_Nuevo(eNodoArbolPuntoOptimo_Hoja);
		Escribir(0, raiz);
	}
}

void VpTree_ABM::ResolverUnderflow(size_t _nroNodoPadre,
		iNodoArbolPuntoOptimoNodoInternoPtr _padre) {

	size_t nroNodoHijo = 0;
	iNodoArbolPuntoOptimoPtr hijo = NULL;

	if ((nroNodoHijo = _padre->ObtenerHijoDerecho()) == 0)
		if ((nroNodoHijo = _padre->ObtenerHijoIzquierdo()) == 0)
			Throw(" ", "Nodo interno sin hijos.");

	hijo = (iNodoArbolPuntoOptimoPtr) archivo->LeerNodo(nroNodoHijo);

	if ((hijo->ObtenerTipoNodo() == eNodoArbolPuntoOptimo_Hoja)
			&& (archivo->DeterminarEstadoNodo(hijo)
					== eEstadoCargaNodo_CargaMinima)) {

		iNodoArbolPuntoOptimoPtr padreNuevo = Fusionar(_padre, nroNodoHijo,
				(iNodoArbolPuntoOptimoNodoHojaPtr) hijo);
		archivo->LiberarNodo(nroNodoHijo);
		Escribir(_nroNodoPadre, padreNuevo);

		padreNuevo->Dispose();
	}

	else {

		while (archivo->DeterminarEstadoNodo(_padre)
				== eEstadoCargaNodo_Underflow)
			_padre->AgregarRegistro(hijo->QuitarRegistro());

		if (archivo->DeterminarEstadoNodo(hijo) != eEstadoCargaNodo_Underflow) {

			Escribir(_nroNodoPadre, _padre);
			Escribir(nroNodoHijo, hijo);

		} else if (hijo->ObtenerTipoNodo() == eNodoArbolPuntoOptimo_Interno) {

			Escribir(_nroNodoPadre, _padre);
			ResolverUnderflow(nroNodoHijo,
					(iNodoArbolPuntoOptimoNodoInternoPtr) hijo);

		} else

			ResolverUnderflow(_nroNodoPadre, _padre, nroNodoHijo,
					(iNodoArbolPuntoOptimoNodoHojaPtr) hijo);
	}

	hijo->Dispose();
}

void VpTree_ABM::ResolverUnderflow(size_t _nroNodoPadre,
		iNodoArbolPuntoOptimoNodoInternoPtr _padre, size_t _nroNodoHijo,
		iNodoArbolPuntoOptimoNodoHojaPtr _hijo) {

	size_t nroNodoHermano = 0;
	size_t* pNroNodoHermano = &nroNodoHermano;
	iNodoArbolPuntoOptimoPtr pHermano = NULL;
	iNodoArbolPuntoOptimoPtr* ppHermano = &pHermano;

	if ((archivo->DeterminarPorcentajeCarga(_padre)
			+ archivo->DeterminarPorcentajeCarga(_hijo)) <= 100) {

		iNodoArbolPuntoOptimoPtr padreNuevo = Fusionar(_padre, _nroNodoHijo,
				_hijo);
		archivo->LiberarNodo(_nroNodoHijo);
		Escribir(_nroNodoPadre, padreNuevo);

		padreNuevo->Dispose();
	}

	else if (ObtenerHermano(_padre, _nroNodoHijo, pNroNodoHermano, ppHermano)
			== eHermanoVpTree_ABM__NodoInterno) {

		Escribir(_nroNodoPadre, (*ppHermano));
		archivo->LiberarNodo(_nroNodoHijo);
		archivo->LiberarNodo(nroNodoHermano);

		while (_padre->ObtenerCantidadRegistros())
			Alta(_padre->QuitarRegistro(), false);

		while (_hijo->ObtenerCantidadRegistros())
			Alta(_hijo->QuitarRegistro(), false);
	}

	else {

		iNodoArbolPuntoOptimoPtr padreNuevo = Fusionar(_padre, _nroNodoHijo,
				_hijo);
		archivo->LiberarNodo(_nroNodoHijo);

		if (nroNodoHermano) {

			iNodoArbolPuntoOptimoPtr padreNuevo2 = Fusionar(
					(iNodoArbolPuntoOptimoNodoInternoPtr) padreNuevo,
					nroNodoHermano,
					(iNodoArbolPuntoOptimoNodoHojaPtr) (*ppHermano));
			archivo->LiberarNodo(nroNodoHermano);

			padreNuevo->Dispose();
			padreNuevo = padreNuevo2;
		}

		if (archivo->DeterminarEstadoNodo(padreNuevo)
				== eEstadoCargaNodo_Overflow)
			ResolverOverflow(_nroNodoPadre,
					(iNodoArbolPuntoOptimoNodoHojaPtr) padreNuevo);
		else
			Escribir(_nroNodoPadre, padreNuevo);

		padreNuevo->Dispose();
	}

	if ((*ppHermano))
		(*ppHermano)->Dispose();
}

void VpTree_ABM::ResolverOverflow(size_t _nroNodoHijo,
		iNodoArbolPuntoOptimoNodoHojaPtr _hijo) {

}

iNodoArbolPuntoOptimoPtr VpTree_ABM::Fusionar(
		iNodoArbolPuntoOptimoNodoInternoPtr _padre, size_t _nroNodoHijo,
		iNodoArbolPuntoOptimoNodoHojaPtr _hijo) {

	iNodoArbolPuntoOptimoPtr padreNuevo = NULL;

	if (_nroNodoHijo == 0)
		Throw(" ", "La raiz no puede ser hija del padre.");

	if (_padre->ObtenerHijoDerecho() == _nroNodoHijo)
		_padre->EstablecerHijoDerecho(0);
	else if (_padre->ObtenerHijoIzquierdo() == _nroNodoHijo)
		_padre->EstablecerHijoIzquierdo(0);
	else
		Throw(" ", "No es hijo del padre");

	if (_padre->ObtenerHijoDerecho() == _padre->ObtenerHijoIzquierdo())
		padreNuevo = NodoArbolPuntoOptimoFactory_Nuevo(
				eNodoArbolPuntoOptimo_Hoja);
	else {

		padreNuevo = NodoArbolPuntoOptimoFactory_Nuevo(
				eNodoArbolPuntoOptimo_Interno);

		((iNodoArbolPuntoOptimoNodoInternoPtr) padreNuevo)->EstablecerPivote(
				_padre->ObtenerPivote());
		((iNodoArbolPuntoOptimoNodoInternoPtr) padreNuevo)->EstablecerRadio(
				_padre->ObtenerRadio());
		((iNodoArbolPuntoOptimoNodoInternoPtr) padreNuevo)->EstablecerHijoDerecho(
				_padre->ObtenerHijoDerecho());
		((iNodoArbolPuntoOptimoNodoInternoPtr) padreNuevo)->EstablecerHijoIzquierdo(
				_padre->ObtenerHijoIzquierdo());
	}

	while (_padre->ObtenerCantidadRegistros())
		padreNuevo->AgregarRegistro(_padre->QuitarRegistro());

	while (_hijo->ObtenerCantidadRegistros())
		padreNuevo->AgregarRegistro(_hijo->QuitarRegistro());

	return padreNuevo;
}

eHermanoVpTree_ABM VpTree_ABM::ObtenerHermano(
		iNodoArbolPuntoOptimoNodoInternoPtr _padre, size_t _nroNodoHijo,
		size_t* _nroNodoHermano, iNodoArbolPuntoOptimoPtr* _hermano) {

	if (_nroNodoHijo == 0)
		Throw(" ", "La raiz no puede ser hija del padre.");

	if (_padre->ObtenerHijoDerecho() == _nroNodoHijo)
		*_nroNodoHermano = _padre->ObtenerHijoIzquierdo();
	else
		*_nroNodoHermano = _padre->ObtenerHijoDerecho();

	if (*_nroNodoHermano == 0) {

		*_hermano = NULL;
		return eHermanoVpTree_ABM__Inexistente;
	}

	*_hermano = (iNodoArbolPuntoOptimoPtr) archivo->LeerNodo(*_nroNodoHermano);

	if ((*_hermano)->ObtenerTipoNodo() == eNodoArbolPuntoOptimo_Hoja)
		return eHermanoVpTree_ABM__Hoja;
	else
		return eHermanoVpTree_ABM__NodoInterno;
}

iFeaturePtr VpTree_ABM::GenerarPivote(iNodoArbolPuntoOptimoNodoHojaPtr _hoja) {
	size_t cantidad = _hoja->ObtenerCantidadRegistros();
	if (!cantidad)
		return NULL;

	iFeaturePtr pivote = NULL;

	iFeaturePtr clave = _hoja->ObtenerRegistro(0)->GetFeature(
			this->nroCampoClave);
	if (clave->ObtenerTipo() & Mascara_Numero) {
		size_t listaClaves[cantidad];

		for (size_t i = 0; i < cantidad; i++)
			listaClaves[i] =
					_hoja->ObtenerRegistro(i)->GetFeature(0)->AsNumber().entero.enteroSinSigno.entero32SinSigno;

		uValue number;
		number.primitivo.numero.entero.enteroSinSigno.entero32SinSigno =
				this->espacioMetrico->CalcularPivote(listaClaves, cantidad);
		pivote = FeatureFactory_Nuevo(number, eValueType::eValueType_U4);
	} else {
		sCadenaANSI *listaClaves[cantidad];

		for (size_t i = 0; i < cantidad; i++)
			listaClaves[i] =
					_hoja->ObtenerRegistro(i)->GetFeature(0)->AsCadenaANSI();

		sCadenaANSI *cadena = this->espacioMetrico->CalcularPivote(listaClaves,
				cantidad);
		pivote = FeatureFactory_Nuevo(cadena);
		if (cadena->cadena)
			delete[] cadena->cadena;
		delete cadena;
	}

	return pivote;
}

float VpTree_ABM::CalcularRadio(iFeaturePtr _pivote,
		iNodoArbolPuntoOptimoNodoHojaPtr _hoja) {

	// Que te ayude mandrake :P
	iHeapPtr heapMinimos = HeapFactory_Nuevo(eHeap::eHeap_Minimo);

	size_t cantidadRegistros = _hoja->ObtenerCantidadRegistros();

	// insertamos en orden todos los registros, luego quitamos la mitad
	// y nos quedamos con el radio igual a la distancia del siguiente en el heap

	for (size_t i = 0; i < cantidadRegistros; i++) {
		sHeapComponentPtr heapComponent = new sHeapComponent();
		iRegistroPtr registro = _hoja->ObtenerRegistro(i);
		heapComponent->valor = this->Distancia(_pivote,
				registro->GetFeature(this->nroCampoClave));
		heapComponent->object = registro;

		heapMinimos->Push(heapComponent);
	}

	for (size_t i = 0; i < cantidadRegistros / 2; i++)
		delete heapMinimos->Pop();

	float radio = heapMinimos->Peek()->valor;
	delete heapMinimos;	// borra el resto

	return radio;
}

eResultadoVpTree_ABM VpTree_ABM::Alta(iRegistroPtr _reg, size_t _nroNodoPadre,
		iNodoArbolPuntoOptimoPtr _padre) {

	_padre->AgregarRegistro(_reg);

	if (archivo->DeterminarEstadoNodo(_padre) != eEstadoCargaNodo_Overflow)
		Escribir(_nroNodoPadre, _padre);

	else if (_padre->ObtenerTipoNodo() == eNodoArbolPuntoOptimo_Hoja)
		ResolverOverflow(_nroNodoPadre,
				(iNodoArbolPuntoOptimoNodoHojaPtr) _padre);

	else {

		iFeaturePtr key;
		iFeaturePtr pivote;
		size_t nroNodoHijo;
		iNodoArbolPuntoOptimoPtr hijo;

		pivote =
				((iNodoArbolPuntoOptimoNodoInternoPtr) _padre)->ObtenerPivote();
		key = _reg->GetFeature(nroCampoClave);

		if (Distancia(key, pivote)
				< ((iNodoArbolPuntoOptimoNodoInternoPtr) _padre)->ObtenerRadio())
			nroNodoHijo =
					((iNodoArbolPuntoOptimoNodoInternoPtr) _padre)->ObtenerHijoIzquierdo();
		else
			nroNodoHijo =
					((iNodoArbolPuntoOptimoNodoInternoPtr) _padre)->ObtenerHijoDerecho();

		hijo = (iNodoArbolPuntoOptimoPtr) archivo->LeerNodo(nroNodoHijo);
		Alta(_reg, nroNodoHijo, hijo);

		key->Dispose();
		pivote->Dispose();
		hijo->Dispose();
	}

	return eResultadoVpTree_ABM__Ok;
}

eResultadoVpTree_ABM VpTree_ABM::Alta(iRegistroPtr _reg, bool _unicidad) {

	if (_unicidad)
		if (Buscar(_reg->GetFeature(this->nroCampoClave), NULL) == eResultadoVpTree_ABM__Ok)
			return eResultadoVpTree_ABM__Duplicado;

	iNodoArbolPuntoOptimoPtr copiaRaiz = raiz->Clone();
	eResultadoVpTree_ABM res = Alta(_reg, 0, copiaRaiz);

	copiaRaiz->Dispose();
	return res;
}

size_t* VpTree_ABM::Ubicar(iFeaturePtr _key, iNodoArbolPuntoOptimoPtr _nodo) {

	size_t* i;
	float dist;
	iFeaturePtr key;
	iRegistroPtr reg;

	i = new size_t;
	*i = 0;

	while (*i < _nodo->ObtenerCantidadRegistros()) {

		reg = _nodo->ObtenerRegistro(*i);
		key = reg->GetFeature(nroCampoClave);
		dist = Distancia(key, _key);

		reg->Dispose();
		key->Dispose();

		if (!dist)
			return i;

		(*i)++;
	}

	delete i;
	return NULL;
}

eResultadoVpTree_ABM VpTree_ABM::Baja(iFeaturePtr _key, size_t _nroNodoPadre,
		iNodoArbolPuntoOptimoPtr _padre, size_t _nroNodoHijo,
		iNodoArbolPuntoOptimoPtr _hijo) {

	size_t* i = Ubicar(_key, _hijo);

	if (!i)

		if (_hijo->ObtenerTipoNodo() == eNodoArbolPuntoOptimo_Hoja)
			return eResultadoVpTree_ABM__Inexistente;
		else {

			iFeaturePtr pivote;
			size_t nroNodoNieto;
			eResultadoVpTree_ABM res;
			iNodoArbolPuntoOptimoPtr nieto;

			pivote =
					((iNodoArbolPuntoOptimoNodoInternoPtr) _hijo)->ObtenerPivote();

			if (Distancia(_key, pivote)
					< ((iNodoArbolPuntoOptimoNodoInternoPtr) _hijo)->ObtenerRadio())
				nroNodoNieto =
						((iNodoArbolPuntoOptimoNodoInternoPtr) _hijo)->ObtenerHijoIzquierdo();
			else
				nroNodoNieto =
						((iNodoArbolPuntoOptimoNodoInternoPtr) _hijo)->ObtenerHijoDerecho();

			nieto = (iNodoArbolPuntoOptimoPtr) archivo->LeerNodo(nroNodoNieto);
			res = Baja(_key, _nroNodoHijo, _hijo, nroNodoNieto, nieto);

			nieto->Dispose();
			pivote->Dispose();
			return res;
		}

	(_hijo->QuitarRegistro(*i))->Dispose();
	delete i;

	if ((archivo->DeterminarEstadoNodo(_hijo) == eEstadoCargaNodo_Underflow)
			&& (_hijo->ObtenerTipoNodo() == eNodoArbolPuntoOptimo_Interno))
		ResolverUnderflow(_nroNodoHijo,
				(iNodoArbolPuntoOptimoNodoInternoPtr) _hijo);

	else if ((archivo->DeterminarEstadoNodo(_hijo) == eEstadoCargaNodo_Underflow)
			&& (_hijo->ObtenerTipoNodo() == eNodoArbolPuntoOptimo_Hoja)
			&& _nroNodoHijo)
		ResolverUnderflow(_nroNodoPadre,
				(iNodoArbolPuntoOptimoNodoInternoPtr) _padre, _nroNodoHijo,
				(iNodoArbolPuntoOptimoNodoHojaPtr) _hijo);
	else
		Escribir(_nroNodoHijo, _hijo);

	return eResultadoVpTree_ABM__Ok;
}

eResultadoVpTree_ABM VpTree_ABM::Baja(const iFeaturePtr _key) {

	iNodoArbolPuntoOptimoPtr copiaRaiz = raiz->Clone();
	eResultadoVpTree_ABM res = Baja(_key, -1, NULL, 0, copiaRaiz);

	copiaRaiz->Dispose();
	return res;
}

eResultadoVpTree_ABM VpTree_ABM::Modificacion(const iRegistroPtr _reg) {

	eResultadoVpTree_ABM res = eResultadoVpTree_ABM__Ok;
	iFeaturePtr key = _reg->GetFeature(nroCampoClave);

	if (Baja(key) == eResultadoVpTree_ABM__Inexistente)
		res = eResultadoVpTree_ABM__Inexistente;
	else
		Alta(_reg, false);

//	El manejo de memoria de la clave es responabilidad de _reg
//	key->Dispose();
	return res;
}

eResultadoVpTree_ABM VpTree_ABM::Buscar(const iFeaturePtr _key, iRegistroPtr *_reg)
{
	*_reg = NULL;
	return eResultadoVpTree_ABM__Ok;
}
