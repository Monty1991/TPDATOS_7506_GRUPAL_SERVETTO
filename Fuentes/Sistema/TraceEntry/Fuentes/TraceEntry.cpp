/* 
 * File:   TraceEntry.cpp
 * Author: Monty
 * 
 * Created on 17 de marzo de 2016, 13:49
 */

#include "../Headers/TraceEntry.h"
#include "../../../Utils/StringUtils/Headers/StringUtils.h"

TraceEntry::TraceEntry(const char *filename, int numeroLinea, const char *functionName)
{
	this->filename = StringUtils_Clonar(filename);
	this->functionName = StringUtils_Clonar(functionName);

	this->numeroLinea = numeroLinea;
}

TraceEntry::~TraceEntry()
{
	if (this->filename)
		delete this->filename;
}

const char *TraceEntry::ObtenerNombreArchivo()
{
	return this->filename;
}

int TraceEntry::ObtenerNumeroLinea()
{
	return this->numeroLinea;
}

const char *TraceEntry::ObtenerNombreMetodo()
{
	return this->functionName;
}

char *TraceEntry::ObtenerAsString()
{
	char x[1000];
	StringUtils_Concatenar(x, "at [%s] on (%s:%d)", this->ObtenerNombreMetodo(), this->ObtenerNombreArchivo(), this->ObtenerNumeroLinea());
	return StringUtils_Clonar(x);
}

TraceEntry *TraceEntry::Clone()
{
	return new TraceEntry(this->ObtenerNombreArchivo(), this->ObtenerNumeroLinea(), this->ObtenerNombreMetodo());
}

void TraceEntry::Dispose()
{
	delete this;
}