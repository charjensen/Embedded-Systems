//Pin Definitions 
const int PIN_TRIG = D4;
const int PIN_ECHO = D5;

//Constants 
const unsigned int MAX_ECHO_DURATION_US = 23200;  // Max valid pulse duration (~400 cm)
const unsigned long MEASUREMENT_DELAY_MS = 60;    // Delay between measurements

void setup() {
  // Initialize serial for output
  Serial.begin(115200);

  // Configure ultrasonic sensor pins
  pinMode(PIN_TRIG, OUTPUT);
  digitalWrite(PIN_TRIG, LOW);
  pinMode(PIN_ECHO, INPUT);
}

void loop() {
  float distance_cm = ping_cm();

  if (distance_cm < 0) {
    Serial.println("Out of Range");
  } else {
    Serial.printf("%.2f cm\n", distance_cm);
  }

  delay(MEASUREMENT_DELAY_MS);
}

float ping_cm() {
  // Send 10 µs pulse to trigger ultrasonic measurement
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Wait for echo to start
  unsigned long start_us = micros();
  while (digitalRead(PIN_ECHO) == LOW) {
    if (micros() - start_us > MAX_ECHO_DURATION_US) return -1;  // Timeout
  }
  unsigned long echo_start_us = micros();

  // Wait for echo to end
  while (digitalRead(PIN_ECHO) == HIGH) {
    if (micros() - echo_start_us > MAX_ECHO_DURATION_US) return -1;  // Timeout
  }
  unsigned long echo_end_us = micros();

  // Calculate duration and convert to distance
  unsigned long echo_duration_us = echo_end_us - echo_start_us;
  if (echo_duration_us > MAX_ECHO_DURATION_US) return -1;

  float distance_cm = echo_duration_us / 58.0;
  return distance_cm;
}
