/*
 * Secuencia de compuertas Q0..Q7 de un 74HC595 — ESP32 / ESP32-S3
 *
 * Enciende y apaga todas las salidas del 74HC595 a la vez.
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

  Serial.println(F("== Secuencia Q0..Q7 74HC595 =="));
}

void loop() {
  sendToShiftReg(0xFF);   // enciende Q0..Q7
  Serial.print(F("ALL ON  | Q7' cascada="));
  Serial.println(digitalRead(PIN_Q7));
  delay(2000);

  sendToShiftReg(0x00);   // apaga Q0..Q7
  Serial.print(F("ALL OFF | Q7' cascada="));
  Serial.println(digitalRead(PIN_Q7));
  delay(1000);
}
