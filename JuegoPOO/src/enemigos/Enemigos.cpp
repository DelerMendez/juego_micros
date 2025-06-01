#include "Enemigos.h"
#include "enemigo.h" 


Enemigo::Enemigo() {
  x = y = 0;
  vida = 4;
  activo = false;
}

void Enemigo::inicializar(int posX, int posY, int vidaInicial) {
  x = posX;
  y = posY;
  vida = vidaInicial;
  activo = true;
}

void Enemigo::dibujar(int frame) {
  if (activo) {
    screen.drawRGBBitmap(x, y, enemigoSprite[frame], ENEMIGO_TAM, ENEMIGO_TAM);
  }
}

void Enemigo::recibirDanio() {
  if (!activo) return;
  vida--;
  if (vida <= 0) {
    eliminar();
  }
}
bool Enemigo::estaVivo() const {
  return activo;
}

void Enemigo::eliminar() {
  screen.fillRect(x, y, ENEMIGO_TAM, ENEMIGO_TAM, ILI9341_BLACK);
  activo = false;
}
