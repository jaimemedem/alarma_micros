// gestionUART.c
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UartCol.h"
#include "Pic32Ini.h"
#include "gestionUART.h"

#define PASSWD_LEN 4

// Variables internas del m�dulo UART
static char comando[CMD_SIZE];
static int  comandoIndex = 0;

// Variable de estado accesible desde fuera
int estadoSistema = 0;   // 0=OFF/standby, 1=ON/armado
int desactivacion  = 0;  // flag de desactivaci�n por UART

// ** Nueva contrase�a din�micamente modificable **
static char uartPassword[PASSWD_LEN+1] = "1234";

void inicializarUARTySistema(void) {
    InicializarUART1(9600);
    INTCONbits.MVEC = 1;
    asm("ei");
    putsUART("\r\nSistema listo. Envie ON/OFF o PASSxxxx:\r\n");
}

void manejarUART(void) {
    char c = getcUART();
    if (c == '\0') return;

    putcUART(c);  // eco

    if (c >= ' ' && c <= '~') {
        if (comandoIndex < CMD_SIZE-1) {
            comando[comandoIndex++] = c;
        }
    }
    else if ((c == '\n' || c == '\r') && comandoIndex > 0) {
        comando[comandoIndex] = '\0';
        putsUART("\r\nComando recibido: ");
        putsUART(comando);
        putsUART("\r\n");
        procesarComando();
        comandoIndex = 0;
    }
}

void procesarComando(void) {
    // Pasa a may�sculas
    for (int i = 0; comando[i]; i++) {
        if (comando[i] >= 'a' && comando[i] <= 'z')
            comando[i] -= 32;
    }

    // 1) PASSxxxx (solo en standby: estadoSistema==0)
    if (strncmp(comando, "PASS", 4) == 0 && strlen(comando) == 4 + PASSWD_LEN) {
        if (estadoSistema == 0) {
            // copia los 4 d�gitos
            memcpy(uartPassword, comando+4, PASSWD_LEN);
            uartPassword[PASSWD_LEN] = '\0';
            putsUART("Nueva contrase�a: ");
            putsUART(uartPassword);
            putsUART("\r\n");
        } else {
            putsUART("Solo en standby se puede cambiar contrase�a.\r\n");
        }
        return;
    }

    // 2) ON / OFF
    if (strcmp(comando, "ON") == 0) {
        estadoSistema = 1;
        putsUART("SISTEMA ACTIVADO (ON)\r\n");
        return;
    }
    if (strcmp(comando, "OFF") == 0) {
        estadoSistema = 0;
        putsUART("SISTEMA DESACTIVADO (OFF)\r\n");
        return;
    }

    // 3) Contrase�a UART din�mica
    if (strcmp(comando, uartPassword) == 0) {
        desactivacion = 1;
        estadoSistema = 0;
        putsUART("Contrase�a correcta. SISTEMA DESACTIVADO\r\n");
        return;
    }

    // 4) Comando no v�lido
    putsUART("COMANDO NO V�LIDO (usar ON/OFF o PASSxxxx o contrase�a)\r\n");
}
