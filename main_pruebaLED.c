/* 
 * File:   main_pruebaLED.c
 * Author: andre
 *
 * Created on 17 de abril de 2025, 16:13
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#define AMARILLO 8
#define ROJO 9

/*
 * 
 */
void main(void){
    ANSELA &= ~((1<<AMARILLO)|(1<<ROJO));
    TRISA  &= ~((1<<AMARILLO)|(1<<ROJO));
    LATA   |= ((1<<AMARILLO)|(1<<ROJO));
    
    while(1){
        LATA &= ~(1<<AMARILLO);
    }
            
}

