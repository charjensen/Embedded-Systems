/*
 * lab4part3.ino
 *
 * Pulses an LED with PWM and drives a motor with direction control.
 * Motor reverses when duty cycle hits boundaries; LED pulses up/down.
 *
 * Author: Charles Jensen
 * Created: 2025-04-17
 * Last Modified: 2025-04-17
 */

// Pin assignments
const int PIN_LED = D7;
const int PIN_PWMA = D8;
const int PIN_AIN2 = D9;
const int PIN_AIN1 = D10;

// Constants
const int MAX_DUTY_CYCLE = 255;
const int MIN_DUTY_CYCLE = 0;
const int PULSE_THRESHOLD_MS = 25; // Delay between LED pulse steps
const int MOTOR_THRESHOLD_MS = 20; // Delay between motor speed steps

// Global state (hardware-controlled values)
int g_current_duty_cycle_led = 0;     // Current LED brightness
int g_current_duty_cycle_motor = 255; // Current motor speed
int g_pulse_direction = 1;            // LED pulse direction (up/down)
int g_motor_direction = 0;            // 0 = forward, 1 = reverse
int g_motor_move_direction = 1;       // Speed step direction

void setup()
{
  Serial.begin(115200);

  // Set pin modes
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PWMA, INPUT);
  pinMode(PIN_AIN2, INPUT);
  pinMode(PIN_AIN1, INPUT);

  // Initialize output states
  analogWrite(PIN_LED, g_current_duty_cycle_led);
  analogWrite(PIN_AIN1, g_current_duty_cycle_motor);
  analogWrite(PIN_AIN2, g_current_duty_cycle_motor);
  analogWrite(PIN_PWMA, 255); // Always on PWM enable
}

void loop()
{
  static int s_last_pulse_update_ms = 0;
  static int s_last_motor_update_ms = 0;

  int current_time_ms = millis();

  // LED pulse direction handling
  if (g_current_duty_cycle_led <= MIN_DUTY_CYCLE)
  {
    g_pulse_direction = 1;
  }
  else if (g_current_duty_cycle_led >= MAX_DUTY_CYCLE)
  {
    g_pulse_direction = -1;
  }

  // Motor direction handling at duty cycle limits
  if (g_current_duty_cycle_motor <= MIN_DUTY_CYCLE)
  {
    g_motor_direction = !g_motor_direction;
    g_motor_move_direction = 1;
  }
  else if (g_current_duty_cycle_motor >= MAX_DUTY_CYCLE)
  {
    g_motor_move_direction = -1;
  }

  // Update LED PWM brightness
  if (current_time_ms - s_last_pulse_update_ms > PULSE_THRESHOLD_MS)
  {
    g_current_duty_cycle_led += g_pulse_direction;
    analogWrite(PIN_LED, g_current_duty_cycle_led);
    s_last_pulse_update_ms = current_time_ms;
  }

  // Update motor PWM speed and direction
  if (current_time_ms - s_last_motor_update_ms > MOTOR_THRESHOLD_MS)
  {
    g_current_duty_cycle_motor += g_motor_move_direction;

    if (g_motor_direction)
    {
      analogWrite(PIN_AIN2, g_current_duty_cycle_motor);
      analogWrite(PIN_AIN1, LOW);
    }
    else
    {
      analogWrite(PIN_AIN1, g_current_duty_cycle_motor);
      analogWrite(PIN_AIN2, LOW);
    }

    s_last_motor_update_ms = current_time_ms;
  }
}
