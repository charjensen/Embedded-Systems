// Pin Definitions
const int PIN_TRIG = D4;
const int PIN_ECHO = D5;
const int PIN_BUTTON = D6;

// Constants
const unsigned int MAX_ECHO_DURATION_US = 23200;        // Max echo duration (~400 cm)
const unsigned long SAMPLE_INTERVAL_MS = 20;            // 50 Hz sample rate (20 ms)

// Global State Variables
bool g_button_was_pressed = false;                      // Tracks previous button state
unsigned long g_button_press_start_ms = 0;              // Time when button was pressed
unsigned long g_last_sample_time_ms = 0;                // Last sampling timestamp

// Sampling Data
float g_distance_sum_cm = 0;
int g_distance_sample_count = 0;
float g_velocity_sum_cms = 0;
int g_velocity_sample_count = 0;
float g_acceleration_sum_cmss = 0;
int g_acceleration_sample_count = 0;

// Previous values for velocity and acceleration
float g_last_distance_cm = 0;
float g_last_velocity_cms = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
}

void loop() {
  bool is_button_pressed = (digitalRead(PIN_BUTTON) == HIGH);
  unsigned long current_time_ms = millis();

  if (is_button_pressed) {
    if (!g_button_was_pressed) {
      // Button just pressed — reset all accumulated data
      g_button_press_start_ms = current_time_ms;
      g_distance_sum_cm = 0;
      g_distance_sample_count = 0;
      g_velocity_sum_cms = 0;
      g_velocity_sample_count = 0;
      g_acceleration_sum_cmss = 0;
      g_acceleration_sample_count = 0;
      g_last_distance_cm = 0;
      g_last_velocity_cms = 0;
    }

    // Sample at fixed interval
    if (current_time_ms - g_last_sample_time_ms >= SAMPLE_INTERVAL_MS) {
      float current_distance_cm = ping_cm();

      if (current_distance_cm > 0) {
        float delta_time_s = (current_time_ms - g_last_sample_time_ms) / 1000.0;

        // Calculate velocity and acceleration
        float velocity_cms = (current_distance_cm - g_last_distance_cm) / delta_time_s;
        float acceleration_cmss = (velocity_cms - g_last_velocity_cms) / delta_time_s;

        // Accumulate values
        g_distance_sum_cm += current_distance_cm;
        g_distance_sample_count++;

        g_velocity_sum_cms += velocity_cms;
        g_velocity_sample_count++;

        g_acceleration_sum_cmss += acceleration_cmss;
        g_acceleration_sample_count++;

        // Update previous values
        g_last_distance_cm = current_distance_cm;
        g_last_velocity_cms = velocity_cms;
      }

      g_last_sample_time_ms = current_time_ms;
    }

    g_button_was_pressed = true;
  } else {
    if (g_button_was_pressed) {
      // Button just released
      unsigned long press_duration_ms = current_time_ms - g_button_press_start_ms;

      if (g_distance_sample_count > 0) {
        float average_distance_cm = g_distance_sum_cm / g_distance_sample_count;
        float average_velocity_cms = g_velocity_sample_count > 0 ? g_velocity_sum_cms / g_velocity_sample_count : 0;
        float average_acceleration_cmss = g_acceleration_sample_count > 0 ? g_acceleration_sum_cmss / g_acceleration_sample_count : 0;

        Serial.printf("Button was pressed for %lu ms\n", press_duration_ms);
        Serial.printf("Average distance: %.2f cm\n", average_distance_cm);
        Serial.printf("Average velocity: %.2f cm/s\n", average_velocity_cms);
        Serial.printf("Average acceleration: %.2f cm/s²\n", average_acceleration_cmss);
        Serial.println();
      } else {
        Serial.println("No valid distance samples recorded.");
      }
    }

    g_button_was_pressed = false;
  }
}

float ping_cm() {
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long start_us = micros();
  while (digitalRead(PIN_ECHO) == LOW) {
    if (micros() - start_us > MAX_ECHO_DURATION_US) return -1;
  }
  unsigned long echo_start_us = micros();

  while (digitalRead(PIN_ECHO) == HIGH) {
    if (micros() - echo_start_us > MAX_ECHO_DURATION_US) return -1;
  }
  unsigned long echo_end_us = micros();

  unsigned long echo_duration_us = echo_end_us - echo_start_us;
  if (echo_duration_us > MAX_ECHO_DURATION_US) return -1;

  return echo_duration_us / 58.0;
}
