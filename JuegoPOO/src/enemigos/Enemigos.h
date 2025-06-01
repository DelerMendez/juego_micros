#ifndef ENEMIGOS_H
#define ENEMIGOS_H

#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 screen;

constexpr int ENEMIGO_TAM = 32;

class Enemigo {
public:
  int x, y;
  int vida;
  bool activo;

  Enemigo();
  void inicializar(int posX, int posY, int vidaInicial);
  void dibujar(int frame);
  void recibirDanio();
  bool estaVivo() const;
  void eliminar();
};

#endif
