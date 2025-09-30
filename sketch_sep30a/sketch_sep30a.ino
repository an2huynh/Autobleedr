// Blink the ESP32's GPIO12 (a.k.a. MTDI strap pin).
// IMPORTANT: Avoid pulling GPIO12 HIGH during reset. Let the board boot first.

const int PIN_OUT = 12;   // <-- GPIO number, not header index

void setup() {
  // Start LOW so we never risk driving it HIGH during early startup.
  pinMode(PIN_OUT, OUTPUT);
  digitalWrite(PIN_OUT, LOW);
  delay(200);  // let everything settle after boot
}

void loop() {
  digitalWrite(PIN_OUT, HIGH);  // drive high
  delay(500);
  digitalWrite(PIN_OUT, LOW);   // drive low
  delay(500);
}
