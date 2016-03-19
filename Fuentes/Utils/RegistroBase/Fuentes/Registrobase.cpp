/* 
 * File:   RegistroBase.cpp
 * Author: Monty
 * 
 * Created on 17 de marzo de 2016, 12:37
 */

#include "../Headers/RegistroBase.h"
#include "../../../Exceptions/ExceptionFactory.h"
#include "../../StringUtils/Headers/StringUtils.h"

RegistroBase::RegistroBase(size_t cantidadCampos)
{
	this->cantidadCampos = cantidadCampos;
	this->campoValores = new uValue[cantidadCampos];
}

RegistroBase::~RegistroBase()
{
	if (this->campoValores)
		delete this->campoValores;
}

uValue RegistroBase::ObtenerCampo(size_t nroCampo)
{
	if (nroCampo >= this->cantidadCampos)
	{
		char msg[20];
		StringUtils_Concatenar(msg,"%d", nroCampo);
		Throw("ArrayIndexOutOfBoundsException", msg);
	}

	return this->campoValores[nroCampo];
}

void RegistroBase::ModificarCampo(size_t nroCampo, uValue valor)
{
	if (nroCampo >= this->cantidadCampos)
		return;

	this->campoValores[nroCampo] = valor;
}

void RegistroBase::Disponer()
{
	delete this;
}