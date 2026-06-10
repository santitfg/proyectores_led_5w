
// Columnas Mosfet Canal N - IRFZ44N _ lado bajo de cambio
// pata en alto permite el paso de correinte
int pinesColumnas[2] = { 3, 4 };

// Filas Canal P - IRF9630 _ lado alto de cambio
// pata en bajo permite el paso de correinte
int pinesFilas[2] = { 8, 9 };

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < sizeof(pinesColumnas) / sizeof(pinesColumnas[0]); i++) {
    pinMode(pinesColumnas[i], OUTPUT);
  }
  for (int i = 0; i < sizeof(pinesFilas) / sizeof(pinesFilas[0]); i++) {
    pinMode(pinesFilas[i], OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
