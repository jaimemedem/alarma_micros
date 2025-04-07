#include <xc.h>
#include "Servo.h"
#include "gestionUART.h"
#include "TimerUtils.h"

#define _XTAL_FREQ 5000000  // Frecuencia de reloj actualizada a 5MHz

int main(void) {
    // Inicializar el UART y el sistema (habilita interrupciones, etc.)
    inicializarUARTySistema();
    
    // Inicializar el servo (configura PWM y timer)
    inicializaServo();
    
    // Configurar RA9 y RA8 como salidas digitales para los LEDs
    TRISAbits.TRISA9 = 0;  // RA9 como salida
    TRISAbits.TRISA8 = 0;  // RA8 como salida
    // Como los LEDs son activos a nivel bajo, se asigna 1 para apagarlos
    LATAbits.LATA9 = 1;    // LED en RA9 apagado
    LATAbits.LATA8 = 1;    // LED en RA8 apagado

    // Variable para detectar cambios en el estado recibido por UART
    int ultimoEstado = -1;

    while (1) {
        // Procesa la entrada UART (lee caracteres, arma el comando y lo procesa)
        manejarUART();

        // Cuando se reciba un comando nuevo (estadoSistema = 1 para ON o 0 para OFF)
        if ((estadoSistema == 1 || estadoSistema == 0) && (estadoSistema != ultimoEstado)) {
            if (estadoSistema == 1) {
                // Comando ON:
                // - Encender LED conectado en RA9 (activo a nivel bajo: poner en 0)
                // - Asegurar que el LED en RA8 permanezca apagado (poner en 1)
                // - Cerrar la puerta (mover el servo hasta el ángulo mínimo)
                LATAbits.LATA9 = 0;   // LED RA9 encendido
                LATAbits.LATA8 = 1;   // LED RA8 apagado
                cerrarPuerta();
                putsUART("\r\nEstado: ON - Puerta cerrada, LED RA9 encendido, LED RA8 apagado\r\n");
            } else if (estadoSistema == 0) {
                // Comando OFF:
                // - Abrir la puerta (mover el servo hasta el ángulo máximo)
                // - Apagar LED en RA9 (poner en 1)
                // - Encender LED en RA8 (poner en 0, por ser activo a nivel bajo)
                LATAbits.LATA9 = 1;   // LED RA9 apagado
                LATAbits.LATA8 = 0;   // LED RA8 encendido
                abrirPuerta();
                putsUART("\r\nEstado: OFF - Puerta abierta, LED RA9 apagado, LED RA8 encendido\r\n");
            }
            // Actualiza el último estado procesado para evitar reejecución innecesaria
            ultimoEstado = estadoSistema;
        }
    }
    
    return 0;
}
