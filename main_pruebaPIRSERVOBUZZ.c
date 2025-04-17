// main_pruebaPIR.c

#include <xc.h>
#include "Pic32Ini.h"   // configuraciones de sistema
#include "Pir.h"        // initPIR(), leerPIR()
#include "Servo.h"      // inicializaServo(), abrirPuerta(), cerrarPuerta()
#include "buzzer.h"     // inicializar_bocina(), sonar(), parar_bocina()
#include "TimerUtils.h" // esperar_ms()

// Deshabilitar JTAG para liberar RB2/RB3 si fuera necesario

int main(void) {
    // --- Desactivar JTAG en tiempo de ejecuci�n (alternativa al pragma) ---

    // Inicializar perif�ricos
    inicializar_bocina();   // configura PWM de buzzer
    initPIR();              // configura PIN RA7 para sensor PIR
    inicializaServo();      // configura OC1 y Timer2 para servo

    // Estado inicial: puerta abierta y buzzer apagado
    abrirPuerta();
    parar_bocina();

    while (1) {
        // leerPIR() devuelve 0 cuando detecta movimiento
        if (leerPIR()) {
            // Intrusi�n detectada
            cerrarPuerta();
            sonar();
        } else {
            // No hay intrusi�n
            abrirPuerta();
            parar_bocina();
        }

        // Peque�a pausa para evitar rebotes y no saturar el bus
        esperar_ms(100);
    }

    return 0;
}
