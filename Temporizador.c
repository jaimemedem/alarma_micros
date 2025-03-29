/* 
 * File:   Temporizador.c
 * Author: andre
 *
 * Created on 10 de febrero de 2025, 9:43
 */

#include "Temporizador.h"
#include <xc.h>

static uint32_t ticks = 0, ticks3=0; // Variable global al driver
void InicializarTimer2(void) {
    T2CON = 0x0000;
    TMR2 = 0;
    PR2 = 4999; // Se configura el timer para que termine la cuenta en 1 s
    IPC2bits.T2IP = 2; // Se configura la prioridad de la interrupción
    IPC2bits.T2IS = 0; // Subprioridad 0
    IFS0bits.T2IF = 0; // Se borra el flag de interrupción por si estaba pendiente
    IEC0bits.T2IE = 1; // y por último se habilita su interrupción
    T2CON = 0x0070; // Se arranca el timer con prescalado 1:256
}

void startTimer2(void){
    T2CON |= 0x8000; // Se arranca el timer con prescalado 1
}
__attribute__((vector(_TIMER_2_VECTOR), interrupt(IPL2SOFT), nomips16)) void
InterrupcionTimer2(void) {
    // Se borra el flag de interrupción
    IFS0bits.T2IF = 0;
    ticks ++;
}
uint32_t getTicks(void) {
    return ticks;
}

void InicializarTimer3(void) {
    T3CON = 0x0000;
    TMR3 = 0;
    PR3 = 4999; // Se configura el timer para que termine la cuenta en 1 ms
    IPC3bits.T3IP = 4; // Se configura la prioridad de la interrupción
    IPC3bits.T3IS = 0; // Subprioridad 0
    IFS0bits.T3IF = 0; // Se borra el flag de interrupción por si estaba pendiente
    IEC0bits.T3IE = 1; // y por último se habilita su interrupción
    T3CON = 0x8000; // Se arranca el timer con prescalado 1
}

__attribute__((vector(_TIMER_3_VECTOR), interrupt(IPL4SOFT), nomips16)) void
InterrupcionTimer3(void) {
    // Se borra el flag de interrupción
    IFS0bits.T3IF = 0;
    ticks3 ++;
}

uint32_t getTicks3(void) {
    return ticks3;
}

