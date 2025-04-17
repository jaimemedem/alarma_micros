#include <xc.h>
#include <string.h>
#include "Pic32Ini.h"
#include "UartCol.h"    // InicializarUART1, putsUART, putcUART
#include "teclado.h"    // teclado_init(), teclado_waitKey()
#include "buzzer.h"     // inicializar_bocina(), sonar(), parar_bocina()
#include "TimerUtils.h" // esperar_ms()

#define SEQ_LEN 4

static const char SEQ_ON [] = "1134";
static const char SEQ_OFF[] = "6779";

int main(void) {
    // Desactivar JTAG en tiempo de ejecución (si lo necesitas)
    DDPCONbits.JTAGEN = 0;

    // Inicializaciones
    InicializarUART1(9600);
    INTCONbits.MVEC = 1;
    __builtin_enable_interrupts();

    putsUART("\r\n== Prueba Teclado + Buzzer ==\r\n");
    putsUART("Pulse 1,2,3,4 para ENCENDER buzzer\r\n");
    putsUART("Pulse 6,7,8,9 para APAGAR buzzer\r\n");

    teclado_init();
    inicializar_bocina();
    parar_bocina();

    char buffer[SEQ_LEN+1];
    int idx = 0;

    while (1) {
        char tecla = teclado_waitKey();
        putcUART(tecla);

        buffer[idx++] = tecla;
        if (idx >= SEQ_LEN) {
            buffer[SEQ_LEN] = '\0';

            if (strcmp(buffer, SEQ_ON) == 0) {
                putsUART(" ? SECUENCIA ON detectada: BUZZER ON\r\n");
                sonar();
            }
            else if (strcmp(buffer, SEQ_OFF) == 0) {
                putsUART(" ? SECUENCIA OFF detectada: BUZZER OFF\r\n");
                parar_bocina();
            }

            // Desplazar para permitir detección solapada
            memmove(buffer, buffer+1, SEQ_LEN-1);
            idx = SEQ_LEN-1;
        }
    }
    return 0;
}
