#include "gestionUART.h"
#include <xc.h>

#define LED_MASK (1 << 0) // RC0

int main(void) {
    inicializarUARTySistema();

    // Configuración de RC0 como salida digital (LED)
    ANSELC &= ~LED_MASK;
    TRISC &= ~LED_MASK;
    LATC |= LED_MASK; // LED apagado (activo bajo)

    while (1) {
        manejarUART();

        if (estadoSistema) {
            LATC &= ~LED_MASK; // Encender LED (activo bajo)
        } else {
            LATC |= LED_MASK;  // Apagar LED
        }
    }

    return 0;
}
