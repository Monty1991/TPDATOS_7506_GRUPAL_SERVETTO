/* 
 * File:   MapaDeBitsFactory.h
 * Author: Monty
 *
 * Created on 19 de marzo de 2016, 01:25
 */

#ifndef MAPADEBITSFACTORY_H
#define	MAPADEBITSFACTORY_H

#include "Headers/iMapaDeBits.h"

iMapaDeBitsPtr MapaDeBitsFactory_Nuevo(const char *buff, size_t tamanioMapa);

#endif	/* MAPADEBITSFACTORY_H */

