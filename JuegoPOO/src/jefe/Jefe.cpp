#include "Jefe.h"
#include "boss.h"           // Contiene el sprite del jefe
#include "barraenemigo.h"   // Contiene barraen[] para la barra de vida
#include "../Juego/Juego.h"          // Para llamar a mostrarVictoria()

Jefe::Jefe() {
  x = y = 0;
  vida = 10;
  activo = false;
  dibujado = false;
  juego = nullptr;
}

void Jefe::asignarJuego(Juego* juegoPtr) {
  juego = juegoPtr;
}

void Jefe::inicializar(int posX, int posY) {
  x = posX;
  y = posY;
  vida = VIDA_MAX_JEFE;
  activo = true;
  dibujado = false;
}

void Jefe::dibujar() {
  if (activo && !dibujado) {
    screen.drawRGBBitmap(x, y, boss[0], JEFE_ANCHO, JEFE_ALTO);
    screen.drawRGBBitmap(x, 0, barraen[vida], JEFE_ANCHO, 10);
    dibujado = true;
  }
}

void Jefe::recibirDano() {
  if (!activo) return;

  vida--;
  if (vida <= 0) {
    activo = false;
    screen.fillRect(x, y, JEFE_ANCHO, JEFE_ALTO, ILI9341_BLACK);

    if (juego) {
      juego->mostrarVictoria();  // Delega la lógica de victoria al controlador principal
    }
    return;
  }

  // Actualiza sprite y barra de vida
  screen.drawRGBBitmap(x, y, boss[0], JEFE_ANCHO, JEFE_ALTO);
  screen.drawRGBBitmap(x, 0, barraen[vida], JEFE_ANCHO, 10);
}

void Jefe::actualizar() {
  if (activo) {
    dibujar();
  }
}

bool Jefe::estaVivo() const {
  return activo;
}

void Jefe::eliminar() {
  activo = false;
  screen.fillRect(x, y, JEFE_ANCHO, JEFE_ALTO, ILI9341_BLACK);
}
