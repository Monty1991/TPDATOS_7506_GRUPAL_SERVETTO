#include "Sistema/Sistema/Headers/Sistema.h"
#include "Aplicacion/Headers/Aplicacion.h"

int main(int argc, char **argv)
{
	Sistema_Inicializar();
	
	Aplicacion *aplicacion = new Aplicacion();
	
	int i = 0;
	i = aplicacion->main(argc, argv);

	delete aplicacion;

	Sistema_Finalizar();
	return i;
}
