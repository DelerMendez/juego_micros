#ifndef JUGADOR_H
#define JUGADOR_H

#include <Adafruit_ILI9341.h>
#include "soldadoidle.h"
#include "balasUI.h"
#include "barravida.h"
enum Direction : uint8_t { LEFT = 1, RIGHT = 0 };
extern Direction direccionActual;

class Jugador {
public:
  int x, y;
  int vidas;
  int frame;
  int prevX;
  int prevY;
  int currentFrame;
  unsigned long lastFrameTime;
  bool disparando;
  

  Jugador();
  void mover(Direction);
  void animar(bool disparando);
  void dibujarVida();
  void reiniciar();
};

#endif
