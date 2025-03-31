#ifndef PIR_H
#define PIR_H

#include <stdint.h>

// Número de bit correspondiente a RA7
#define LECTURAS_PIR 7

// Prototipos de funciones
void initPIR(void);
uint8_t leerPIR(void);

#endif // PIR_H
