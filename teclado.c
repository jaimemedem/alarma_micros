#include <xc.h>
#include "teclado.h"

// Pines usados para el teclado 4x3:
// Filas: RA0, RA1, RA2, RA3 (entradas)
// Columnas: RB0, RB1, RB2 (salidas)

#define NUM_FILAS 4
#define NUM_COLUMNAS 3

// Definición de pines y máscaras
#define MASCARA_FILAS   0x0F        // RA0 - RA3
#define MASCARA_COLUMNAS 0x07       // RB0 - RB2

#define PORT_FILAS      PORTA
#define TRIS_FILAS      TRISA
#define ANSEL_FILAS     ANSELA

#define LAT_COLUMNAS    LATB
#define TRIS_COLUMNAS   TRISB
#define ANSEL_COLUMNAS  ANSELB

// Mapa de teclas [fila][columna]
static const char mapaTeclas[NUM_FILAS][NUM_COLUMNAS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

void initKeypad(void) {
    // Configurar pines de filas como entradas digitales
    ANSEL_FILAS &= ~MASCARA_FILAS;
    TRIS_FILAS |= MASCARA_FILAS;

    // Configurar pines de columnas como salidas digitales
    ANSEL_COLUMNAS &= ~MASCARA_COLUMNAS;
    TRIS_COLUMNAS &= ~MASCARA_COLUMNAS;
    LAT_COLUMNAS |= MASCARA_COLUMNAS; // Inicialmente en alto
}

char leerTecla(void) {
    for (int col = 0; col < NUM_COLUMNAS; col++) {
        // Activar columna actual a 0, las otras a 1
        LAT_COLUMNAS |= MASCARA_COLUMNAS;
        LAT_COLUMNAS &= ~(1 << col);

        // Pequeña espera para estabilizar
        for (volatile int d = 0; d < 50; d++);

        // Leer filas
        for (int fila = 0; fila < NUM_FILAS; fila++) {
            if (!((PORT_FILAS >> fila) & 1)) {
                char tecla = mapaTeclas[fila][col];

                // Esperar a que se suelte (anti-rebote)
                while (!((PORT_FILAS >> fila) & 1));
                for (volatile int d = 0; d < 5000; d++); // debounce

                return tecla;
            }
        }
    }

    return 0; // Ninguna tecla detectada
}
