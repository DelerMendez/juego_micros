#ifndef JEFE_H
#define JEFE_H

#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 screen;

constexpr int JEFE_ANCHO = 60;
constexpr int JEFE_ALTO = 48;
constexpr int VIDA_MAX_JEFE = 10;

class Juego;  // ← Declaración adelantada

class Jefe {
public:
  int x, y;
  int vida;
  bool activo;
  bool dibujado;

  Jefe();

  void inicializar(int posX, int posY);
  void dibujar();
  void recibirDano();
  void actualizar();
  bool estaVivo() const;
  void eliminar();

  void asignarJuego(Juego* juegoPtr);  // ← Nueva función pública

private:
  Juego* juego = nullptr;  // ← Inicializado a null para seguridad
};

#endif
