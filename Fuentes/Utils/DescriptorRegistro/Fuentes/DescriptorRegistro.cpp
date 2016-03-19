/* 
 * File:   DescriptorRegistro.cpp
 * Author: Monty
 * 
 * Created on 18 de marzo de 2016, 03:47
 */

#include "../Headers/DescriptorRegistro.h"
#include "../../../Exceptions/ExceptionFactory.h"
#include "../../StringUtils/Headers/StringUtils.h"

DescriptorRegistro::DescriptorRegistro(eTipoCampo *valores, size_t cantidad)
{
	this->valores = valores;
	this->cantidad = cantidad;
}

DescriptorRegistro::~DescriptorRegistro()
{
	if (this->valores)
		delete this;
}

eTipoCampo DescriptorRegistro::ObtenerTipoCampo(size_t posicion)
{
	if (posicion >= this->cantidad)
	{
		char msg[20];
		StringUtils_Concatenar(msg,"%d", posicion);
		Throw("ArrayIndexOutOfBoundsException", msg);
	}

	return this->valores[posicion];
}

size_t DescriptorRegistro::ObtenerCantidadCampos()
{
	return this->cantidad;
}

void DescriptorRegistro::Dispose()
{
	delete this;
}
