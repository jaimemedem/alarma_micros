#ifndef SERVO_H
#define SERVO_H

void inicializaServo(void);
void ajustarAngulo(int delta);
int obtenerAngulo(void);
void setAngulo(int angulo);
void abrirPuerta(void);
void cerrarPuerta(void);

#endif // SERVO_H
