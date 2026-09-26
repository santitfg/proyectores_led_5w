const int dataPin = 23;   // SER (DS)
const int clockPin = 18;  // SRCLK (SHCP)
const int latchPin = 5;   // RCLK (STCP)

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
}

void loop() {
  byte datos = 0b10101010; // Enciende alternadamente los pines Q0 a Q7
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, datos);
  digitalWrite(latchPin, HIGH);
  
  delay(1000);

  datos = 0b01010101; 
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, datos);
  digitalWrite(latchPin, HIGH);
  
  delay(1000);
  datos = 0b00000000; 
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, datos);
  digitalWrite(latchPin, HIGH);
  
  delay(1000);

}