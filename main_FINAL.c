// main.c
#include <xc.h>
#include <string.h>
#include "Pic32Ini.h"      // PIC setup
#include "gestionUART.h"   // UART command handling
#include "UartCol.h"       // putsUART, putcUART
#include "Pir.h"           // initPIR, leerPIR
#include "Servo.h"         // inicializaServo, abrirPuerta, cerrarPuerta
#include "buzzer.h"        // inicializar_bocina, sonar, parar_bocina
#include "TimerUtils.h"    // esperar_ms
#include "teclado.h"       // teclado_init, teclado_getKey

// LED ámbar (bit 0) y rojo (bit 1) en PORTC, activos bajo
#define LED_AMBER_MASK  (1 << 0)
#define LED_RED_MASK    (1 << 1)

// Códigos de desarme por teclado
#define CODE_DIS_SCAN   "4467"
#define CODE_DIS_ALARM  "1134"
#define CODE_LEN        4

// PIR
#define PIR_ACTIVE      1        // nivel activo
#define PIR_DEBOUNCE_MS 50       // ms de filtrado

// Parpadeo en ALARM cada 2 s
#define BLINK_MS        2000    

typedef enum { STANDBY, SCANNING, ALARM } state_t;

int main(void) {
    // liberar JTAG
    DDPCONbits.JTAGEN = 0;

    // init módulos
    inicializarUARTySistema();
    initPIR();
    inicializaServo();
    inicializar_bocina();
    teclado_init();

    // configurar RC0/RC1 como digitales y salidas
    ANSELC &= ~(LED_AMBER_MASK | LED_RED_MASK);
    TRISC  &= ~(LED_AMBER_MASK | LED_RED_MASK);
    // apagar LEDs (alto = apagado)
    LATC |= (LED_AMBER_MASK | LED_RED_MASK);

    // estado inicial
    abrirPuerta();
    parar_bocina();

    state_t state = STANDBY, prev = -1;
    char buf[CODE_LEN+1]; int idx = 0;
    uint8_t pir_last = leerPIR();

    while (1) {
        // ? UART ?
        manejarUART();
        if (desactivacion) {
            estadoSistema = 0; desactivacion = 0;
            putsUART("\r\n> Desarmado por UART\r\n");
        }

        // ? PIR con filtrado ?
        uint8_t pir = leerPIR();
        if (pir != pir_last) {
            esperar_ms(PIR_DEBOUNCE_MS);
            if (leerPIR() == pir) pir_last = pir;
        }

        // ? Teclado ?
        if (char k = teclado_getKey()) {
            putcUART(k);
            buf[idx++] = k;
            if (idx >= CODE_LEN) {
                buf[CODE_LEN] = '\0';
                if (state == SCANNING && strcmp(buf, CODE_DIS_SCAN) == 0) {
                    estadoSistema = 0; putsUART("\r\n> Desarmado por teclado\r\n");
                }
                if (state == ALARM && strcmp(buf, CODE_DIS_ALARM) == 0) {
                    estadoSistema = 0; putsUART("\r\n> Desarmado en alarma\r\n");
                }
                memmove(buf, buf+1, CODE_LEN-1);
                idx = CODE_LEN-1;
            }
        }

        // ? Transiciones ?
        switch (state) {
            case STANDBY:
                if (estadoSistema) state = SCANNING;
                break;
            case SCANNING:
                if (!estadoSistema) state = STANDBY;
                else if (pir_last == PIR_ACTIVE) state = ALARM;
                break;
            case ALARM:
                if (!estadoSistema) state = STANDBY;
                break;
        }

        // ? Entrada en nuevo estado ?
        if (state != prev) {
            switch (state) {
                case STANDBY:
                    // apagar todo y reset
                    LATC |= (LED_AMBER_MASK | LED_RED_MASK);
                    parar_bocina();
                    abrirPuerta();
                    idx = 0; pir_last = leerPIR();
                    break;
                case SCANNING:
                    // encender ámbar
                    LATC &= ~LED_AMBER_MASK;
                    putsUART("\r\n> Sistema ARMADO y ESCANEANDO\r\n");
                    abrirPuerta();
                    idx = 0;
                    break;
                case ALARM:
                    // ámbar ON + cerrar puerta
                    LATC &= ~LED_AMBER_MASK;
                    putsUART("\r\n¡Intruso! Cerrando puerta...\r\n");
                    cerrarPuerta();
                    idx = 0;
                    break;
            }
        }

        // ? Continuo en ALARM: parpadeo lento ?
        if (state == ALARM) {
            LATC &= ~LED_RED_MASK;   // rojo ON
            sonar();
            esperar_ms(BLINK_MS);
            LATC |= LED_RED_MASK;    // rojo OFF
            parar_bocina();
            esperar_ms(BLINK_MS);
        }

        prev = state;
    }
    return 0;
}
