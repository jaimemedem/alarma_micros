#ifndef TECLADO_H
#define TECLADO_H

// Inicializa las l�neas de filas y columnas
void teclado_init(void);

// Devuelve 0 si no hay tecla, o el car�cter de la tecla pulsada
// con antirrebote y evitando m�ltiples. No bloqueante: si detecta
// una pulsaci�n v�lida, espera a que se suelte antes de devolverla.
char teclado_getKey(void);

// Bloqueante: espera hasta que se pulse y suelte una tecla, y devuelve su car�cter
char teclado_waitKey(void);

#endif // TECLADO_H
