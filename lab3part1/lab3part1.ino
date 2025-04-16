/*
 * lab3part1.ino
 *
 * Controls an LED using serial commands: 'on' or 'off'.
 * Prints 'waiting...' every second while idle.
 *
 * Author: Charles Jensen
 * Created: 2025-04-16
 * Last Modified: 2025-04-16
 */

// Constants for pins
const int LED_PIN = D7;

// Timing constant
const int PRINT_INTERVAL_THRESHOLD_MS = 1000;

// Global Variables
static String g_cmd = ""; // Accumulates serial command characters

void setup()
{
    Serial.begin(115200);       // Begin serial communication
    pinMode(LED_PIN, OUTPUT);   // Set LED pin as output
    digitalWrite(LED_PIN, LOW); // Start with LED off
}

void loop()
{
    static int s_last_print_time_ms = 0; // Tracks last 'waiting...' print

    int current_time_ms = millis();

    // Print status once per second
    if (current_time_ms - s_last_print_time_ms > PRINT_INTERVAL_THRESHOLD_MS)
    {
        s_last_print_time_ms = current_time_ms;
        Serial.println(F("waiting..."));
    }

    // Process serial input
    if (Serial.available() > 0)
    {
        char c = Serial.read();

        if (c == '\n' || c == '\r')
        {
            g_cmd.toLowerCase();
            Serial.printf("command: '%s'\n", g_cmd.c_str());

            if (g_cmd == "on")
            {
                digitalWrite(LED_PIN, HIGH); // Turn on LED
            }
            else if (g_cmd == "off")
            {
                digitalWrite(LED_PIN, LOW); // Turn off LED
            }
            else
            {
                Serial.printf("unrecognized command: '%s'\n", g_cmd.c_str());
            }

            g_cmd = ""; // Reset command buffer
        }
        else
        {
            g_cmd.concat(c); // Continue building command
        }
    }
}
