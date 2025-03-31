#include "Temporizador.h"
#include <xc.h>

static volatile uint32_t ticks = 0;

void InicializarTimer1(void) {
    T1CON = 0x0000;
    TMR1 = 0;

    // Para Fcy = 5 MHz, prescaler 256 ? PR1 = 19531 para 1s
    PR1 = 19531;

    IPC1bits.T1IP = 2;
    IPC1bits.T1IS = 0;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;

    T1CON = 0x8030; // TCKPS = 0b11 (1:256), Timer ON
}

void startTimer1(void) {
    TMR1 = 0;
    T1CONbits.ON = 1;
}

void reiniciarTicks(void) {
    ticks = 0;
}

uint32_t getTicks(void) {
    return ticks;
}

__attribute__((vector(_TIMER_1_VECTOR), interrupt(IPL2SOFT), nomips16))
void InterrupcionTimer1(void) {
    IFS0bits.T1IF = 0;
    ticks++;
}
