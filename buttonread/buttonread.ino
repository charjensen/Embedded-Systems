//Pins
const int PIN_BUTTON = D6;
const int PIN_LED = D7;

//Constants
const unsigned long INACTIVITY_TIMEOUT_MS = 1000;

//Global Variables
// Tracks current button state
int g_button_state = 0;

// Last time the button was pressed
unsigned long g_button_last_pressed_at_ms = 0;

// Last time inactivity was printed
unsigned long g_last_print_time_ms = 0;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_LED, LOW);
}

void loop() {
  g_button_state = digitalRead(PIN_BUTTON);
  unsigned long current_time_ms = millis();

  if (g_button_state == HIGH) {
    // Button is pressed — turn LED on and update last press time
    digitalWrite(PIN_LED, HIGH);
    g_button_last_pressed_at_ms = current_time_ms;
  } else {
    // Button not pressed — turn LED off
    digitalWrite(PIN_LED, LOW);
  }

  // If more than 1 second has passed since last press and last message
  if ((current_time_ms - g_button_last_pressed_at_ms > INACTIVITY_TIMEOUT_MS) &&
      (current_time_ms - g_last_print_time_ms > INACTIVITY_TIMEOUT_MS)) {
    Serial.printf("No state change since %lu ms\n", g_button_last_pressed_at_ms);
    g_last_print_time_ms = current_time_ms;
  }
}
