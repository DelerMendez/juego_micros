#include "Juego.h"
#include <Arduino.h>
#include "balasUI.h"
#include "Sprite.h"
#include "barraenemigo.h"
#include "balaenemigo.h"
#include "../utils/Utils.h"
#include "../sonido/sonido.h"

extern Adafruit_ILI9341 screen;
constexpr int SPRITE_SIZE = 32;

#define BTN_IZQ A0
#define BTN_DER A1
#define BTN_DISPARO A2
#define BTN_REINICIAR A4
#define vidaenemigo 2

Juego::Juego() :
 estado(JUGANDO),
 ultimoDisparoEnemigo(0), 
 ultimoDisparoJefe(0)
 {}

void Juego::iniciar() {
  pinMode(BTN_IZQ, INPUT_PULLUP);
  pinMode(BTN_DER, INPUT_PULLUP);
  pinMode(BTN_DISPARO, INPUT_PULLUP);
  pinMode(BTN_REINICIAR, INPUT_PULLUP);

  screen.begin();
  screen.setRotation(1);
  screen.fillScreen(ILI9341_BLACK);

  jugador.reiniciar();
  jefe.asignarJuego(this);  

  for (int i = 0; i < MAX_ENEMIGOS; i++) {
    int posX = 70 + (i % 3) * 50;
    int posY = 30 + (i / 3) * 50;
    enemigos[i].inicializar(posX, posY, vidaenemigo);
  }

  jugador.animar(false);
  for (int i = 0; i < MAX_ENEMIGOS; i++) enemigos[i].dibujar(0);


  jugador.dibujarVida();
  dibujarUIBalas(); 
}

void Juego::actualizar() {
  if (estado != JUGANDO) return;

  manejarEntrada();
  actualizarBalas();
  actualizarBalasEnemigas();
  verificarColisiones();
  verificarJefe();
}

void Juego::reiniciar() {
  estado = JUGANDO;
  screen.fillScreen(ILI9341_BLACK);
  jugador.reiniciar();

  jefe = Jefe();             
  jefe.asignarJuego(this);   

  for (int i = 0; i < MAX_ENEMIGOS; i++) {
    int posX = 70 + (i % 3) * 50;
    int posY = 30 + (i / 3) * 50;
    enemigos[i].inicializar(posX, posY, vidaenemigo);
  }
  for (int i = 0; i < MAX_BALAS; i++) {
    balasJugador[i].desactivar();  
    }


  jugador.animar(false);
  for (int i = 0; i < MAX_ENEMIGOS; i++) enemigos[i].dibujar(0);
  jugador.dibujarVida();
  dibujarUIBalas(); 
  delay(300);
}

void Juego::manejarEntrada() {
  bool izq = digitalRead(BTN_IZQ) == LOW;
  bool der = digitalRead(BTN_DER) == LOW;
  bool disparo = digitalRead(BTN_DISPARO) == LOW;

  if (izq) jugador.mover(LEFT);
  if (der) jugador.mover(RIGHT);
  jugador.disparando = disparo;

  static bool btnDisparoPrevio = false;
  bool nuevoDisparo = disparo && !btnDisparoPrevio;
  btnDisparoPrevio = disparo;

  if (nuevoDisparo) dispararJugador();

  jugador.animar(disparo);
}

void Juego::dispararJugador() {
  sonido(1000,1200,30);
  for (int i = 0; i < MAX_BALAS; i++) {
    if (!balasJugador[i].estaActiva()) {
      balasJugador[i].dispararDesde(jugador.x + 14, jugador.y - 12, true);
      break;
    }
  }
   dibujarUIBalas(); 
}

void Juego::dispararEnemigo() {
  int candidatos[3];
  int cantidad = 0;

  if (enemigos[3].estaVivo()) 
    candidatos[cantidad++] = 3;
  else if (enemigos[0].estaVivo()) 
    candidatos[cantidad++] = 0;
  if (enemigos[4].estaVivo())
    candidatos[cantidad++] = 4;
  else if (enemigos[1].estaVivo()) 
    candidatos[cantidad++] = 1;
  if (enemigos[5].estaVivo()) 
    candidatos[cantidad++] = 5;
  else if (enemigos[2].estaVivo()) 
    candidatos[cantidad++] = 2;

  if (cantidad == 0) return;
  int idx = random(0, cantidad);
  int e = candidatos[idx];

  for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
    if (!balasEnemigas[i].estaActiva()) {
      sonido(190,200,40);
      balasEnemigas[i].dispararDesde(enemigos[e].x + 14, enemigos[e].y + SPRITE_SIZE, false);
      return;
    }
  }
}

void Juego::dispararJefe() {
  for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
    sonido(120,90,30);
    if (!balasEnemigas[i].estaActiva()) {
      balasEnemigas[i].dispararDesde(jefe.x + 28, jefe.y + JEFE_ALTO, false);
      return;
    }
  }
}

void Juego::actualizarBalas() {
  for (int i = 0; i < MAX_BALAS; i++) {
    bool desactivada = balasJugador[i].mover();
    balasJugador[i].dibujar(balanemigo[0]);

    if (desactivada) {
      dibujarUIBalas();  
    }
  }
}


void Juego::actualizarBalasEnemigas() {
  for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
    
    balasEnemigas[i].mover();
    balasEnemigas[i].dibujar(balanemigo[0]);

    if (balasEnemigas[i].estaActiva() && colisionRect(balasEnemigas[i].x, balasEnemigas[i].y, 9, 9,jugador.x, jugador.y, SPRITE_SIZE, SPRITE_SIZE)) {
      jugador.vidas -= jefe.estaVivo() ? 3 : 1;
      jugador.dibujarVida();
      balasEnemigas[i].desactivar();

      if (jugador.vidas <= 0) {
        screen.fillScreen(ILI9341_BLACK);
        screen.setCursor(80, 100);
        screen.setTextColor(ILI9341_RED);
        screen.setTextSize(3);
        screen.print("GAME OVER");

        screen.setCursor(30, 140);
        screen.setTextSize(2);
        screen.setTextColor(ILI9341_WHITE);
        screen.print("Presiona para reiniciar");

        while (digitalRead(BTN_REINICIAR) != LOW);
        delay(500);
        reiniciar();
      }
    }
  }
}

void Juego::verificarColisiones() {
  for (int i = 0; i < MAX_BALAS; i++) {
    if (!balasJugador[i].estaActiva()) continue;

    for (int j = 0; j < MAX_ENEMIGOS; j++) {
      if (enemigos[j].estaVivo() && colisionRect(balasJugador[i].x, balasJugador[i].y, 9, 9,enemigos[j].x, enemigos[j].y, SPRITE_SIZE, SPRITE_SIZE)) {
        sonido(80,90,30);
        enemigos[j].recibirDanio();
        balasJugador[i].borrar();
        balasJugador[i].desactivar();
        dibujarUIBalas();
        break;
      }
    }

    if (jefe.estaVivo() &&colisionRect(balasJugador[i].x, balasJugador[i].y, 9, 9,jefe.x, jefe.y, JEFE_ANCHO, JEFE_ALTO)) {
      sonido(80,90,30);
      jefe.recibirDano();
      balasJugador[i].borrar();
      balasJugador[i].desactivar();
      dibujarUIBalas();
    }
  }
}
void Juego::dibujarUIBalas() {
  int balasDisponibles = 0;
  for (int i = 0; i < MAX_BALAS; i++) {
    if (!balasJugador[i].estaActiva()) {
      balasDisponibles++;
    }
  }

  screen.fillRect(270, 20, 42, 7, ILI9341_BLACK);

  balasDisponibles = constrain(balasDisponibles, 0, 3);

  screen.drawRGBBitmap(270, 20, iconoBalas[balasDisponibles], 32, 32);
}


void Juego::verificarJefe() {
  if (!jefe.estaVivo()) {
    bool grupo1 = enemigos[3].estaVivo() || enemigos[0].estaVivo();
    bool grupo2 = enemigos[4].estaVivo() || enemigos[1].estaVivo();
    bool grupo3 = enemigos[5].estaVivo() || enemigos[2].estaVivo();

    if (!(grupo1 || grupo2 || grupo3)) {
      jefe.inicializar(120, 40);
    }
  } else {
    jefe.actualizar();
    unsigned long now = millis();

    if (now - ultimoDisparoJefe >= 1500) {
      dispararJefe();
      ultimoDisparoJefe = now;
    }
  }

  unsigned long now = millis();
  unsigned long intervalo = max(500UL, 2000UL - now / 5000UL);
  if (now - ultimoDisparoEnemigo >= intervalo) {
    dispararEnemigo();
    ultimoDisparoEnemigo = now;
  }
}

void Juego::mostrarVictoria() {
  estado = GANASTE;
  suenavic();
  screen.fillScreen(ILI9341_BLACK);
  screen.setCursor(80, 100);
  screen.setTextColor(ILI9341_GREEN);
  screen.setTextSize(3);
  screen.print("¡GANASTE!");

  screen.setCursor(30, 140);
  screen.setTextSize(2);
  screen.setTextColor(ILI9341_WHITE);
  screen.print("Presiona para reiniciar");

  while (digitalRead(BTN_REINICIAR) != LOW);
  delay(500);
  reiniciar();
}
