#include <xc.h>
#include "TimerUtils.h"

#define FCY 40000000UL  // Frecuencia del sistema (ajústala si es diferente)
#define PRESCALER 64
#define TICKS_PER_MS (FCY / PRESCALER / 1000)

void esperar_ms(unsigned int ms) {
    // Configurar Timer4
    T4CON = 0;
    TMR4 = 0;
    PR4 = TICKS_PER_MS * ms;
    T4CONbits.TCKPS = 2; // Preescalador 1:64
    T4CONbits.ON = 1;

    // Esperar a que termine
    while (!IFS0bits.T4IF);
    IFS0bits.T4IF = 0; // Limpiar flag
    T4CONbits.ON = 0;  // Apagar timer
}
