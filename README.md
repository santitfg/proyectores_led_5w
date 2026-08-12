### Conector 1 — entrada de datos serie (SER)

Chip: 74HC595 (registro de desplazamiento serie-in / paralelo-out). Molex 5 pines.

| Pin | Color | Señal | GPIO ESP32 | GPIO ESP32-S3 | Descripción |
|-----|-------|-------|------------|---------------|-------------|
| 1   | ⚫ Negro  | GND   | —          | —             | Tierra |
| 2   | ⚪ Blanco | SRCLK / CP | `gpio23`   | `gpio18`      | Shift Clock (SH_CP, pin 11) / SPI2 SCLK |
| 3   | 🟢 Verde  | RCLK / PL | `gpio22`   | `gpio10`      | Register Clock / Latch (ST_CP, pin 12) |
| 4   | 🩶 Gris   | SER  / DS | `gpio2`    | `gpio4`       | Serial data in (DS, pin 14) / SPI2 MOSI |
| 5   | 🔴 Rojo   | VCC   | —          | —             | Alimentación |

### Conector 2 — salida en cascada (Q7)

Conector para encadenar el `SER` (pin 14) del siguiente 74HC595 a la salida en cascada `Q7'` (pin 9) de este.

| Pin | Color | Señal | GPIO ESP32 | GPIO ESP32-S3 | Descripción |
|-----|-------|-------|------------|---------------|-------------|
| 1   | ⚫ Negro  | GND   | —          | —             | Tierra |
| 2   | ⚪ Blanco | SRCLK  / CP| `gpio23`   | `gpio18`      | Shift Clock (SH_CP, pin 11) / SPI2 SCLK |
| 3   | 🟢 Verde  | RCLK  / PL | `gpio22`   | `gpio10`      | Register Clock / Latch (ST_CP, pin 12) |
| 4   | 🩶 Gris   | Q7   | `gpio35`   | `gpio5`       | Salida en cascada (Q7', pin 9) hacia el `SER` del siguiente 74HC595 |
| 5   | 🔴 Rojo   | VCC   | —          | —             | Alimentación |
