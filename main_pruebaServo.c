#include <xc.h> 
#include <stdint.h> 
#include "Servo.h" 
#include "Pic32Ini.h" 
#include "TimerUtils.h"

#define PIN_PULSADOR 5
#define CERRADO -90
#define ABIERTO 90

int main(void){
    inicializaServo();

    // Configurar RB5 como entrada digital
    ANSELB &= ~(1 << PIN_PULSADOR);
    TRISB |= (1 << PIN_PULSADOR);

    int ant = (PORTB >> PIN_PULSADOR) & 1;

    while(1){
        int act = (PORTB >> PIN_PULSADOR) & 1;

        // Detecta flanco de bajada (pulsación)
        if (act != ant && act == 0) {
            int angulo = obtenerAngulo();
            if (angulo <= CERRADO + 1) {
                abrirPuerta();
            } else if (angulo >= ABIERTO - 1) {
                cerrarPuerta();
            }

            esperar_ms(300); // Pequeña espera para evitar rebotes
        }

        ant = act;
    }

    return 0;
}
