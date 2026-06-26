
/*
 * Matrix LED multiplexing via 74HC595
 *
 * Columnas: activo BAJO  → Q0/Q1 en LOW enciende la columna
 * Filas:    activo BAJO  → Q2/Q3 en LOW enciende la fila
 * (ambos canales invierten la lógica en sendToShiftReg)
 *
 * ── Conexión 74HC595 ─────────────────────────────────────────
 *   Arduino D13 → DS   (pin 14, pata 14)  serial data
 *   Arduino D11 → SHCP (pin 11, pata 11)  shift clock
 *   Arduino D12 → STCP (pin 12, pata 12)  latch / storage clock
 *   VCC → VCC (pin 16), OEa (pin 13) a GND, SRCLR (pin 10) a VCC
 *
 * ── Salidas del 595 ──────────────────────────────────────────
 *   Q0 (pata 15) → columna 0   activo LOW
 *   Q1 (pata  1) → columna 1   activo LOW
 *   Q2 (pata  2) → fila    0   activo LOW
 *   Q3 (pata  3) → fila    1   activo LOW
 *
 * ── Resistencias recomendadas ────────────────────────────────
 *   100-470 Ω en serie entre cada salida Q y el gate del MOSFET
 *     → limita corriente de pico y evita oscilaciones en el gate
 *   10 kΩ pull-up a VCC en cada gate
 *     → garantiza MOSFET apagado durante arranque/reset del 595
 *
 * ── Bit layout del registro (LSB = Q0) ───────────────────────
 *   bits [ N_COLS-1 .. 0 ]            → columnas  (lógico: 1=activo)
 *   bits [ N_COLS+N_ROWS-1 .. N_COLS ]→ filas     (lógico: 1=activo)
 *   Ambos se invierten al enviar al 595 (activo LOW físico)
 *
 *   2×2:  [r1, r0, c1, c0]           → bits [3..0]
 *   4×4:  [r3,r2,r1,r0, c3,c2,c1,c0] → bits [7..0]  (un solo 595)
 *
 * ── Control individual de LEDs ───────────────────────────────
 *   setLED(col, row, state) modifica colMask/rowMask y envía.
 *   NOTA: en matriz NxN encender (c0,r0) Y (c1,r1) simultáneamente
 *   también ilumina (c0,r1) y (c1,r0) → "ghosting".
 *   Para evitarlo se usa multiplexing por fila (ver comentario al
 *   final del archivo).
 */

// ── Pines 74HC595 (adaptado a ESP8266 mini) ────────────────────
// Usar macros Dx (D0..D8) definidas por el core ESP8266
// Mapado sugerido (puedes cambiar según tu placa):
//   DATA (DS / MOSI) -> D7 (GPIO13)
//   CLOCK (SCK)       -> D5 (GPIO14)
//   LATCH (RCLK)      -> D6 (GPIO12)
// Evitar D0/D3/D4/D8 si afectan el arranque en tu módulo.
#define PIN_DATA   D7   // DS - serial data (MOSI)
#define PIN_LATCH  D6   // RCLK - latch / storage clock
#define PIN_CLOCK  D5   // SCLK - shift clock

// ── Dimensiones ─────────────────────────────────────────────────
const uint8_t N_COLS = 4;   // ← cambiar a 4 para escalar a 4×4
const uint8_t N_ROWS = 4;   // ← cambiar a 4 para escalar a 4×4

// ── Estado lógico global (1 = activo, independiente de polaridad)
uint8_t colMask = 0;
uint8_t rowMask = 0;

// ────────────────────────────────────────────────────────────────
// Envía colMask + rowMask al 74HC595
// Ambos canales son activo LOW → se invierte lógica antes de enviar
// ────────────────────────────────────────────────────────────────
void sendToShiftReg() {
  uint8_t colBitsPhys = (~colMask) & ((1 << N_COLS) - 1);  // activo LOW
  uint8_t rowBitsPhys = (~rowMask) & ((1 << N_ROWS) - 1);  // activo LOW
  uint8_t out = (rowBitsPhys << N_COLS) | colBitsPhys;

  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, LSBFIRST, out);
  digitalWrite(PIN_LATCH, HIGH);
}

// ────────────────────────────────────────────────────────────────
// Control individual de un LED
// ────────────────────────────────────────────────────────────────
void setLED(uint8_t col, uint8_t row, bool state) {
  if (col >= N_COLS || row >= N_ROWS) return;
  if (state) {
    colMask |=  (1 << col);
    rowMask |=  (1 << row);
  } else {
    colMask &= ~(1 << col);
    rowMask &= ~(1 << row);
  }
  sendToShiftReg();
}

// ────────────────────────────────────────────────────────────────
// Imprime uint8_t como "XXXX XXXX" (8 bits, MSB a la izquierda)
// ────────────────────────────────────────────────────────────────
void printBin8(uint8_t val) {
  for (int8_t i = 7; i >= 0; i--) {
    Serial.print((val >> i) & 1);
    if (i == 4) Serial.print(' ');
  }
}

// ────────────────────────────────────────────────────────────────
// Setup
// ────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(PIN_DATA,  OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);

  colMask = 0; rowMask = 0;
  sendToShiftReg();  // todo apagado

  Serial.println(F("== Matrix 74HC595 demo =="));
  Serial.print(F("Matriz: ")); Serial.print(N_COLS);
  Serial.print('x');           Serial.println(N_ROWS);
  Serial.println(F("idx | logic (lógico)  | phys (al 595)  | cols | rows"));
}

// ────────────────────────────────────────────────────────────────
// Demo: índice acumulativo recorre compuertas una a una
//   Orden: col0 → col1 → … → row0 → row1 → … → reset
//   Cada paso habilita UNA compuerta más y reporta por Serial.
// ────────────────────────────────────────────────────────────────
uint8_t demoIdx = 0;
const uint8_t TOTAL_GATES = N_COLS + N_ROWS;

void loop() {
  // Reconstruir máscaras según índice acumulativo
  colMask = 0; rowMask = 0;
  for (uint8_t i = 0; i <= demoIdx; i++) {
    if (i < N_COLS) colMask |=  (1 << i);
    else            rowMask |=  (1 << (i - N_COLS));
  }

  sendToShiftReg();

  // ── Reporte Serial ──────────────────────────────────────────
  // logicByte: [rowN..row0 | colN..col0] (sin invertir, 1 = activo)
  // physByte : lo que recibe físicamente el 74HC595
  uint8_t logicByte = (rowMask << N_COLS) | colMask;
  uint8_t physByte  = ((~rowMask & ((1 << N_ROWS) - 1)) << N_COLS)
                    | (~colMask & ((1 << N_COLS) - 1));  // ambos activo LOW

  Serial.print(F(" "));
  Serial.print(demoIdx);
  Serial.print(F("  | logic: ")); printBin8(logicByte);
  Serial.print(F("  | phys: "));  printBin8(physByte);
  Serial.print(F("  | cols=0b")); Serial.print(colMask, BIN);
  Serial.print(F("  rows=0b"));   Serial.println(rowMask, BIN);

  demoIdx = (demoIdx + 1) % TOTAL_GATES;
  delay(200);
}

/*
 * ── Multiplexing por fila (para control sin ghosting) ────────
 *
 * Para encender LEDs individuales sin ghosting, llamar en loop():
 *
 *   for (uint8_t r = 0; r < N_ROWS; r++) {
 *     // Sólo la fila r activa
 *     rowMask = (1 << r);
 *     // Columnas de esa fila según framebuffer[r]
 *     colMask = framebuffer[r];   // uint8_t framebuffer[N_ROWS]
 *     sendToShiftReg();
 *     delayMicroseconds(500);     // ~500 µs por fila → invisible al ojo
 *     colMask = 0; rowMask = 0;
 *     sendToShiftReg();           // apagar entre filas evita ghosting
 *   }
 *
 * framebuffer[0] = 0b01;  // fila 0: col0 encendida
 * framebuffer[1] = 0b10;  // fila 1: col1 encendida
 */
