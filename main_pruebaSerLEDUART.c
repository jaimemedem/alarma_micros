#include <xc.h>
#include "gestionUART.h"
#include "Servo.h"
#include "Temporizador.h"

#define LED_RC4_MASK (1 << 4)
#define LED_RC5_MASK (1 << 5)
#define PIN_PULSADOR 5

uint32_t lastTicks = 0;
int modoParpadeo = 0;

void configurarIO(void);
void manejarPulsador(int *ant);
void actualizarParpadeoLED(void);

int main(void) {
    configurarIO();
    inicializarUARTySistema();
    inicializaServo();
    setAngulo(90);     // Forzar ángulo abierto
    abrirPuerta();

    LATC |= LED_RC4_MASK; // Apagar LED RC4
    LATC |= LED_RC5_MASK; // Apagar LED RC5

    int ant = (PORTB >> PIN_PULSADOR) & 1;

    while (1) {
        manejarUART();

        if (estadoSistema) {
            LATC &= ~LED_RC5_MASK;  // Encender RC5 (activo bajo)
            manejarPulsador(&ant);
        } else {
            // Estado OFF ? reset
            LATC |= LED_RC4_MASK;   // Apagar RC4
            LATC |= LED_RC5_MASK;   // Apagar RC5
            modoParpadeo = 0;
            T1CONbits.ON = 0;       // Detener Timer1
            reiniciarTicks();
            setAngulo(90);
            abrirPuerta();
        }

        actualizarParpadeoLED();
    }

    return 0;
}

void configurarIO(void) {
    // LEDs como salidas digitales
    ANSELC &= ~(LED_RC4_MASK | LED_RC5_MASK);
    TRISC &= ~(LED_RC4_MASK | LED_RC5_MASK);
    LATC |= (LED_RC4_MASK | LED_RC5_MASK); // Apagar (activo bajo)

    // Pulsador RB5 como entrada
    ANSELB &= ~(1 << PIN_PULSADOR);
    TRISB |= (1 << PIN_PULSADOR);

    InicializarTimer1(); // Timer1 para parpadeo (ajustado a 5 MHz)
}

void manejarPulsador(int *ant) {
    int act = (PORTB >> PIN_PULSADOR) & 1;

    if (act != *ant && act == 0) { // Flanco de bajada
        cerrarPuerta();
        modoParpadeo = 1;
        reiniciarTicks();
        startTimer1(); // Inicia parpadeo en RC4
    }

    *ant = act;
}

void actualizarParpadeoLED(void) {
    if (modoParpadeo) {
        uint32_t current = getTicks();
        if (current > lastTicks) {
            LATC ^= LED_RC4_MASK;
            lastTicks = current;
        }
    }
}
