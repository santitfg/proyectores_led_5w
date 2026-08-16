/*
 * Test de hardware — barrido de las 16 combinaciones fila×columna — ESP32 / ESP32-S3
 *
 * Recorre las 4 filas × 4 columnas (16 combinaciones), encendiendo una
 * fila y una columna a la vez para verificar cada cruce de la matriz:
 *   - Q0..Q3 → base de 2N2222 → gate de IRF9630 (canal P, filas, lado alto)
 *   - Q4..Q7 → gate directo de IRLZ44N (canal N, columnas, lado bajo)
 *
 * En ambos casos la señal es ALTA para habilitar el paso de corriente:
 *   - Filas (IRF9630, high side): el 2N2222 invierte la señal, por lo
 *     que un "1" en Q0..Q3 satura el 2N2222, que a su vez tira el gate
 *     del IRF9630 a GND y lo enciende.
 *   - Columnas (IRLZ44N, low side): un "1" en Q4..Q7 lleva el gate
 *     directo a VCC y lo enciende.
 *
 * ── Conexión 74HC595 (Conector 1, ver README.md del proyecto) ──
 *              Señal        ESP32     ESP32-S3
 *   Pin 2   SRCLK / CP   gpio23      gpio18   (Shift Clock, pata 11)
 *   Pin 3   RCLK  / PL   gpio22      gpio10   (Latch,       pata 12)
 *   Pin 4   SER   / DS   gpio2       gpio4    (Data in,     pata 14)
 *   Pin 1   GND          —           —
 *   Pin 5   VCC          —           —
 *
 * OE (pata 13) a GND y SRCLR (pata 10) a VCC (fijo, no controlado
 * desde el ESP32).
 *
 * ── Conector 2 (cascada) ────────────────────────────────────────
 *   Pin 4   Q7    gpio35      gpio5    (Q7', pata 9, hacia SER del siguiente 595)
 *   Se lee como entrada para capturar el bit que sale por cascada.
 *
 * El target (ESP32 clásico vs ESP32-S3) se detecta automáticamente
 * con CONFIG_IDF_TARGET_ESP32S3, definido por el core de Arduino.
 */

#if CONFIG_IDF_TARGET_ESP32S3
  #define PIN_SRCLK 18   // Shift clock
  #define PIN_RCLK  10   // Latch
  #define PIN_SER    4   // Serial data in
  #define PIN_Q7     5   // Cascade out (Q7')
#else
  #define PIN_SRCLK 23   // Shift clock
  #define PIN_RCLK  22   // Latch
  #define PIN_SER    2   // Serial data in
  #define PIN_Q7    35   // Cascade out (Q7')
#endif

// Q0..Q3 → filas (2N2222 + IRF9630) | Q4..Q7 → columnas (IRLZ44N directo)
#define N_ROWS 4
#define N_COLS 4

// ────────────────────────────────────────────────────────────────
// Envía un byte al 74HC595 y actualiza el latch
// ────────────────────────────────────────────────────────────────
void sendToShiftReg(uint8_t value) {
  digitalWrite(PIN_RCLK, LOW);
  shiftOut(PIN_SER, PIN_SRCLK, LSBFIRST, value);
  digitalWrite(PIN_RCLK, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_SER,   OUTPUT);
  pinMode(PIN_SRCLK, OUTPUT);
  pinMode(PIN_RCLK,  OUTPUT);
  pinMode(PIN_Q7,    INPUT);   // Q7' de cascada, gpio35/gpio5 sólo lectura

  sendToShiftReg(0x00);  // todas las salidas apagadas

  Serial.println(F("== Test hardware: barrido de las 16 combinaciones fila x columna =="));
}

void loop() {
  for (uint8_t row = 0; row < N_ROWS; row++) {
    for (uint8_t col = 0; col < N_COLS; col++) {
      uint8_t out = (uint8_t)(1 << row) | (uint8_t)(1 << (N_ROWS + col));
      sendToShiftReg(out);

      Serial.print(F("FILA "));
      Serial.print(row);
      Serial.print(F(" / COLUMNA "));
      Serial.print(col);
      Serial.print(F(" | byte=0x"));
      Serial.print(out, HEX);
      Serial.print(F(" | Q7' cascada="));
      Serial.println(digitalRead(PIN_Q7));

      delay(500);
    }
  }
}
