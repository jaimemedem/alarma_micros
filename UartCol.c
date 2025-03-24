#include <string.h>
#include <stdio.h>
#include <xc.h>
#include "Pic32Ini.h" // Archivo de configuración del PIC

#define PIN_U1RX 13 // U1RX se conecta a RB13
#define PIN_U1TX 7  // U1TX se conecta a RB7
#define TAM_COLA 100 // Tamaño del buffer de transmisión y recepción

// Estructura para manejar colas de transmisión y recepción
typedef struct {
    int icabeza;
    int icola;
    char cola[TAM_COLA];
} cola_t;

static cola_t cola_tx, cola_rx;

void InicializarUART1(int baudios) {
    if (baudios > 38400) {
        U1BRG = 5000000 / 4 / baudios - 1;
        U1MODEbits.BRGH = 1;
    } else {
        U1BRG = 5000000 / 16 / baudios - 1;
        U1MODEbits.BRGH = 0;
    }
    
    // Configuración de interrupciones UART
    IFS1bits.U1RXIF = 0; // Borra flag de recepción
    IEC1bits.U1RXIE = 1; // Habilita interrupciones de recepción
    IFS1bits.U1TXIF = 0; // Borra flag de transmisión
    IPC8bits.U1IP = 3;   // Prioridad 3
    IPC8bits.U1IS = 1;   // Subprioridad 1
    
    // Configuración de pines UART en RB13 y RB7
    ANSELB &= ~((1 << PIN_U1RX) | (1 << PIN_U1TX)); // Configura como digitales
    TRISB |= (1 << PIN_U1RX);  // Configura RB13 como entrada
    LATB |= (1 << PIN_U1TX);   // Mantiene RB7 en alto mientras no transmite
    
    // Desbloqueo de registros y remapeo de pines
    SYSKEY = 0xAA996655; 
    SYSKEY = 0x556699AA; 
    U1RXR = 3; // U1RX en RB13
    RPB7R = 1; // U1TX en RB7
    SYSKEY = 0x1CA11CA1; // Bloqueo de registros
    
    // Habilitación de UART
    U1STAbits.UTXISEL = 2;
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;
    U1MODE = 0x8000;

    // Inicialización de colas
    cola_tx.icabeza = 0;
    cola_tx.icola = 0;
    cola_rx.icabeza = 0;
    cola_rx.icola = 0;
}

// Interrupción UART1 corregida
__attribute__((vector(32), interrupt(IPL3SOFT), nomips16)) void InterrupcionUART1(void) {
    if (IFS1bits.U1RXIF == 1) { // Interrupción de recepción
        if ((cola_rx.icabeza + 1 == cola_rx.icola) ||
            (cola_rx.icabeza + 1 == TAM_COLA && cola_rx.icola == 0)) {
            // La cola está llena, se descarta el carácter
        } else {
            cola_rx.cola[cola_rx.icabeza] = U1RXREG; // Guarda el carácter recibido
            cola_rx.icabeza = (cola_rx.icabeza + 1) % TAM_COLA;
        }
        IFS1bits.U1RXIF = 0; // Borra flag de recepción
    }

    if (IFS1bits.U1TXIF == 1) { // Interrupción de transmisión
        if (cola_tx.icola != cola_tx.icabeza) { // Si hay datos en la cola
            U1TXREG = cola_tx.cola[cola_tx.icola]; // Envía el carácter
            cola_tx.icola = (cola_tx.icola + 1) % TAM_COLA;
        } else {
            IEC1bits.U1TXIE = 0; // Deshabilita la interrupción si no hay más datos
        }
        IFS1bits.U1TXIF = 0; // Borra flag de transmisión
    }
}

// Obtiene un carácter de la cola de recepción
char getcUART(void) {
    char c = '\0';
    if (cola_rx.icola != cola_rx.icabeza) { // Si hay datos en la cola
        c = cola_rx.cola[cola_rx.icola];
        cola_rx.icola = (cola_rx.icola + 1) % TAM_COLA;
    }
    return c;
}

// Envía un carácter a la UART
void putcUART(char c) {
    if ((cola_tx.icabeza + 1 != cola_tx.icola) &&
        !(cola_tx.icabeza + 1 == TAM_COLA && cola_tx.icola == 0)) {
        cola_tx.cola[cola_tx.icabeza] = c;
        cola_tx.icabeza = (cola_tx.icabeza + 1) % TAM_COLA;
    }
    IEC1bits.U1TXIE = 1; // Habilita la interrupción de transmisión
}

// Envía una cadena de caracteres a la UART
void putsUART(char s[]) {
    while (*s != '\0') {
        if ((cola_tx.icabeza + 1 == cola_tx.icola) ||
            (cola_tx.icabeza + 1 == TAM_COLA && cola_tx.icola == 0)) {
            break; // La cola está llena, aborta la transmisión
        }
        cola_tx.cola[cola_tx.icabeza] = *s++;
        cola_tx.icabeza = (cola_tx.icabeza + 1) % TAM_COLA;
    }
    IEC1bits.U1TXIE = 1; // Habilita la interrupción de transmisión
}
