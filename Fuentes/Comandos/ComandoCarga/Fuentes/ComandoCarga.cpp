#include "../Headers/ComandoCarga.h"
#include "../../../TDA/VpTree_ABM/VpTree_ABMFactory.h"
#include "../../../io/Archivo/ArchivoFactory.h"
#include "../../../Memoria/DescriptorRegistro/DescriptorRegistroFactory.h"
#include "../../../Memoria/Registro/RegistroFactory.h"
#include "../../../Sistema/Sistema/Headers/Sistema.h"
#include "../../../Exceptions/ExceptionFactory.h"

ComandoCarga::ComandoCarga()
{

}

ComandoCarga::~ComandoCarga()
{

}

void ComandoCarga::Dispose()
{
	delete this;
}

const char *ComandoCarga::ObtenerNombre()
{
	return "cargar";
}

void ComandoCarga::ImprimirAyuda(FILE *salida)
{
	fprintf(salida, "%s: n t f i\n", this->ObtenerNombre());
	fprintf(salida, "\tn: nombre archivo de datos\n");
	fprintf(salida, "\tt: tamanio de bloque, en bytes\n");
	fprintf(salida, "\tf: el archivo de datos csv, con el schema en primer fila\n");
	fprintf(salida, "\ti: nro de campo que contiene la clave\n");
}

size_t ObtenerSiguienteLinea(iArchivoPtr archivoTexto, char *linea, size_t tamanioBloque)
{
	size_t largo = 0;
	
	int c = archivoTexto->ReadChar();
	while ((c != EOF) && (largo < tamanioBloque))
	{
		if (c == '\n')
			break;

		linea[largo++] = c;
		c = archivoTexto->ReadChar();
	}

	linea[largo] = '\0';

	return largo;
}

void ComandoCarga::Ejecutar(FILE *salida, const char **listaParametros, size_t cantidad)
{
	if (cantidad < 4)
	{
		this->ImprimirAyuda(salida);
		return;
	}

	size_t tamanioBloque = strtoull(listaParametros[1], NULL, 0);
	size_t nroCampo = strtoull(listaParametros[3], NULL, 0);

	iArchivoPtr archivoCSV = ArchivoFactory_Nuevo(listaParametros[2], eTipoArchivo::eTipoArchivo_Texto);

	iVpTree_ABMPtr vpTree = NULL;
	
	Sistema_Execute(vpTree = VpTree_ABMFactory_Nuevo(listaParametros[0], nroCampo, tamanioBloque, (tamanioBloque * 3) /10, 16););

	size_t largo = 0;
	char linea[tamanioBloque];

	largo = ObtenerSiguienteLinea(archivoCSV, linea, tamanioBloque);

	iDescriptorRegistroPtr descriptorRegistro = NULL;
	
	Sistema_Execute(descriptorRegistro = DescriptorRegistroFactory_Nuevo(linea););

	largo = ObtenerSiguienteLinea(archivoCSV, linea, tamanioBloque);
	while (largo != 0)	// o sea, hasta la primer linea en blanco (o fin de archivo)
	{
		iRegistroPtr registro = NULL;
		
		TryCatchBlock(
		Sistema_Execute(registro = RegistroFactory_Nuevo(descriptorRegistro, linea););,
		[&](iExceptionPtr e){
			Sistema_Log(e);
			break;	// una linea que no contiene un registro
		}
		)

		eResultadoVpTree_ABM resultado;
		Sistema_Execute(resultado = vpTree->Alta(registro, true););			//insertamos el primero que aparezca
		if (resultado != eResultadoVpTree_ABM::eResultadoVpTree_ABM__Ok)	// borramos los siguientes
			registro->Dispose();
		
		largo = ObtenerSiguienteLinea(archivoCSV, linea, tamanioBloque);
	}
	descriptorRegistro->Dispose();

	archivoCSV->Close();
	vpTree->Dispose();
}
