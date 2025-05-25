#ifndef JUEGO_H
#define JUEGO_H

#include <Adafruit_ILI9341.h>
#include "../jugador/Jugador.h"
#include "../enemigos/Enemigos.h"
#include "../jefe/Jefe.h"
#include "../bala/Bala.h"

constexpr int MAX_BALAS = 3;
constexpr int MAX_ENEMIGOS = 6;
constexpr int MAX_BALAS_ENEMIGAS = 6;

enum EstadoJuego { JUGANDO, GAME_OVER, GANASTE };

class Juego {
public:
  Juego();

  void iniciar();
  void actualizar();
  void reiniciar();
  void mostrarVictoria();
  void dibujarUIBalas();


private:

  Jugador jugador;
  Enemigo enemigos[MAX_ENEMIGOS];
  Jefe jefe;
  Bala balasJugador[MAX_BALAS];
  Bala balasEnemigas[MAX_BALAS_ENEMIGAS];
  EstadoJuego estado;
  unsigned long ultimoDisparoEnemigo;
  unsigned long ultimoDisparoJefe;
   

  void manejarEntrada();
  void actualizarBalas();
  void actualizarBalasEnemigas();
  void dispararJugador();
  void dispararEnemigo();
  void dispararJefe();
  void verificarColisiones();
  void verificarJefe();

  friend class Jefe;  // para que Jefe pueda acceder a `estado` si es necesario
};

#endif
