// teclado.c

#include <xc.h>
#include <stdint.h>
#include "teclado.h"
#include "TimerUtils.h"  // para esperar_ms()

// -- Pin mappings --
#define COL1_LAT    LATBbits.LATB2
#define COL2_LAT    LATCbits.LATC5
#define COL3_LAT    LATAbits.LATA0

#define COL1_TRIS   TRISBbits.TRISB2
#define COL2_TRIS   TRISCbits.TRISC5
#define COL3_TRIS   TRISAbits.TRISA0

// Lectura de filas RC6..RC9 ? bits 0..3
#define ROWS_PORT   ((PORTC >> 6) & 0x0F)

// Mapa de teclas filas×columnas
static const char mapaTeclas[4][3] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'A','0','B'}
};

// Tiempo de antirrebote (ms)
#define DEBOUNCE_MS 20

void teclado_init(void) {
    // Deshabilitar funciones analógicas en pines usados
    ANSELB &= ~(1 << 2);                   // RB2 (columna 1)
    ANSELC &= (uint16_t)~((1 << 5)         // RC5 (columna 2)
                       | (1 << 6)         // RC6?RC9 (filas)
                       | (1 << 7)
                       | (1 << 8)
                       | (1 << 9));
    ANSELA &= ~(1 << 0);                   // RA0 (columna 3)

    // Columnas como salidas, nivel alto (inactivo)
    COL1_TRIS = 0; COL1_LAT = 1;
    COL2_TRIS = 0; COL2_LAT = 1;
    COL3_TRIS = 0; COL3_LAT = 1;

    // Filas como entradas (pull?up externas)
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC8 = 1;
    TRISCbits.TRISC9 = 1;
}

// Escanea columnas y devuelve 0 si no hay tecla válida,
// o el carácter pulsado.
static char scanKey(void) {
    for (uint8_t col = 0; col < 3; col++) {
        // Poner todas las columnas en nivel alto (inactivo)
        COL1_LAT = COL2_LAT = COL3_LAT = 1;

        // Bajar solo la columna actual
        if      (col == 0) COL1_LAT = 0;
        else if (col == 1) COL2_LAT = 0;
        else               COL3_LAT = 0;

        __asm__ volatile("nop");
        __asm__ volatile("nop");

        uint8_t pressed = (~ROWS_PORT) & 0x0F;  // 1 = tecla pulsada
        if (pressed) {
            // Si más de una fila, ignorar
            if ((pressed & (pressed - 1)) == 0) {
                uint8_t row = __builtin_ctz(pressed);
                // Restaurar columnas antes de salir
                COL1_LAT = COL2_LAT = COL3_LAT = 1;
                return mapaTeclas[row][col];
            }
            break;
        }
    }
    // Restaurar columnas
    COL1_LAT = COL2_LAT = COL3_LAT = 1;
    return 0;
}

char teclado_getKey(void) {
    char k = scanKey();
    if (k) {
        esperar_ms(DEBOUNCE_MS);
        if (scanKey() == k) {
            // Esperar a que se suelte
            while (scanKey() == k);
            return k;
        }
    }
    return 0;
}

char teclado_waitKey(void) {
    char k;
    do {
        k = teclado_getKey();
    } while (!k);
    return k;
}
