#include <xc.h> 
#include <stdint.h> 
#include "Servo.h" 
#include "Pic32Ini.h" 

#define PIN_PULSADOR 5
#define CERRADO 0
#define ABIERTO 90

int main(void){
    TRISB = 0xFFFF;
    int ant, act;
    ant=(PORTB>>PIN_PULSADOR)&1;
    while(1){
        act=(PORTB>>PIN_PULSADOR)&1;
        if(act=!ant && act==0)
        {
            int angulo= obtenerAngulo();
            if (angulo==CERRADO)
            {
                abrirPuerta();
            }else if(angulo == ABIERTO)
            {
                cerrarPuerta();
            }
        }
    }
}