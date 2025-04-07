#ifndef TECLADO_H
#define TECLADO_H

#include <stdint.h>

// Inicializa el teclado
void initKeypad(void);

// Devuelve la tecla pulsada o 0 si no hay ninguna
char leerTecla(void);

#endif
