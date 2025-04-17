#ifndef GESTION_UART_H
#define GESTION_UART_H

// Tamaño del buffer de comandos
#define CMD_SIZE 20

// Variable global que indica si el sistema está activo (1) u OFF (0)
extern int estadoSistema;
extern int desactivacion;

// Inicializa UART y sistema base
void inicializarUARTySistema(void);

// Maneja recepción de datos por UART, eco y procesamiento
void manejarUART(void);

// Procesa el comando almacenado al pulsar Enter
void procesarComando(void);

#endif // GESTION_UART_H
