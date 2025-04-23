/* 
 * File:   main_pruebaFULL.c
 * Author: andre
 *
 * Created on 17 de abril de 2025, 20:27
 */

#include <xc.h>
#include <string.h>
#include "Pic32Ini.h"
#include "gestionUART.h"
#include "UartCol.h"
#include "Pir.h"
#include "Servo.h"
#include "buzzer.h"
#include "TimerUtils.h"
#include "teclado.h"

// LEDs en RC0 (ámbar) y RC1 (rojo), activos a nivel bajo
#define LED_AMBER_MASK   0x1   // bit 0
#define LED_RED_MASK     0x2   // bit 1

// Códigos de 4 dígitos para desactivar
#define CODE_DIS_SCAN   "4467"
#define CODE_DIS_ALARM  "1134"
#define CODE_LEN        4

// Parámetros PIR
#define PIR_ACTIVE      1
#define PIR_DEBOUNCE_MS 50

typedef enum { STATE_STANDBY, STATE_SCANNING, STATE_ALARM } system_state_t;

int main(void) {
    DDPCONbits.JTAGEN = 0;               // libera JTAG
    inicializarUARTySistema();           // UART + mensaje inicial
    initPIR();                           // configura RA7
    inicializaServo();                   // PWM servo RB15
    inicializar_bocina();                // PWM buzzer RA1
    teclado_init();                      // teclado 4×3

    // Configurar RC0/RC1 como salidas digitales y apagarlos
    ANSELC &= ~(LED_AMBER_MASK | LED_RED_MASK);
    TRISC  &= ~(LED_AMBER_MASK | LED_RED_MASK);
    LATC  |=  (LED_AMBER_MASK | LED_RED_MASK);

    abrirPuerta();
    parar_bocina();

    system_state_t state = STATE_STANDBY, prev_state = -1;
    char keybuf[CODE_LEN+1];
    int  keyidx = 0;
    uint8_t pir_last = leerPIR();

    while (1) {
        // UART
        manejarUART();
        if (desactivacion) {
            estadoSistema = 0;
            desactivacion  = 0;
            putsUART("\r\n> Desarmado por UART\r\n");
        }

        // PIR con filtrado
        uint8_t pir = leerPIR();
        if (pir != pir_last) {
            esperar_ms(PIR_DEBOUNCE_MS);
            if (leerPIR() == pir) pir_last = pir;
        }

        // Teclado
        char k = teclado_getKey();
        if (k) {
            putcUART(k);
            keybuf[keyidx++] = k;
            if (keyidx >= CODE_LEN) {
                keybuf[CODE_LEN] = '\0';
                if (state==STATE_SCANNING && strcmp(keybuf, CODE_DIS_SCAN)==0) {
                    estadoSistema=0; putsUART("\r\n> Desarmado por teclado\r\n");
                }
                if (state==STATE_ALARM && strcmp(keybuf, CODE_DIS_ALARM)==0) {
                    estadoSistema=0; putsUART("\r\n> Desarmado en alarma\r\n");
                }
                memmove(keybuf, keybuf+1, CODE_LEN-1);
                keyidx = CODE_LEN-1;
            }
        }

        // Transiciones
        switch (state) {
            case STATE_STANDBY:
                if (estadoSistema) state = STATE_SCANNING;
                break;
            case STATE_SCANNING:
                if (!estadoSistema) state = STATE_STANDBY;
                else if (pir_last == PIR_ACTIVE) state = STATE_ALARM;
                break;
            case STATE_ALARM:
                if (!estadoSistema) state = STATE_STANDBY;
                break;
        }

        // Acciones al entrar en estado nuevo
        if (state != prev_state) {
            switch (state) {
                case STATE_STANDBY:
                    LATC |=  (LED_AMBER_MASK | LED_RED_MASK);
                    parar_bocina();
                    abrirPuerta();
                    keyidx = 0;
                    pir_last = leerPIR();
                    break;
                case STATE_SCANNING:
                    LATC &= ~LED_AMBER_MASK;
                    putsUART("\r\n> Sistema ACTIVADO y ESCANEANDO\r\n");
                    abrirPuerta();
                    keyidx = 0;
                    break;
                case STATE_ALARM:
                    LATC &= ~LED_AMBER_MASK;
                    putsUART("\r\n¡Intruso detectado! Cerrando puerta...\r\n");
                    cerrarPuerta();
                    keyidx = 0;
                    break;
            }
        }

        // Parpadeo en ALARM cada 500 ms (ahora visible)
        if (state == STATE_ALARM) {
            LATC &= ~LED_RED_MASK; sonar();   esperar_ms(500);
            LATC |=  LED_RED_MASK; parar_bocina(); esperar_ms(500);
        }

        prev_state = state;
    }

    return 0;
}
