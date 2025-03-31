#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UartCol.h"
#include "Pic32Ini.h"
#include "gestionUART.h"

// Variables internas del módulo
static char comando[CMD_SIZE];
static int comandoIndex = 0;

// Variable de estado accesible desde fuera
int estadoSistema = 0; // Por defecto OFF

void inicializarUARTySistema(void) {
    // UART
    InicializarUART1(9600);

    // Habilitar interrupciones globales
    INTCONbits.MVEC = 1;
    asm("ei");

    putsUART("\r\nSistema listo. Envie ON/OFF:\r\n");
}

void manejarUART(void) {
    char c = getcUART();
    
    if (c != '\0') {
        putcUART(c);  // Eco

        if (c >= ' ' && c <= '~') {
            if (comandoIndex < (CMD_SIZE - 1)) {
                comando[comandoIndex++] = c;
            }
        } else if ((c == '\n' || c == '\r') && comandoIndex > 0) {
            comando[comandoIndex] = '\0';  // Termina cadena

            putsUART("\r\nComando recibido: ");
            putsUART(comando);
            putsUART("\r\n");

            procesarComando();
            comandoIndex = 0;
        }
    }
}

void procesarComando(void) {
    int i;

    // Convertir comando a mayúsculas
    for (i = 0; comando[i]; i++) {
        if (comando[i] >= 'a' && comando[i] <= 'z') {
            comando[i] -= 32;
        }
    }

    if (strcmp(comando, "ON") == 0) {
        estadoSistema = 1;
        putsUART("\r\nSISTEMA ACTIVADO (ON)\r\n");

    } else if (strcmp(comando, "OFF") == 0) {
        estadoSistema = 0;
        putsUART("\r\nSISTEMA DESACTIVADO (OFF)\r\n");

    } else {
        putsUART("\r\nCOMANDO NO VALIDO (usar ON/OFF)\r\n");
    }
}
