//Constants
const int SENSOR = A0;
const int LED_PIN = D7;
const int THRESHOLD = 1000;

//Pin Setup
int led_state = HIGH;

void setup() {
  Serial.begin(115200);

  //Creating Pin inputs and outputs
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR, INPUT);

  //Initializing LED state
  digitalWrite(LED_PIN, led_state);  

}

void loop() {

  //Amount of samples
  const int SAMPLES = 10;

  //Value to be averaged at the end
  int accumulator = 0;

  //Collect 10 samples
  for (int i = 0; i < SAMPLES; i++) 
    accumulator += analogRead(SENSOR);

  //Calculate the average
  int avg = (accumulator /= SAMPLES);

  if (avg >= THRESHOLD) {
    Serial.printf("triggered\n");
    digitalWrite(LED_PIN, LOW);
  }
  else digitalWrite(LED_PIN, HIGH);

}
