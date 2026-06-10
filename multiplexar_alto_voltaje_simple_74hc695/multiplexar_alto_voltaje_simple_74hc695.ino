
// Columnas Mosfet Canal N - IRFZ44N _ lado bajo de cambio
// pata en alto permite el paso de correinte
int pinesColumnas[2] = { 3, 4 };
int nColumnas = 0;
int nFilas = 0;
// Filas Canal P - IRF9630 _ lado alto de cambio
// pata en bajo permite el paso de correinte
int pinesFilas[2] = { 8, 9 };

void setup() {
  nColumnas = sizeof(pinesColumnas) / sizeof(pinesColumnas[0]);
  nFilas = sizeof(pinesFilas) / sizeof(pinesFilas[0]);
  // put your setup code here, to run once:
  for (int i = 0; i < nColumnas; i++) {
    pinMode(pinesColumnas[i], OUTPUT);
  }
  for (int i = 0; i < nFilas; i++) {
    pinMode(pinesFilas[i], OUTPUT);
  }
}

void loop() {

  for (int i = 0; i < nColumnas; i++) {
    for (int j = 0; j < nFilas; j++) {
      digitalWrite(pinesColumnas[i], HIGH);
      digitalWrite(pinesFilas[j], HIGH);
      delay(600);
      digitalWrite(pinesColumnas[i], LOW);
      digitalWrite(pinesFilas[j], LOW);
      delay(200);
    }
  }
  // largoBits=
  // bitsColumnas=
  // bitsFilas=
}
