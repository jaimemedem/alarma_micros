/* 
 * File:   main_pruebaFULL.c
 * Author: andre
 *
 * Created on 17 de abril de 2025, 20:27
 */


#include <xc.h>
#include <string.h>
#include "Pic32Ini.h"      // Configuración general del PIC
#include "gestionUART.h"   // inicializarUARTySistema(), manejarUART(), estadoSistema, desactivacion
#include "UartCol.h"       // putsUART(), putcUART()
#include "Pir.h"           // initPIR(), leerPIR()
#include "Servo.h"         // inicializaServo(), abrirPuerta(), cerrarPuerta()
#include "buzzer.h"        // inicializar_bocina(), sonar(), parar_bocina()
#include "TimerUtils.h"    // esperar_ms()
#include "teclado.h"       // teclado_init(), teclado_getKey()

// LEDs en RC0 (ámbar) y RC1 (rojo), activos a nivel bajo
#define LED_AMBER LATCbits.LATC0
#define LED_RED   LATCbits.LATC1

// Códigos de 4 dígitos para desactivar
#define CODE_DIS_SCAN   "4467"
#define CODE_DIS_ALARM  "1134"
#define CODE_LEN        4

// Parámetros PIR
#define PIR_ACTIVE      1    // nivel de detección
#define PIR_DEBOUNCE_MS 50   // ms de filtrado

typedef enum { STATE_STANDBY, STATE_SCANNING, STATE_ALARM } system_state_t;

int main(void) {
    // 1) Deshabilita JTAG para liberar pines RBx/RCx
    DDPCONbits.JTAGEN = 0;

    // 2) Inicializa todos los módulos
    inicializarUARTySistema();   // UART y mensaje "Sistema listo"
    initPIR();                   // prepara RA7
    inicializaServo();           // PWM en RB15 para servo
    inicializar_bocina();        // PWM en RA1 para buzzer
    teclado_init();              // pines de teclado matricial

    // 3) Configura LEDs como salidas digitales, apagados (alto = apagado)
    ANSELC &= ~((1 << 0) | (1 << 1));
    TRISC  &= ~((1 << 0) | (1 << 1));
    LED_AMBER = 1;
    LED_RED   = 1;

    // 4) Estado inicial STANDBY: puerta abierta, buzzer apagado
    abrirPuerta();
    parar_bocina();

    // 5) Variables de estado y buffers
    system_state_t state = STATE_STANDBY, prev_state = -1;
    char keybuf[CODE_LEN+1];
    int  keyidx = 0;
    uint8_t pir_last = leerPIR();

    while (1) {
        // ??? UART ??????????????????????????????????????
        manejarUART();
        if (desactivacion) {
            estadoSistema = 0;           // forzamos OFF
            desactivacion  = 0;
            putsUART("\r\n> Desactivado por UART\r\n");
        }

        // ??? PIR con filtrado ????????????????????????
        uint8_t pir = leerPIR();
        if (pir != pir_last) {
            esperar_ms(PIR_DEBOUNCE_MS);
            if (leerPIR() == pir) pir_last = pir;
        }

        // ??? Teclado ????????????????????????????
        char k = teclado_getKey();
        if (k) {
            putcUART(k);                 // eco
            keybuf[keyidx++] = k;
            if (keyidx >= CODE_LEN) {
                keybuf[CODE_LEN] = '\0';
                if (state == STATE_SCANNING && strcmp(keybuf, CODE_DIS_SCAN) == 0) {
                    estadoSistema = 0;
                    putsUART("\r\n> Desactivado por teclado\r\n");
                }
                if (state == STATE_ALARM && strcmp(keybuf, CODE_DIS_ALARM) == 0) {
                    estadoSistema = 0;
                    putsUART("\r\n> Desactivado por teclado (alarma)\r\n");
                }
                memmove(keybuf, keybuf + 1, CODE_LEN - 1);
                keyidx = CODE_LEN - 1;
            }
        }

        // ??? Transiciones de estado ????????????????????
        switch (state) {
            case STATE_STANDBY:
                if (estadoSistema == 1) state = STATE_SCANNING;
                break;
            case STATE_SCANNING:
                if (estadoSistema == 0) {
                    state = STATE_STANDBY;
                } else if (pir_last == PIR_ACTIVE) {
                    state = STATE_ALARM;
                }
                break;
            case STATE_ALARM:
                if (estadoSistema == 0) {
                    state = STATE_STANDBY;
                }
                break;
        }

        // ??? Acciones de entrada (solo al cambiar de estado) ?
        if (state != prev_state) {
            switch (state) {
                case STATE_STANDBY:
                    // restaurar condiciones iniciales
                    LED_AMBER = 1; LED_RED = 1;
                    parar_bocina();
                    abrirPuerta();
                    keyidx = 0;
                    pir_last = leerPIR();  // reiniciar filtro PIR
                    break;
                case STATE_SCANNING:
                    LED_AMBER = 0;  // ámbar encendido
                    putsUART("\r\n> Sistema ACTIVADO y ESCANEANDO\r\n");
                    abrirPuerta();
                    keyidx = 0;
                    break;
                case STATE_ALARM:
                    LED_AMBER = 0;  // ámbar sigue ON
                    putsUART("\r\n¡Intruso detectado! Cerrando puerta...\r\n");
                    cerrarPuerta(); // **Ahora sí cierra la puerta**
                    keyidx = 0;
                    break;
            }
        }

        // ??? Acciones por estado ??????????????????????
        if (state == STATE_ALARM) {
            // parpadeo 1 s ON/OFF del LED rojo y buzzer
            LED_RED = 0; sonar();   esperar_ms(1000);
            LED_RED = 1; parar_bocina(); esperar_ms(1000);
        }

        prev_state = state;
    }
    return 0;
}
