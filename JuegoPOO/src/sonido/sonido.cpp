#include "Sonido.h"

#define BUZZER_PIN 5  // Asegúrate de que coincida con tu definición en el archivo principal

void suenavic() {
  int melodia[] = {400, 600, 700, 1000};
  int duracion[] = {200, 200, 200, 400};
  for (int repetir = 0; repetir < 2; repetir++) {
    for (int i = 0; i < 4; i++) {
      tone(BUZZER_PIN, melodia[i], duracion[i]);
      delay(duracion[i] * 1.3);
    }
  }
  noTone(BUZZER_PIN);
}

void sonido(int freq1, int freq2, int duracion) {
  tone(BUZZER_PIN, freq1, duracion);
  delay(30);
  tone(BUZZER_PIN, freq2, duracion);
  delay(30);
  noTone(BUZZER_PIN);
}
