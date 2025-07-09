/*
 * lab4part1.ino
 *
 * Blinks an LED using blocking delays with customizable high/low times.
 *
 * Author: Charles Jensen
 * Created: 2025-04-17
 * Last Modified: 2025-04-17
 */

// Pin assignment
const int PIN_LED = D7;

// Constants (in milliseconds)
const int BLINK_TIME_HIGH_MS = 1;
const int BLINK_TIME_LOW_MS = 9;

// Function prototype
void blink(int time_high_ms, int time_low_ms);

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
}

void loop()
{
  blink(BLINK_TIME_HIGH_MS, BLINK_TIME_LOW_MS);
}

// Turns LED on/off for specified high/low durations
void blink(int time_high_ms, int time_low_ms)
{
  digitalWrite(PIN_LED, HIGH);
  delay(time_high_ms);

  digitalWrite(PIN_LED, LOW);
  delay(time_low_ms);
}
