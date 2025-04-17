#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Configuración del PIC y UART
#include "Pic32Ini.h"
#include "UartCol.h"       // InicializarUART1, getcUART, putcUART, putsUART
#include "gestionUART.h"   // manejarUART(), procesarComando(), variables estadoSistema y desactivacion

// Sensores y actuadores
#include "Pir.h"           // initPIR(), leerPIR()
#include "buzzer.h"        // inicializar_bocina(), sonar(), parar_bocina()
#include "Servo.h"         // inicializaServo(), abrirPuerta(), cerrarPuerta()
#include "Temporizador.h"  // InicializarTimer1(), startTimer1(), getTicks()
#include "TimerUtils.h"    // esperar_ms()

// Pines según conexiones:
// RA8 = LED amarillo (activo bajo)
// RA9 = LED rojo    (activo bajo)
#define LED_YELLOW_BIT 8
#define LED_RED_BIT    9

// Estados de la máquina
#define SYS_OFF     0
#define SYS_ACTIVE  1
#define SYS_ALARM   2

int main(void) {
    int sysState = SYS_OFF;
    int prevState = -1;

    // --- Inicializaciones básicas ---
    InicializarUART1(9600);
    INTCONbits.MVEC = 1;
    __builtin_enable_interrupts();
    putsUART("\r\nSistema listo. Envie ON/OFF:\r\n");

    inicializar_bocina();
    initPIR();
    inicializaServo();

    // Configurar LEDs como salidas digitales
    ANSELA &= ~((1 << LED_YELLOW_BIT) | (1 << LED_RED_BIT));
    TRISA  &= ~((1 << LED_YELLOW_BIT) | (1 << LED_RED_BIT));
    // Apagar ambos (activo bajo => LAT=1)
    LATAbits.LATA8 = 1;
    LATAbits.LATA9 = 1;

    // Asegurar puerta cerrada al arrancar
    cerrarPuerta();

    // Bucle principal
    while (1) {
        // Leer y procesar UART
        manejarUART();

        // Si la contraseña marcó desactivación, forzamos OFF
        if (desactivacion) {
            estadoSistema = 0;
            desactivacion = 0;
        }

        switch (sysState) {
            case SYS_OFF:
                if (prevState != SYS_OFF) {
                    // Paso a OFF: todo apagado y puerta cerrada
                    LATAbits.LATA8 = 1;
                    LATAbits.LATA9 = 1;
                    parar_bocina();
                    cerrarPuerta();
                }
                if (estadoSistema == 1) {
                    sysState = SYS_ACTIVE;
                }
                break;

            case SYS_ACTIVE:
                if (prevState != SYS_ACTIVE) {
                    // Entrada en ACTIVO
                    putsUART("\r\nSISTEMA ACTIVADO. Abriendo puerta...\r\n");
                    LATAbits.LATA8 = 0;   // LED amarillo ON
                    abrirPuerta();
                }
                if (estadoSistema == 0) {
                    sysState = SYS_OFF;
                    break;
                }
                if (leerPIR()) {
                    sysState = SYS_ALARM;
                }
                break;

            case SYS_ALARM:
                if (prevState != SYS_ALARM) {
                    // Entrada en ALARMA
                    putsUART("\r\n¡Intruso detectado! Cerrando puerta...\r\n");
                    LATAbits.LATA8 = 0;   // amarillo sigue ON
                    cerrarPuerta();
                }
                // Parpadeo y buzzer 1 Hz, check UART cada medio ciclo
                LATAbits.LATA9 = 0;  // rojo ON
                sonar();
                esperar_ms(1000);
                manejarUART();
                if (estadoSistema == 0) { sysState = SYS_OFF; break; }

                LATAbits.LATA9 = 1;  // rojo OFF
                parar_bocina();
                esperar_ms(1000);
                manejarUART();
                if (estadoSistema == 0) { sysState = SYS_OFF; }
                break;
        }

        prevState = sysState;
    }

    return 0;
}
