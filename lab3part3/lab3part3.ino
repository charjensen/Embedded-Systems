/*
 * lab3part3.ino
 *
 * Controls a servo with start/stop commands via Serial or button.
 * Includes LED feedback: 2 Hz blink while running, 10 Hz while stopping.
 * Servo returns to 0 deg before stopping.
 *
 * Author: Charles Jensen
 * Created: 2025-04-16
 * Last Modified: 2025-04-16
 */

#include <ESP32Servo.h>

// Constants for hardware pins
const int LED_PIN = D7;
const int SERVO_PIN = D9;
const int BUTTON_PIN = D6;

// Timing and servo configuration constants
const int PRINT_INTERVAL_THRESHOLD_MS = 1000; // Print status every 1000ms
const int SERVO_START_POSITION_DEG = 0;       // Initial servo position
const int SERVO_UPDATE_THRESHOLD_MS = 15;     // Delay between servo updates

// Enum representing finite state machine states
enum ProgramState
{
    STATE_RUN,      // Servo oscillating, LED blinking at 2Hz
    STATE_STOPPING, // Waiting to return to 0 deg, LED blinking at 10Hz
    STATE_STOPPED   // Servo stationary at 0 deg, LED off
};

// Function prototypes
void handle_start();
void handle_stop();
void blink_led(int blink_interval_ms);
void rotate_servo();
void rotate_servo_to_position_zero();

// Global variables
Servo g_servo;                                // Servo object instance
int g_servo_position_deg = 0;                 // Current servo angle
int g_servo_direction_deg = 1;                // Current servo sweep direction
int g_led_state = LOW;                        // LED ON/OFF state
ProgramState g_program_state = STATE_STOPPED; // Initial FSM state
String g_cmd = "";                            // Serial command buffer

void setup()
{
    Serial.begin(115200); // Begin serial communication

    // Configure hardware pin modes
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    // Initialize servo and LED
    g_servo.attach(SERVO_PIN);
    g_servo.write(SERVO_START_POSITION_DEG);
    digitalWrite(LED_PIN, LOW);
}

void loop()
{
    // Static variables that persist across loop calls
    static int s_last_print_time_ms = 0;      // Tracks time of last print
    static bool s_button_was_pressed = false; // Button debounce tracking

    int current_time_ms = millis();

    // Periodically print 'waiting...'
    if (current_time_ms - s_last_print_time_ms > PRINT_INTERVAL_THRESHOLD_MS)
    {
        s_last_print_time_ms = current_time_ms;
        Serial.println(F("waiting..."));
    }

    // Handle serial input and build complete commands
    if (Serial.available() > 0)
    {
        char c = Serial.read();
        if (c == '\n' || c == '\r')
        {

            if (g_cmd.isEmpty())
                return;

            g_cmd.toLowerCase();
            Serial.printf("command: '%s'\n", g_cmd.c_str());

            if (g_cmd == "start")
            {
                g_program_state = STATE_RUN;
            }
            else if (g_cmd == "stop")
            {
                g_program_state = STATE_STOPPING;
            }
            else
            {
                Serial.printf("unrecognized command: '%s'\n", g_cmd.c_str());
            }
            g_cmd = "";
        }
        else
        {
            g_cmd.concat(c);
        }
    }

    // Handle button press and toggle state accordingly
    bool is_button_pressed = digitalRead(BUTTON_PIN);
    if (is_button_pressed)
    {
        s_button_was_pressed = true;
    }
    else
    {
        if (s_button_was_pressed)
        {
            switch (g_program_state)    
            {
            case STATE_STOPPED:
                g_program_state = STATE_RUN;
                break;
            case STATE_RUN:
                g_program_state = STATE_STOPPING;
                break;
            case STATE_STOPPING:
                g_program_state = STATE_RUN;
                break;
            }
        }
        s_button_was_pressed = false;
    }

    // Execute logic based on current state
    switch (g_program_state)
    {
    case STATE_RUN:
        handle_start();
        break;
    case STATE_STOPPING:
        handle_stop();
        break;
    case STATE_STOPPED:
    default:
        break;
    }
}

// Behavior while in STATE_RUN: Blink LED slowly and move servo
void handle_start()
{
    blink_led(250); // 2 Hz blink interval
    rotate_servo();
}

// Behavior while in STATE_STOPPING: Blink LED quickly and return servo to 0 deg
void handle_stop()
{
    blink_led(50); // 10 Hz blink interval
    rotate_servo_to_position_zero();
}

// Toggle LED state based on timing interval
void blink_led(int blink_interval_ms)
{
    static int s_led_last_blink_time_ms = 0;
    int current_time_ms = millis();
    if (current_time_ms - s_led_last_blink_time_ms > blink_interval_ms)
    {
        g_led_state = !g_led_state;
        digitalWrite(LED_PIN, g_led_state);
        s_led_last_blink_time_ms = current_time_ms;
    }
}

// Move servo back and forth between 0 and 180 degrees
void rotate_servo()
{
    static int s_servo_last_update_time_ms = 0;

    // Change direction if limits are exceeded
    if (g_servo_position_deg >= 180)
        g_servo_direction_deg = -1;
    else if (g_servo_position_deg <= 0)
        g_servo_direction_deg = 1;

    int current_time_ms = millis();
    if (current_time_ms - s_servo_last_update_time_ms > SERVO_UPDATE_THRESHOLD_MS)
    {
        g_servo_position_deg += g_servo_direction_deg;
        g_servo.write(g_servo_position_deg);
        s_servo_last_update_time_ms = current_time_ms;
    }
}

// Move servo gradually to 0 deg; switch to STATE_STOPPED when done
void rotate_servo_to_position_zero()
{
    static int s_servo_last_update_time_ms = 0;

    if (g_servo_position_deg > 0)
    {
        g_servo_direction_deg = -1;
    }
    else
    {
        g_program_state = STATE_STOPPED;
        digitalWrite(LED_PIN, LOW);
        return;
    }

    int current_time_ms = millis();
    if (current_time_ms - s_servo_last_update_time_ms > SERVO_UPDATE_THRESHOLD_MS)
    {
        g_servo_position_deg += g_servo_direction_deg;
        g_servo.write(g_servo_position_deg);
        s_servo_last_update_time_ms = current_time_ms;
    }
}
