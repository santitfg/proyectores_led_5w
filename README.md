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

estoy controlando con el 74hc595 la matriz desde las siguientes filas y columnas:
Q0 ... Q3 se conectan a los 2n2222 controlanto el gate de IRF9630
Q4 ... Q7 se conectan directo al gate IRLZ44N


# revision de laburo


PCB

IRF9630 high side y esta a up down en gate
IRLZ44N	low side y esta a pull down en gate

————

revision en protoboard 5v y señales activas de irlz44n y 2n2222 a 3.3:
IRLZ44N
gate ( pin 1) 10k a pull Down 1k a señal (vcc activa el paso de corriente)
drain ( pin 2) a gnd-terminal
source (pin 3) a GND

IRF9630
gate ( pin 1) 10k a pull up 1k a señal (gnd activa el paso de corriente)(2n2222)
drain ( pin 2) a Vout-terminal
source (pin 3) a VIN

2n2222 ( funcionamiento real es emisor a gnd y colector lado vcc)
emisor (pin 1) a GND
base (pin 2) señal ic ( via 1k Resi)
colector (pin 3) ( señal hacia irf9630 via 1k resi) vcc activa el paso de corriente)x

TODO: arreglar kicad corrigiendo los elementos mencionados (inverti pista VIN x Vout-terminal)

Fallas en el circuito funciona la mitad de puntos en modo damero
O X O X
X O X O 
O X O X
X O X O

Nomenclar correstamente las columnas y filas 
pero los v y gnd en los mismos no todos fallan de la misma manera
1: 2: 3: 4: 



Filas
q0 (pin 15) : IRF9630 1
q1 (pin 1) : IRF9630 2
q2 (pin 2) : IRF9630 3
q3 (pin 3) : IRF9630 4

Columnas
q4 (pin 4) : IRLZ44N 1
q5 (pin 5) : IRLZ44N 2
q6 (pin 6) : IRLZ44N 3
q7 (pin 7) : IRLZ44N 4

.   q4  q3  q2  q1
q3  4   3   2   1
q2  8   7   6   5
q1  12  11  10  9
q0  16  15  14  13     

funciona raro al enviar 0xFF se apagan las salidas y al 0x00 se prenden