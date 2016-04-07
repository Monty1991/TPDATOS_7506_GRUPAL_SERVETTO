/*
 * SerializadorByteMap.cpp
 *
 *  Created on: 06/04/2016
 *      Author: cristian
 */

#include <cstring>
#include "../Headers/SerializadorByteMap.h"
#include "../../CaracterUtils/Headers/CaracterUtils.h"

unsigned SerializadorByteMap::calcularEspacio(const iByteMapPtr _mapa) {

	if (_mapa->getConfiguracion() == eConfiguracion_file_blocks)
		return _mapa->getTamanio() * 2 + 2;
	else
		return _mapa->getTamanio() * 6 + 2;
}

unsigned SerializadorByteMap::serializar(const iByteMapPtr _mapa,
		char* _buffer) {

	unsigned usefulChars;
	unsigned cantBytes;
	unsigned long offset;
	char * cantBytesPtr = (char*) &cantBytes;
	char * offsetPtr = (char*) &offset;
	char * usefulCharsPtr = (char*) &usefulChars;

	if (big_endian()) {

		offsetPtr += (sizeof(unsigned long) - 4);
		cantBytesPtr += ((sizeof(unsigned)) - 2);
		usefulCharsPtr += ((sizeof(unsigned)) - 2);
	}

	usefulChars = calcularEspacio(_mapa);
	memcpy(_buffer, usefulCharsPtr, 2);
	_buffer += 2;

	if (_mapa->getConfiguracion() == eConfiguracion_file_blocks)

		for (unsigned i = 0; i < _mapa->getTamanio(); i++) {

			cantBytes = _mapa->getCantBytesLibres(i);
			memcpy(_buffer, cantBytesPtr, 2);
			_buffer += 2;
		}

	else {

		iByteMapPtr _copiaMapa = _mapa->clone();

		while (_copiaMapa->getTamanio() != 0) {

			offset = _copiaMapa->getReferencia(0);
			cantBytes = _copiaMapa->getCantBytesLibres(offset);
			_copiaMapa->upDate(offset, cantBytes);
			memcpy(_buffer, cantBytesPtr, 2);
			_buffer += 2;
			memcpy(_buffer, offsetPtr, 4);
			_buffer += 4;
		}

		delete _copiaMapa;
	}

	return usefulChars;
}

