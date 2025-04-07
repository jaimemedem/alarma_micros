#include <xc.h>
#include "buzzer.h"
#include <stdio.h>
#include <stdlib.h>

#define PIN_PULSADOR 5

int main(void) {
    int pulsador;

    // Configurar todos los pines de PORTB como digitales
    ANSELB = 0;

    // (Opcional) Configuración de PORTC para depuración u otros fines
    TRISC = 0;
    LATC = 0xFFFE;

    // Inicializar el buzzer
    inicializar_bocina();
    // Configurar solo el bit del pulsador sin sobrescribir otras configuraciones de TRISB
    TRISB |= (1<<PIN_PULSADOR); // RB5 como entrada (pulsador)

    while (1) {
        // Lee el estado del pulsador en RB5
        pulsador = (PORTB >> PIN_PULSADOR) & 1;
        if (pulsador == 0) {
            sonar();
        } else {
            parar_bocina();
        }
    }

    return 0;
}
