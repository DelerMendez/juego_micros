#include "Bala.h"
#include "balasUI.h"  

Bala::Bala() {
  x = y = 0;
  activa = false;
  velocidad = 5;
  haciaArriba = true;
}

void Bala::dispararDesde(int origenX, int origenY, bool direccionArriba) {
  if (activa) return;

  x = origenX;
  y = origenY;
  activa = true;
  haciaArriba = direccionArriba;
}

bool Bala::mover() {
  if (!activa) return false;

  borrar();
  y += (haciaArriba ? -velocidad : velocidad);

  
  if (y < -BALA_TAM || y > 240 + BALA_TAM) {
    desactivar();
    return true; 
  }

  return false;   
}



void Bala::dibujar(const uint16_t* sprite) {
  if (activa) {
    screen.drawRGBBitmap(x, y, sprite, BALA_TAM, BALA_TAM);
  }
}


void Bala::borrar() {
  screen.fillRect(x, y, BALA_TAM, BALA_TAM, ILI9341_BLACK);
}

bool Bala::estaActiva() const {
  return activa;
}

void Bala::desactivar() {
    borrar();
    activa = false;
}
