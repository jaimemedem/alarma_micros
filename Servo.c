#include <xc.h>
#include <stdint.h>
#include "Servo.h"
#include "TimerUtils.h"

#define MIN_ANGLE -90
#define MAX_ANGLE 90
#define PWM_BASE 2499
#define PWM_SCALE 14
#define TIMER_PERIOD 49999

static int current_angle = MIN_ANGLE;

void inicializaServo(void) {
    ANSELB &= ~(1 << 15);
    TRISB &= ~(1 << 15);

    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB15R = 5;
    SYSKEY = 0x1CA11CA1;

    OC1CON = 0;
    OC1R = PWM_BASE;
    OC1RS = PWM_BASE;
    OC1CON = 0x8006;

    T2CON = 0;
    TMR2 = 0;
    PR2 = TIMER_PERIOD;
    T2CON = 0x8010;
}

void ajustarAngulo(int delta) {
    int nuevo = current_angle + delta;
    if (nuevo > MAX_ANGLE) nuevo = MAX_ANGLE;
    if (nuevo < MIN_ANGLE) nuevo = MIN_ANGLE;

    current_angle = nuevo;
    OC1RS = PWM_BASE + (current_angle + 90) * PWM_SCALE;
}

int obtenerAngulo(void) {
    return current_angle;
}

void setAngulo(int angulo) {
    if (angulo > MAX_ANGLE) angulo = MAX_ANGLE;
    if (angulo < MIN_ANGLE) angulo = MIN_ANGLE;

    current_angle = angulo;
    OC1RS = PWM_BASE + (current_angle + 90) * PWM_SCALE;
}

void abrirPuerta(void) {
    while (current_angle < MAX_ANGLE) {
        ajustarAngulo(10);
        esperar_ms(100);
    }
    OC1RS = PWM_BASE + (current_angle + 90) * PWM_SCALE;
}

void cerrarPuerta(void) {
    while (current_angle > MIN_ANGLE) {
        ajustarAngulo(-10);
        esperar_ms(100);
    }
    OC1RS = PWM_BASE + (current_angle + 90) * PWM_SCALE;
}
