/*
 * Test de hardware — filas y columnas del 74HC595 — ESP32 / ESP32-S3
 *
 * Alterna dos estados complementarios para verificar ambos drivers de
 * compuerta:
 *   - Q0..Q3 → base de 2N2222 → gate de IRF9630 (canal P, filas, lado alto)
 *   - Q4..Q7 → gate directo de IRLZ44N (canal N, columnas, lado bajo)
 *
 * El 2N2222 invierte la señal para el IRF9630 (gate a masa = MOSFET
 * conduce), y esa inversión se cancela con la inversión propia del canal P
 * (gate en bajo = conduce), así que a nivel de bit enviado al 595 ambos
 * grupos usan la misma convención: 1 = salida activa.
 *
 * Estado A: filas activas (0x0F) / columnas apagadas
 * Estado B: columnas activas (0xF0) / filas apagadas  (0xF0 = ~0x0F)
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
#define ROWS_MASK 0xFF
#define COLS_MASK 0x00   // == (uint8_t)~ROWS_MASK

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

  Serial.println(F("== Test hardware: filas (Q0-3) vs columnas (Q4-7) =="));
}

void loop() {
  sendToShiftReg(ROWS_MASK);   // filas ON (IRF9630), columnas OFF (IRLZ44N)
  Serial.print(F("FILAS ON  / COLUMNAS OFF | byte=0x"));
  Serial.print(ROWS_MASK, HEX);
  Serial.print(F(" | Q7' cascada="));
  Serial.println(digitalRead(PIN_Q7));
  delay(1000);

  sendToShiftReg(COLS_MASK);   // filas OFF, columnas ON — patrón invertido
  Serial.print(F("FILAS OFF / COLUMNAS ON  | byte=0x"));
  Serial.print(COLS_MASK, HEX);
  Serial.print(F(" | Q7' cascada="));
  Serial.println(digitalRead(PIN_Q7));
  delay(1000);
}
