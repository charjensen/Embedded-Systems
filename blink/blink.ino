// define led according to pin diagram in article
const int LED_PIN = D7;
const int WAIT_TIME = 125;
int led_state = LOW;

void setup() {
  // initialize digital pin led as an output
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  led_state = !led_state;                     // toggle between HIGH and LOW
  digitalWrite(LED_PIN, led_state);           // set the LED
  delay(WAIT_TIME);                           // wait
}