#include <xc.h>
#include <stdint.h>
#include "Pir.h"
#include "UartCol.h"
#include "Pic32Ini.h"

#define LED_RC4_MASK (1 << 4)

void configurarIO(void);

int main(void) {
    configurarIO();
    initPIR();
    InicializarUART1(9600); // UART a 9600 baudios

    putsUART("\r\nSistema PIR iniciado.\r\n");

    int estadoAnterior = -1;

    while (1) {
        int estadoActual = leerPIR();

        if (estadoActual != estadoAnterior) {
            if (estadoActual) {
                LATC &= ~0x1;
                putsUART("Movimiento detectado\r\n");
            } else {
                LATC |= 0x1;
                putsUART("Sin movimiento\r\n");
            }

            estadoAnterior = estadoActual;
        }
    }

    return 0;
}

void configurarIO(void) {
    // Configura RC4 como salida digital (LED)
    ANSELC &= ~LED_RC4_MASK;
    TRISC &= ~LED_RC4_MASK;
    LATC |= LED_RC4_MASK;  // LED apagado inicialmente

    // Habilitar interrupciones (por si UART las usa)
    INTCONbits.MVEC = 1;
    asm("ei");
}
