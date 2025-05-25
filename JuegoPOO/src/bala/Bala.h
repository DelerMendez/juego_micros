#ifndef BALA_H
#define BALA_H

#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 screen;

constexpr int BALA_TAM = 9;

class Bala {
public:
  int x, y;
  bool activa;
  int velocidad;
  bool haciaArriba;

  Bala();

  void dispararDesde(int origenX, int origenY, bool direccionArriba);
  bool mover();
  void dibujar(const uint16_t* sprite);
  void borrar();
  bool estaActiva() const;
  void desactivar();
  
};

#endif
