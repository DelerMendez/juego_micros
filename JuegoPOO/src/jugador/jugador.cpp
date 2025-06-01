#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include "Jugador.h"
#include "Sprite.h"
constexpr int SCREEN_WIDTH     = 320;
constexpr int SCREEN_HEIGHT    = 240;
constexpr int SPRITE_SIZE      = 32;
constexpr int MOVE_STEP        = 9;
constexpr int FRAME_INTERVAL   = 50;

#define TFT_DC   7
#define TFT_CS   6
#define TFT_MOSI 11
#define TFT_CLK  13
#define TFT_RST  10
#define TFT_MISO 12

Adafruit_ILI9341 screen(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

 

Jugador::Jugador() {
  reiniciar();
}

void Jugador::mover(Direction dir) {
  direccionActual = dir;
  if (dir == LEFT && x >= MOVE_STEP) 
    x -= MOVE_STEP;
  else if (dir == RIGHT && x <= SCREEN_WIDTH - SPRITE_SIZE - MOVE_STEP)
    x += MOVE_STEP;
}

void Jugador::animar(bool disparando) {
  static bool lastDisparando = false;
  static Direction lastDir = RIGHT;

  bool necesitaActualizar = (x != prevX || y != prevY || disparando != lastDisparando || direccionActual != lastDir);

  if (necesitaActualizar) {
    screen.fillRect(prevX, prevY, SPRITE_SIZE, SPRITE_SIZE, ILI9341_BLACK);
    if (disparando) {
      screen.drawRGBBitmap(x, y, soldadodisparo[0], SPRITE_SIZE, SPRITE_SIZE);
    } else {
      screen.drawRGBBitmap(x, y, soldadoquieto[direccionActual], SPRITE_SIZE, SPRITE_SIZE);
    }
    prevX = x;
    prevY = y;
    lastDisparando = disparando;
    lastDir = direccionActual;
  }
}

void Jugador::dibujarVida() {
  screen.drawRGBBitmap(0, 20, vida[vidas], 28, 7);
}

void Jugador::reiniciar() {
  x = 0;
  y = SCREEN_HEIGHT - SPRITE_SIZE;
  frame = 0;
  vidas = 3;
  prevX = -1;
  prevY = -1;
  currentFrame = 0;
  lastFrameTime = 0;
  disparando = false;
}
