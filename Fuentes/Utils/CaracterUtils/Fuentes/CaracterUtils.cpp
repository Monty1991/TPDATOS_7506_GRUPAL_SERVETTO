#include "../Headers/CaracterUtils.h"

bool big_endian() {

	int entero = 1;
	char* p = (char*) &entero;
	return p[0] == 0x00;
}

bool isDigit(const char& _c) {

	switch (_c) {

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return true;

	default:
		return false;
	}
}

unsigned char getDigit(const char& _c) {

	switch (_c) {

	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	}
}

