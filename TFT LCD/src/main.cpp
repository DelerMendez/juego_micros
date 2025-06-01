#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include "soldadoidle.h"
#include "enemigo.h"
#include "balaenemigo.h"
#include "Sprite.h"
#include "balasUI.h"
#include "barravida.h"
#include "boss.h"
#include "barraenemigo.h"

#define TFT_DC   7
#define TFT_CS   6
#define TFT_MOSI 11
#define TFT_CLK  13
#define TFT_RST  10
#define TFT_MISO 12

#define BTN_IZQ A0
#define BTN_DER A1
#define BTN_DISPARO A2
#define BTN_REINICIAR A4
#define BUZZER_PIN 5

#define MAX_BALAS 3
#define VELOCIDAD_BALA 5

#define MAX_ENEMIGOS 6
#define MAX_BALAS_ENEMIGAS 6
#define VELOCIDAD_BALA_ENEMIGA 2

constexpr int SCREEN_WIDTH     = 320;
constexpr int SCREEN_HEIGHT    = 240;
constexpr int SPRITE_SIZE      = 32;
constexpr int MOVE_STEP        = 9;
constexpr int FRAME_INTERVAL   = 50;


bool jefea = false;
bool dibujar = true;
int vidaJefe = 10;
int j1x = 120, j1y = 40;




Adafruit_ILI9341 screen(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

enum Direction : uint8_t { LEFT = 1, RIGHT = 0 };

int x = 0, y = SCREEN_HEIGHT - SPRITE_SIZE, prevX = -1, prevY = -1, currentFrame = 0;
unsigned long lastFrameTime = 0;

bool balaActiva[MAX_BALAS] = { false };
int balaX[MAX_BALAS];
int balaY[MAX_BALAS];

bool balaEnemigaActiva[MAX_BALAS_ENEMIGAS] = { false };
int balaEnemigaX[MAX_BALAS_ENEMIGAS];
int balaEnemigaY[MAX_BALAS_ENEMIGAS];
int ve = 4;
bool enemigoActivo[MAX_ENEMIGOS] = { true, true, true, true, true, true };
int vidaEnemigo[MAX_ENEMIGOS] = { ve, ve, ve, ve, ve, ve };
int enemigoX[MAX_ENEMIGOS];
int enemigoY[MAX_ENEMIGOS];

Direction direccionActual = RIGHT;

bool disparando = false;
bool btnDisparoPrevio = false;

unsigned long ultimoDisparoEnemigo = 0;
int vidasJugador = 3;

const unsigned long INTERVALO_DISPARO_BASE = 2000;
void suenavic(){
  int melodia[] = {400, 600, 700, 1000};
  int duracion[] = {200,200,200,400};
  for (int repetir = 0;repetir<2; repetir++){
    for (int i = 0; i<4; i++){
      tone(BUZZER_PIN, melodia[i], duracion[i]);
      delay(duracion[i] *1.3);
    }
  }
  noTone(BUZZER_PIN);
}
void sonido(int freq1, int freq2,int duracion){
  tone(BUZZER_PIN, freq1,duracion);
  delay(30);
  tone(BUZZER_PIN, freq2, duracion);
  delay(30);
  noTone(BUZZER_PIN);
}



void dibujarBarraVida() {
  screen.drawRGBBitmap(0, 20, vida[vidasJugador], 28, 7);
}

bool colisionRect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
  return !(x1 + w1 < x2 || x1 > x2 + w2 || y1 + h1 < y2 || y1 > y2 + h2);
}

void moverPlayer(Direction dir) {
  direccionActual = dir;
  if (dir == LEFT && x >= MOVE_STEP) 
    x -= MOVE_STEP;
  else if (dir == RIGHT && x <= SCREEN_WIDTH - SPRITE_SIZE - MOVE_STEP)
    x += MOVE_STEP;
}

void animatePlayer(bool disparando) {
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

void dibujarEnemigos() {
  for (int i = 0; i < MAX_ENEMIGOS; i++) {
    if (enemigoActivo[i]) {
      screen.drawRGBBitmap(enemigoX[i], enemigoY[i], enemigoSprite[currentFrame], SPRITE_SIZE, SPRITE_SIZE);
    }
  }
}

int contarBalasActivas() {
  int count = 0;
  for (int i = 0; i < MAX_BALAS; i++) {
    if (balaActiva[i]) count++;
  }
  return count;
}

void dibujarIconoBalas() {
  static int balasPrevias = -1;
  int balasRestantes = MAX_BALAS - contarBalasActivas();
  if (balasRestantes != balasPrevias) {
    screen.fillRect(SCREEN_WIDTH - SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE, ILI9341_BLACK);
    screen.drawRGBBitmap(SCREEN_WIDTH - SPRITE_SIZE, 0, iconoBalas[balasRestantes], SPRITE_SIZE, SPRITE_SIZE);
    balasPrevias = balasRestantes;
  }
}
void reiniciarJuego() {
  dibujar = true;
  jefea = false;
  vidasJugador = 3;
  vidaJefe = 10;
  x = 0;
  y = SCREEN_HEIGHT - SPRITE_SIZE;
  prevX = -1;
  prevY = -1;
  currentFrame = 0;
  lastFrameTime = 0;

  for (int i = 0; i < MAX_BALAS; i++) balaActiva[i] = false;
  for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) balaEnemigaActiva[i] = false;

  for (int i = 0; i < MAX_ENEMIGOS; i++) {
    enemigoActivo[i] = true;
    vidaEnemigo[i] = ve;
    enemigoX[i] = 70 + (i % 3) * 50;
    enemigoY[i] = 30 + (i / 3) * 50;
  }

  screen.fillScreen(ILI9341_BLACK);
  animatePlayer(false);
  dibujarEnemigos();
  dibujarIconoBalas();
  dibujarBarraVida();
}

void actualizarBalas() {
  for (int i = 0; i < MAX_BALAS; i++) {
    if (!balaActiva[i]) continue;

    screen.fillRect(balaX[i], balaY[i], 9, 9, ILI9341_BLACK);
    balaY[i] -= VELOCIDAD_BALA;

    bool impacto = false;

    // Colisión con enemigos normales
    for (int j = 0; j < MAX_ENEMIGOS; j++) {
      if (enemigoActivo[j] && colisionRect(balaX[i], balaY[i], 9, 9, enemigoX[j], enemigoY[j], SPRITE_SIZE, SPRITE_SIZE)) {
        sonido(190, 200,40);
        vidaEnemigo[j]--;
        impacto = true;

        if (vidaEnemigo[j] <= 0) {
          screen.fillRect(enemigoX[j], enemigoY[j], SPRITE_SIZE, SPRITE_SIZE, ILI9341_BLACK);
          enemigoActivo[j] = false;
        }
        break;
      }
    }
    if (balaY[i] + 9 < 0 || impacto) {
      balaActiva[i] = false;
    } else {
      screen.drawRGBBitmap(balaX[i], balaY[i], balanemigo[0], 9, 9);
    }
  }
}
void actualizarBalasjefe(int &jx, int j1y, int &vidaJefe) {
  
  for (int i = 0; i < MAX_BALAS; i++) {
    if (!balaActiva[i]) continue;

    screen.fillRect(balaX[i], balaY[i], 9, 9, ILI9341_BLACK);
    balaY[i] -= VELOCIDAD_BALA;

    bool impacto = false;

    // Colisión con el jefe
    if (!impacto && jefea && colisionRect(balaX[i], balaY[i], 9, 9, jx, j1y, 60, 48)) {
      sonido(200,230,30);
      vidaJefe--;
      screen.drawRGBBitmap(jx, 0, barraen[vidaJefe], 60, 10);
      impacto = true;
      
      if (vidaJefe <= 0) {
        jefea = false;
        screen.fillRect(jx, j1y, 60, 48, ILI9341_BLACK);
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
        reiniciarJuego();
      } else {
        screen.drawRGBBitmap(jx, j1y, boss[0], 60, 48);
      }
    }

    if (balaY[i] + 9 < 0 || impacto) {
      balaActiva[i] = false;
    } else {
      screen.drawRGBBitmap(balaX[i], balaY[i], balanemigo[0], 9, 9);
    }
  }
}


void disparar() {
  for (int i = 0; i < MAX_BALAS; i++) {
    if (!balaActiva[i]) {
      balaActiva[i] = true;
      balaX[i] = x + SPRITE_SIZE / 2 - 4;
      balaY[i] = y - 12;
      sonido(1000,1200,30);
      break;
    }
  }
}

void dispararEnemigos() {
  int candidatos[3];
  int cantidad = 0;

  // Enemigo 4 → si está vivo, puede disparar; si no, lo hace el 1 si está activo
  if (enemigoActivo[3]) {
    candidatos[cantidad++] = 3;
  } else if (enemigoActivo[0]) {
    candidatos[cantidad++] = 0;
  }

  // Enemigo 5 → si está vivo, puede disparar; si no, lo hace el 2 si está activo
  if (enemigoActivo[4]) {
    candidatos[cantidad++] = 4;
  } else if (enemigoActivo[1]) {
    candidatos[cantidad++] = 1;
  }

  // Enemigo 6 → si está vivo, puede disparar; si no, lo hace el 3 si está activo
  if (enemigoActivo[5]) {
    candidatos[cantidad++] = 5;
  } else if (enemigoActivo[2]) {
    candidatos[cantidad++] = 2;
  }

  if (cantidad == 0) return;  // No hay nadie que pueda disparar

  // Elegir uno al azar entre los disponibles
  int idx = random(0, cantidad);
  int enemigoIdx = candidatos[idx];

  for (int j = 0; j < MAX_BALAS_ENEMIGAS; j++) {
    if (!balaEnemigaActiva[j]) {
      sonido(190,200,40);
      balaEnemigaActiva[j] = true;
      balaEnemigaX[j] = enemigoX[enemigoIdx] + SPRITE_SIZE / 2 - 4;
      balaEnemigaY[j] = enemigoY[enemigoIdx] + SPRITE_SIZE;
      return;
    }
  }
}



void actualizarBalasEnemigas() {
  for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
    if (!balaEnemigaActiva[i]) continue;

    screen.fillRect(balaEnemigaX[i], balaEnemigaY[i], 9, 9, ILI9341_BLACK);
    balaEnemigaY[i] += VELOCIDAD_BALA_ENEMIGA;

    if (colisionRect(balaEnemigaX[i], balaEnemigaY[i], 9, 9, x, y, SPRITE_SIZE, SPRITE_SIZE)) {
      Serial.println("¡Jugador alcanzado!");
      if (jefea == true){
        sonido(80,90,30);
        vidasJugador = vidasJugador - 3;
      }
      else{
        sonido(110,150,30);
        vidasJugador--;
      }
      
      dibujarBarraVida();
      balaEnemigaActiva[i] = false;

      if (vidasJugador <= 0) {

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
        reiniciarJuego();
      }
    } else if (balaEnemigaY[i] > SCREEN_HEIGHT) {
      balaEnemigaActiva[i] = false;
    } else {
      screen.drawRGBBitmap(balaEnemigaX[i], balaEnemigaY[i], balanemigo[0], 9, 9);
    }
    
  }
  

}

void dispararJefe(int jx, int jy) {
  for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
    if (!balaEnemigaActiva[i]) {
      sonido(500,540,50);
      balaEnemigaActiva[i] = true;
      balaEnemigaX[i] = jx + 30 - 4; // posición central del jefe (80 es X, 30 es ancho/2)
      balaEnemigaY[i] = jy + 48;     // parte inferior del jefe
      return;
    }
  }
}


void jefe(int jx, int jy) {
  static unsigned long ultimo = 0;
  unsigned long now = millis();
  

  if (!jefea) {
    
    bool grupo1 = enemigoActivo[3] || enemigoActivo[0];
    bool grupo2 = enemigoActivo[4] || enemigoActivo[1];
    bool grupo3 = enemigoActivo[5] || enemigoActivo[2];

    jefea = !(grupo1 || grupo2 || grupo3);  
  } else {
    unsigned long intervalo = max(500UL, 5000UL - now / 5000UL);

    if (now - ultimo >= intervalo) {
      if (dibujar) {
        screen.drawRGBBitmap(jx, jy, boss[0], 60, 48);
        screen.drawRGBBitmap(jx, 0, barraen[vidaJefe], 60, 10);
        dibujar = false;
      }
      ultimo = now;
    }
    static unsigned long ultimoDisparoJefe = 0;
    unsigned long intervaloDisparoJefe = 1500;  // o el valor que prefieras

    if (now - ultimoDisparoJefe >= intervaloDisparoJefe) {
      dispararJefe(jx, jy);
      ultimoDisparoJefe = now;
    }

  }
  actualizarBalasjefe(jx, jy, vidaJefe);
}
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_IZQ, INPUT_PULLUP);
  pinMode(BTN_DER, INPUT_PULLUP);
  pinMode(BTN_DISPARO, INPUT_PULLUP);
  pinMode(BTN_REINICIAR, INPUT_PULLUP);

  Serial.begin(9600);
  randomSeed(analogRead(A3));
  screen.begin();
  screen.setRotation(1);
  screen.fillScreen(ILI9341_BLACK);

  x = 0;
  y = SCREEN_HEIGHT - SPRITE_SIZE;
  prevX = x;
  prevY = y;

  for (int i = 0; i < MAX_ENEMIGOS; i++) {
    enemigoX[i] = 70 + (i % 3) * 50;
    enemigoY[i] = 30 + (i / 3) * 50;
  }

  animatePlayer(false);
  dibujarEnemigos();
  dibujarIconoBalas();
  dibujarBarraVida();
}

void loop() { 
  
  
  unsigned long now = millis();
  bool btnIzq = digitalRead(BTN_IZQ) == LOW;
  bool btnDer = digitalRead(BTN_DER) == LOW;
  bool btnActualDisparo = digitalRead(BTN_DISPARO) == LOW;
  bool disparoNuevo = btnActualDisparo && !btnDisparoPrevio;
  btnDisparoPrevio = btnActualDisparo;

  bool moved = false;

  if (btnIzq) { moverPlayer(LEFT); moved = true; }
  if (btnDer) { moverPlayer(RIGHT); moved = true; }

  disparando = btnActualDisparo;

  if (moved || disparando) {
    if (now - lastFrameTime >= FRAME_INTERVAL) {
      lastFrameTime = now;
      currentFrame = (currentFrame + 1) % 3;
    }
    animatePlayer(disparando);
  }

  if (disparoNuevo) {
    disparar();
  }

  actualizarBalas();
  
  dibujarIconoBalas();

  // Contar cuántos enemigos pueden disparar
  int enemigosDisponibles = 0;
  if (enemigoActivo[3] || enemigoActivo[0]) enemigosDisponibles++;
  if (enemigoActivo[4] || enemigoActivo[1]) enemigosDisponibles++;
  if (enemigoActivo[5] || enemigoActivo[2]) enemigosDisponibles++;

  unsigned long intervalo = INTERVALO_DISPARO_BASE - now / 5000UL;
  if (enemigosDisponibles <= 1) {
    intervalo += 1000;  // Aumenta el tiempo de disparo si solo queda uno
  }
  intervalo = max(500UL, intervalo);

  if (now - ultimoDisparoEnemigo >= intervalo) {
    dispararEnemigos();
    ultimoDisparoEnemigo = now;
  }
  
  actualizarBalasEnemigas();
  jefe(j1x,j1y);
  
} 