// main.c
#include <xc.h>
#include "Pic32Ini.h"   // Configuración del PIC
#include "Pir.h"        // initPIR(), leerPIR()
#include "Servo.h"      // inicializaServo(), abrirPuerta(), cerrarPuerta()
#include "buzzer.h"     // inicializar_bocina(), sonar(), parar_bocina()
#include "TimerUtils.h" // esperar_ms()

// Si tu PIR saca '1' al detectar movimiento, déjalo a 1;
// si es activo bajo, cámbialo aquí a 0.
#define PIR_DETECT_LEVEL 1
#define PIR_FILTER_MS     50   // antirrebote / filtrado de PIR

int main(void) {
    // Desactiva JTAG para liberar todos los pines de I/O
    DDPCONbits.JTAGEN = 0;

    // Inicializa módulos
    initPIR();
    inicializaServo();
    inicializar_bocina();

    // Estado inicial: puerta abierta y buzzer apagado
    abrirPuerta();
    parar_bocina();

    // Lee el estado inicial del PIR
    uint8_t prevPir = leerPIR();

    while (1) {
        // Lee PIR y filtra cambios
        uint8_t pir = leerPIR();
        if (pir != prevPir) {
            // Ha cambiado: espera un poco y confirma
            esperar_ms(PIR_FILTER_MS);
            if (leerPIR() == pir) {
                prevPir = pir;
                if (prevPir == PIR_DETECT_LEVEL) {
                    // Movimiento detectado
                    cerrarPuerta();
                    sonar();
                } else {
                    // Vuelta a reposo
                    abrirPuerta();
                    parar_bocina();
                }
            }
        }
        // Pequeño retardo para no saturar el bucle
        esperar_ms(10);
    }

    return 0;
}
