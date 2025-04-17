// main.c
#include <xc.h>
#include "Pic32Ini.h"
#include "UartCol.h"    // InicializarUART1, putsUART, putcUART
#include "teclado.h"

int main(void) {
    InicializarUART1(9600);
    INTCONbits.MVEC = 1;
    __builtin_enable_interrupts();
    putsUART("\r\nTeclado 4x3 listo. Pulse una tecla:\r\n");

    teclado_init();

    while (1) {
        char tecla = teclado_waitKey();
        putsUART("Tecla: ");
        putcUART(tecla);
        putsUART("\r\n");
    }
    return 0;
}
