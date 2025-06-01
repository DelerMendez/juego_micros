#include "../Juego/Juego.h"
Direction direccionActual = RIGHT;
Juego juego;

void setup() {
  juego.iniciar();
}
void loop() {
  juego.actualizar();
}
