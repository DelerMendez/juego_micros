#include "Enemigos.h"
#include "enemigo.h" // contiene enemigoSprite[]

// Constructor vacío
Enemigo::Enemigo() {
  x = y = 0;
  vida = 4;
  activo = false;
}

// Inicializa posición y vida
void Enemigo::inicializar(int posX, int posY, int vidaInicial) {
  x = posX;
  y = posY;
  vida = vidaInicial;
  activo = true;
}

// Dibuja el sprite del enemigo actual
void Enemigo::dibujar(int frame) {
  if (activo) {
    screen.drawRGBBitmap(x, y, enemigoSprite[frame], ENEMIGO_TAM, ENEMIGO_TAM);
  }
}

// Aplica daño al enemigo y lo elimina si la vida llega a 0
void Enemigo::recibirDanio() {
  if (!activo) return;
  vida--;
  if (vida <= 0) {
    eliminar();
  }
}

// Retorna si el enemigo está activo
bool Enemigo::estaVivo() const {
  return activo;
}

// Borra el enemigo de pantalla y lo desactiva
void Enemigo::eliminar() {
  screen.fillRect(x, y, ENEMIGO_TAM, ENEMIGO_TAM, ILI9341_BLACK);
  activo = false;
}
