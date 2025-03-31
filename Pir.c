#include <xc.h>
#include "Pir.h"

//El sensor se alimenta con 3,3V
//Tiene otra toma GND
//y una tercera toma de datos conectada a RA7

#define LECTURAS_PIR 7

void initPIR(void){
    ANSELA &= ~(1 << LECTURAS_PIR);
    TRISA |= (1 << LECTURAS_PIR);
}

uint8_t leerPIR(void){
    int lectura=(PORTA>>LECTURAS_PIR)&1;
    return lectura;
}
