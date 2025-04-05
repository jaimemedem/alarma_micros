#include <xc.h>
#include "buzzer.h"

#define BUZZER_PIN 14  // Pin RB14 (RPB14)

// PWM Configuración
#define PWM_FREQ_HZ 2000         // 2 kHz
#define SYSCLK 5000000UL        // Reloj del sistema
#define PRESCALER 1
#define PR3_VAL ((SYSCLK / PRESCALER) / PWM_FREQ_HZ)

void initBuzzer(void) {
    // Desactiva la función analógica del pin RB14
    ANSELB &= ~(1 << BUZZER_PIN);
    
    // Configura RB14 como salida digital
    TRISB &= ~(1 << BUZZER_PIN);

    // Mapear OC2 a RPB14
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB14R = 5; // OC2
    SYSKEY = 0x1CA11CA1;

    // Configura Timer3 para 2kHz
    T3CON = 0;
    TMR3 = 0;
    PR3 = PR3_VAL - 1;
    T3CON = 0x8000; // Preescalador 1, Timer ON

    // Configura OC2 para PWM
    OC2CON = 0;
    OC2R = PR3_VAL / 2;   // Ciclo de trabajo 50%
    OC2RS = PR3_VAL / 2;
    OC2CON = 0x8006; // PWM mode sin fallos
}

void activarBuzzer(void) {
    OC2CONSET = 0x8000; // Activa OC2
}

void apagarBuzzer(void) {
    OC2CONCLR = 0x8000; // Desactiva OC2
}
