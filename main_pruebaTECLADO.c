#include <xc.h>
#include "teclado.h"
#include "UartCol.h"

#define LEDS_MASK 0x0F // RC0?RC3

void configurarLEDs(void);

int main(void) {
    initKeypad();
    configurarLEDs();
    InicializarUART1(9600); // UART a 9600 baudios

    putsUART("\r\nTeclado listo. Pulse una tecla:\r\n");

    while (1) {
        char tecla = leerTecla();

        if (tecla != 0) {
            // Apagar todos los LEDs
            LATC |= LEDS_MASK;

            // Encender LED correspondiente (opcional)
            switch (tecla) {
                case '1': LATC &= ~(1 << 0); break;
                case '2': LATC &= ~(1 << 1); break;
                case '3': LATC &= ~(1 << 2); break;
                case '4': LATC &= ~(1 << 3); break;
            }

            // Enviar la tecla por UART
            putsUART("Tecla pulsada: ");
            putcUART(tecla);
            putsUART("\r\n");
        }
    }

    return 0;
}

void configurarLEDs(void) {
    ANSELC &= ~LEDS_MASK;
    TRISC  &= ~LEDS_MASK;
    LATC   |= LEDS_MASK; // Apagar todos (activo bajo)
}
