#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Configuración del PIC y UART
#include "Pic32Ini.h"
#include "UartCol.h"
#include "gestionUART.h"

// Sensores y actuadores
#include "Pir.h"
#include "buzzer.h"
#include "Servo.h"
#include "Temporizador.h"
#include "TimerUtils.h"

// Pines según nuevas conexiones:
// RC0 = LED amarillo (activo bajo)
// RC1 = LED rojo    (activo bajo)
#define LED_YELLOW_BIT 0   // RC0
#define LED_RED_BIT    1   // RC1

// Estados de la máquina
#define SYS_OFF     0
#define SYS_ACTIVE  1
#define SYS_ALARM   2

int main(void) {
    int sysState  = SYS_OFF;
    int prevState = -1;

    // --- Inicializaciones básicas ---
    inicializarUARTySistema();  // UART + mensaje inicial
    INTCONbits.MVEC = 1;
    __builtin_enable_interrupts();

    inicializar_bocina();
    initPIR();
    inicializaServo();

    // Configurar RC0 y RC1 como salidas digitales
    ANSELC &= ~((1 << LED_YELLOW_BIT) | (1 << LED_RED_BIT));
    TRISC  &= ~((1 << LED_YELLOW_BIT) | (1 << LED_RED_BIT));
    // Apagar ambos (activo bajo => LAT=1)
    LATCbits.LATC0 = 1;
    LATCbits.LATC1 = 1;

    // **Abrir puerta al arrancar**
    abrirPuerta();

    while (1) {
        // Leer y procesar UART (ON/OFF/1234)
        manejarUART();

        // Si la contraseña marcó desactivación, forzamos OFF y apagamos buzzer
        if (desactivacion) {
            parar_bocina();
            estadoSistema = 0;
            desactivacion  = 0;
        }

        switch (sysState) {
            case SYS_OFF:
                if (prevState != SYS_OFF) {
                    // Entrando en OFF: LEDs apagados, buzzer apagado
                    LATCbits.LATC0 = 1;
                    LATCbits.LATC1 = 1;
                    parar_bocina();
                    // **No cerramos la puerta en OFF**
                }
                if (estadoSistema == 1) {
                    sysState = SYS_ACTIVE;
                }
                break;

            case SYS_ACTIVE:
                if (prevState != SYS_ACTIVE) {
                    // Entrada ACTIVO: LED amarillo ON y garantizar puerta abierta
                    putsUART("\r\nSISTEMA ACTIVADO. Puerta abierta.\r\n");
                    LATCbits.LATC0 = 0;   // amarillo ON
                    abrirPuerta();
                }
                if (estadoSistema == 0) {
                    // OFF desde activo
                    parar_bocina();
                    sysState = SYS_OFF;
                    break;
                }
                // PIR devuelve 0 cuando detecta intruso
                if (leerPIR() == 0) {
                    sysState = SYS_ALARM;
                }
                break;

            case SYS_ALARM:
                if (prevState != SYS_ALARM) {
                    // Entrada ALARMA: LED amarillo sigue ON, cerrar puerta
                    putsUART("\r\n¡Intruso detectado! Cerrando puerta...\r\n");
                    LATCbits.LATC0 = 0;   // amarillo ON
                    cerrarPuerta();
                }
                // Parpadeo 2 s ON / 2 s OFF para LED rojo y buzzer
                LATCbits.LATC1 = 0;  // rojo ON
                sonar();
                esperar_ms(2000);
                manejarUART();
                if (estadoSistema == 0) {
                    parar_bocina();
                    sysState = SYS_OFF;
                    break;
                }

                LATCbits.LATC1 = 1;  // rojo OFF
                parar_bocina();
                esperar_ms(2000);
                manejarUART();
                if (estadoSistema == 0) {
                    parar_bocina();
                    sysState = SYS_OFF;
                }
                break;
        }

        prevState = sysState;
    }

    return 0;
}
