#include <xc.h>
#include <stdint.h>

#define CERRADO 0
#define ABIERTO 90
#define PWM_BASE 2499 // 1 ms en cuentas de timer
#define PWM_SCALE 14  // Cuentas por grado
#define TIMER_PERIOD 49999 // 20 ms en cuentas de timer

static int current_angle = CERRADO;

void inicializaServo(void) {
    // Configuración del pin RB15 como salida digital
    ANSELB &= ~(1 << 15);
    
    // Desbloqueo de registros protegidos
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB15R = 5; // Asigna OC1 al pin RB15
    SYSKEY = 0x1CA11CA1; // Bloqueo de registros
    
    // Configuración del módulo de PWM (Output Compare 1 - OC1)
    OC1CON = 0;
    OC1R = PWM_BASE; // Pulso inicial de 1 ms
    OC1RS = PWM_BASE;
    OC1CON = 0x8006; // Modo PWM sin fallos
    
    // Configuración del Timer2 para generar la señal PWM
    T2CON = 0;
    TMR2 = 0;
    PR2 = TIMER_PERIOD;
    T2CON = 0x8010; // Preescalado 2 y habilitación del timer
}

void abrirPuerta() {
    current_angle = ABIERTO;
    OC1RS = PWM_BASE + (current_angle + 90) * PWM_SCALE;
}

void cerrarPuerta() {
    current_angle = CERRADO;
    OC1RS = PWM_BASE + (current_angle + 90) * PWM_SCALE;
}



int obtenerAngulo(void) {
    return current_angle;
}
