# Multiplexado de Matriz LED con 74HC595

Control de una matriz de LEDs de alto voltaje mediante un registro de desplazamiento 74HC595,
con MOSFETs canal N (IRFZ44N) para columnas y canal P (IRF9630) para filas.

Actualmente configurado en **2×2**, escalable a **4×4** cambiando dos constantes.

---

## Hardware

### Componentes
| Componente | Función |
|---|---|
| Arduino (cualquier) | Microcontrolador |
| 74HC595 | Registro de desplazamiento (8 bits) |
| IRFZ44N × N_COLS | MOSFET canal N → columnas (lado bajo) |
| IRF9630 × N_ROWS | MOSFET canal P → filas (lado alto) |

### Conexión 74HC595

| Arduino | Pin 74HC595 | Señal |
|---------|-------------|-------|
| D11 | 14 (DS) | Serial data |
| D13 | 11 (SHCP) | Shift clock |
| D10 | 12 (STCP) | Latch / storage clock |
| VCC | 16 (VCC) | Alimentación |
| GND | 8 (GND) | Tierra |
| GND | 13 (OE) | Output enable (activo bajo) |
| VCC | 10 (SRCLR) | Clear (activo bajo, mantener HIGH) |

Las salidas Q0–Q(N_COLS-1) van a las compuertas de los IRFZ44N (columnas)
y Q(N_COLS)–Q(N_COLS+N_ROWS-1) van a las compuertas de los IRF9630 (filas).

---

## Bit layout del registro

```
2×2:  bit3  bit2  bit1  bit0
       r1    r0    c1    c0

4×4:  bit7  bit6  bit5  bit4  bit3  bit2  bit1  bit0
       r3    r2    r1    r0    c3    c2    c1    c0
```

- **Columnas (IRFZ44N canal N):** `1` = activo (MOSFET conduce)
- **Filas (IRF9630 canal P):** lógica `1` = activo, pero el firmware invierte automáticamente al enviar al 595 (`~rowMask`) porque el gate del P-channel requiere bajo para conducir

---

## Demo

El `loop()` habilita una compuerta más cada **200 ms** en orden acumulativo:

```
idx=0  c0           → col 0 habilitada
idx=1  c0 c1        → cols 0 y 1 habilitadas
idx=2  c0 c1 r0     → cols 0-1 + fila 0 habilitada
idx=3  c0 c1 r0 r1  → todo habilitado → reset
```

Salida Serial (115200 baud):
```
 0  | logic: 0000 0001  | phys: 0000 1101  | cols=0b1    rows=0b0
 1  | logic: 0000 0011  | phys: 0000 1111  | cols=0b11   rows=0b0
 2  | logic: 0000 0101  | phys: 0000 0101  | cols=0b1    rows=0b1
 3  | logic: 0000 0111  | phys: 0000 0111  | cols=0b11   rows=0b11
```
- **logic**: estado lógico (1 = activo, sin importar polaridad)
- **phys**: byte real enviado al 595 (filas invertidas para P-channel)

---

## Escalar a 4×4

Solo cambiar estas dos líneas en el sketch:

```cpp
const uint8_t N_COLS = 4;
const uint8_t N_ROWS = 4;
```

No hay más cambios necesarios. El cálculo de máscaras y el bit packing se adaptan automáticamente.

---

## Control individual de LEDs

```cpp
setLED(col, row, true);   // encender LED en columna col, fila row
setLED(col, row, false);  // apagar
```

> **Nota ghosting:** activar simultáneamente múltiples columnas y filas ilumina
> LEDs no deseados (ej: encender (c0,r0) y (c1,r1) también activa (c0,r1) y (c1,r0)).
> Ver la sección de multiplexing por fila al final del sketch para evitarlo.

---

## Multiplexing por fila (sin ghosting)

Para control de LEDs individuales sin ghosting, usar un framebuffer por fila:

```cpp
uint8_t framebuffer[N_ROWS] = { 0b01, 0b10 };  // fila0: col0 ON, fila1: col1 ON

for (uint8_t r = 0; r < N_ROWS; r++) {
    rowMask = (1 << r);
    colMask = framebuffer[r];
    sendToShiftReg();
    delayMicroseconds(500);   // tiempo de encendido por fila
    colMask = 0; rowMask = 0;
    sendToShiftReg();         // apagar entre filas evita ghosting
}
```

El ojo percibe todas las filas encendidas si el ciclo completo dura < ~20 ms.
