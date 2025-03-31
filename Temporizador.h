#ifndef TEMPORIZADOR_H
#define TEMPORIZADOR_H

#include <stdint.h>

// Inicializa el Timer1 para interrupciones cada 1 segundo
void InicializarTimer1(void);

// Reinicia el contador de tiempo y activa el Timer1
void startTimer1(void);

// Obtiene la cantidad de segundos transcurridos desde el último reinicio
uint32_t getTicks(void);

// Reinicia el contador de segundos
void reiniciarTicks(void);

#endif // TEMPORIZADOR_H
