#include <ESP32Servo.h>

Servo g_myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int g_pos = 0;    // variable to store the servo position

const int SERVO_PIN = D9;

void setup() {
  g_myservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  for (g_pos = 0; g_pos <= 180; g_pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    g_myservo.write(g_pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (g_pos = 180; g_pos >= 0; g_pos -= 1) { // goes from 180 degrees to 0 degrees
    g_myservo.write(g_pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}