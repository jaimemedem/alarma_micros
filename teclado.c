#include <xc.h>
#include "teclado.h"

// === Pines definidos por el usuario ===

// FILAS (entradas digitales): RC6 ? RC9
#define FILAS_PORT   PORTC
#define FILAS_TRIS   TRISC
#define FILAS_ANSEL  ANSELC
#define FILAS_MASK   ((1 << 6) | (1 << 7) | (1 << 8) | (1 << 9))

// COLUMNAS (salidas digitales): RB3, RB2, RA1
#define COL1_LAT LATB
#define COL1_TRIS TRISB
#define COL1_ANSEL ANSELB
#define COL1_BIT 3

#define COL2_LAT LATB
#define COL2_TRIS TRISB
#define COL2_ANSEL ANSELB
#define COL2_BIT 2

#define COL3_LAT LATA
#define COL3_TRIS TRISA
#define COL3_ANSEL ANSELA
#define COL3_BIT 1

// Mapa de teclas [fila][columna]
static const char mapaTeclas[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'A', '0', 'B'}
};

void initKeypad(void) {
    // Configurar filas como entradas digitales
    FILAS_ANSEL &= ~FILAS_MASK;
    FILAS_TRIS  |= FILAS_MASK;

    // Configurar columnas como salidas digitales
    COL1_ANSEL &= ~(1 << COL1_BIT);
    COL1_TRIS  &= ~(1 << COL1_BIT);
    COL1_LAT   |=  (1 << COL1_BIT);

    COL2_ANSEL &= ~(1 << COL2_BIT);
    COL2_TRIS  &= ~(1 << COL2_BIT);
    COL2_LAT   |=  (1 << COL2_BIT);

    COL3_ANSEL &= ~(1 << COL3_BIT);
    COL3_TRIS  &= ~(1 << COL3_BIT);
    COL3_LAT   |=  (1 << COL3_BIT);
}

char leerTecla(void) {
    // Columnas y sus LATs
    volatile unsigned int *lats[3] = { &COL1_LAT, &COL2_LAT, &COL3_LAT };
    uint8_t bits[3] = { COL1_BIT, COL2_BIT, COL3_BIT };

    for (int col = 0; col < 3; col++) {
        // Poner la columna actual en 0 y las otras en 1
        for (int i = 0; i < 3; i++) {
            if (i == col)
                *(lats[i]) &= ~(1 << bits[i]);
            else
                *(lats[i]) |= (1 << bits[i]);
        }

        // Espera para estabilizar
        for (volatile int d = 0; d < 100; d++);

        // Leer filas (RC6 a RC9 ? bits 6 a 9)
        for (int fila = 0; fila < 4; fila++) {
            if (!((FILAS_PORT >> (6 + fila)) & 1)) {
                // Anti-rebote: esperar a que se suelte
                while (!((FILAS_PORT >> (6 + fila)) & 1));
                for (volatile int d = 0; d < 5000; d++);

                return mapaTeclas[fila][col];
            }
        }
    }

    return 0;
}
