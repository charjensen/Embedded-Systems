/*
 * lab4part2.ino
 *
 * Gradually increases and decreases the LED brightness using PWM.
 *
 * Author: Charles Jensen
 * Created: 2025-04-17
 * Last Modified: 2025-04-17
 */

// Pin assignments
const int PIN_LED = D7;

// Constants
const int MAX_DUTY_CYCLE = 255;
const int MIN_DUTY_CYCLE = 0;
const int PULSE_THRESHOLD_MS = 25; // Time between brightness steps

// Global state
int g_current_duty_cycle = 127; // Initial PWM brightness
int g_pulse_direction = 1;      // Direction of brightness change

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  analogWrite(PIN_LED, g_current_duty_cycle);
}

void loop()
{
  static int s_last_pulse_update_ms = 0; // Time of last PWM update
  int current_time_ms = millis();

  // Reverse direction at bounds
  if (g_current_duty_cycle <= MIN_DUTY_CYCLE)
  {
    g_pulse_direction = 1;
  }
  else if (g_current_duty_cycle >= MAX_DUTY_CYCLE)
  {
    g_pulse_direction = -1;
  }

  // Update PWM when threshold passed
  if (current_time_ms - s_last_pulse_update_ms > PULSE_THRESHOLD_MS)
  {
    g_current_duty_cycle += g_pulse_direction;
    analogWrite(PIN_LED, g_current_duty_cycle);
    s_last_pulse_update_ms = current_time_ms;
  }
}
