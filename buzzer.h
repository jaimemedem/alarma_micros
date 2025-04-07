#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <xc.h>
#include "Pic32Ini.h"

void inicializar_bocina(void);
void sonar(void);
void parar_bocina(void);

#endif // BUZZER_H
