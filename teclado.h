#ifndef TECLADO_H
#define TECLADO_H

// Inicializa las líneas de filas y columnas
void teclado_init(void);

// Devuelve 0 si no hay tecla, o el carácter de la tecla pulsada
// con antirrebote y evitando múltiples. No bloqueante: si detecta
// una pulsación válida, espera a que se suelte antes de devolverla.
char teclado_getKey(void);

// Bloqueante: espera hasta que se pulse y suelte una tecla, y devuelve su carácter
char teclado_waitKey(void);

#endif // TECLADO_H
