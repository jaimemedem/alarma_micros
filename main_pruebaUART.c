#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "UartCol.h"
#include "Pic32Ini.h"

#define LED_MASK    0x0001
#define CMD_SIZE    20

// Variables globales
char comando[CMD_SIZE];
int comandoIndex = 0;

// Prototipos de funciones
void inicializarSistema(void);
void procesarComando(void);
void manejarUART(void);

int main(void) {
    inicializarSistema();
    putsUART("\r\nSistema listo. Envie ON/OFF:\r\n");
    
    while(1) {
        manejarUART();
    }
}

void inicializarSistema(void) {
    // Configuración hardware
    ANSELC &= ~LED_MASK;
    TRISC &= ~LED_MASK;
    LATC |= LED_MASK;  // LED apagado (asumimos que es activo bajo)
    
    // Configuración UART
    InicializarUART1(9600);
    INTCONbits.MVEC = 1;
    asm(" ei");
}

void manejarUART(void) {
    char c = getcUART();
    
    if(c != '\0') {
        // Eco del carácter recibido
        putcUART(c);
        
        // Solo caracteres imprimibles se almacenan
        if(c >= ' ' && c <= '~') {
            if(comandoIndex < (CMD_SIZE - 1)) {
                // Almacena el carácter y luego incrementa comandoIndex
                comando[comandoIndex++] = c;
            }
        }
        // Procesa el comando al recibir Enter (ya sea \n o \r)
        else if((c == '\n' || c == '\r') && comandoIndex > 0) {
            comando[comandoIndex] = '\0'; // Finaliza la cadena
            
            // Eco del comando completo
            putsUART("\r\nComando recibido: ");
            putsUART(comando);
            putsUART("\r\n");
            
            procesarComando();
            comandoIndex = 0;
        }
    }
}

void procesarComando(void) {
    char respuesta[50];
    int i;
    
    // Convertir todo el comando a mayúsculas para evitar problemas de comparación
    for(i = 0; comando[i]; i++) {
        if(comando[i] >= 'a' && comando[i] <= 'z') {
            comando[i] -= 32;
        }
    }
    
    if(strcmp(comando, "ON") == 0) {
        LATC &= ~LED_MASK;  // Enciende LED (activo bajo)
        strcpy(respuesta, "\r\nLED ENCENDIDO\r\n");
    }
    else if(strcmp(comando, "OFF") == 0) {
        LATC |= LED_MASK;   // Apaga LED (activo bajo)
        strcpy(respuesta, "\r\nLED APAGADO\r\n");
    }
    else {
        strcpy(respuesta, "\r\nCOMANDO NO VALIDO (usar ON/OFF)\r\n");
    }
    
    putsUART(respuesta);
}