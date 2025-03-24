#ifndef UART_H
#define UART_H


#include <xc.h>

#define PIN_U1RX 13 // U1RX se conecta a RB13
#define PIN_U1TX 7  // U1TX se conecta a RB7
#define TAM_COLA 100 // Tamaño del buffer de transmisión y recepción

// Estructura para manejar colas de transmisión y recepción
typedef struct {
    int icabeza;
    int icola;
    char cola[TAM_COLA];
} cola_t;

// Declaraciones de funciones
void InicializarUART1(int baudios);
void InterrupcionUART1(void);
char getcUART(void);
void putcUART(char c);
void putsUART(char s[]);

#endif // UART_H