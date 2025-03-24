#ifndef SERVO_H
#define SERVO_H

#include <xc.h>
#include <stdint.h>

#define CERRADO 0
#define ABIERTO 90
#define PWM_BASE 2499 // 1 ms en cuentas de timer
#define PWM_SCALE 14  // Cuentas por grado
#define TIMER_PERIOD 49999 // 20 ms en cuentas de timer

void inicializaServo(void);
void abrirPuerta();
void cerrarPuerta();
int obtenerAngulo(void);

#endif /* SERVO_H */