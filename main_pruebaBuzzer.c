#include <xc.h>
#include "buzzer.h"

#include <stdio.h>
#include <stdlib.h>

#define PIN_PULSADOR 5

int main(void) {
    int  pulsador;
    ANSELB = 0;
    TRISC = 0;
    LATC = 0xFFFE;
    initBuzzer();

    TRISB = 1 << PIN_PULSADOR;
    while (1) {
        pulsador = (PORTB >> PIN_PULSADOR) & 1;
        if (pulsador == 0) {
            activarBuzzer();
        }else{
            apagarBuzzer();
        }
        
    }

}
