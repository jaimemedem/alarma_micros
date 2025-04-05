#ifndef TECLADO_H
#define TECLADO_H

#include <stdint.h>

// Inicializa el teclado matricial
void initKeypad(void);

// Lee una tecla (retorna carácter '0'-'9', '*', '#', o 0 si ninguna)
char leerTecla(void);

#endif
