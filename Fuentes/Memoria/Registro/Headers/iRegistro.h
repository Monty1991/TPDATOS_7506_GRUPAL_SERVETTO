/* 
 * File:   iRegistro.h
 * Author: Monty
 *
 * Created on 21 de marzo de 2016, 05:57
 */

#ifndef IREGISTRO_H
#define	IREGISTRO_H

#include "../../Feature/Headers/iFeature.h"
#include "../../Object/Headers/iObject.h"

typedef class iRegistro: public iObject
{
	public:
		virtual size_t GetSize() = 0;
		
		virtual iFeaturePtr GetFeature(size_t posicion) = 0;
		virtual void SetFeature(size_t posicion, iFeaturePtr feature) = 0;

} *iRegistroPtr;

#endif	/* IREGISTRO_H */
