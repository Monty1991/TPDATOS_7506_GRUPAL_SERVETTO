/* 
 * File:   TraceEntry.h
 * Author: Monty
 *
 * Created on 17 de marzo de 2016, 13:49
 */

#ifndef TRACEENTRY_H
#define	TRACEENTRY_H

#include "iTraceEntry.h"

class TraceEntry: public iTraceEntry
{
	private:
		char *filename;
		size_t numeroLinea;
		char *functionName;

	public:
		TraceEntry(const char *filename, size_t numeroLinea, const char *nombreMetodo);
		virtual ~TraceEntry();

		const char *ObtenerNombreArchivo();
		size_t ObtenerNumeroLinea();
		const char *ObtenerNombreMetodo();

		char *ObtenerAsString();

		virtual TraceEntry *Clone();
		virtual void Dispose();
		
	private:

};

#endif	/* TRACEENTRY_H */
