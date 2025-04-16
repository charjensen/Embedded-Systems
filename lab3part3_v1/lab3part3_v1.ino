#include <ESP32Servo.h>

//Program states
enum State {
  RUN,
  STOPPING,
  STOPPED
};

//Prototypes
void start();
void stop();
void blink_led(int led_blink_threshold_ms);
void rotate_servo();

//Global Variables
int g_last_print_time_ms = 0;

String g_cmd = "";

Servo g_servo;
int g_servo_position = 0;
int g_servo_direction_deg = 1;
int g_servo_last_update_ms = 0;

int g_led_state = LOW;
int g_led_last_blink_ms = 0;

State g_state = STOPPED;

int g_button_state = 0;
bool g_button_was_pressed = false;

//Pins
const int LED_PIN = D7;
const int SERVO_PIN = D9;
const int BUTTON_PIN = D6;

//Constants
const int PRINT_INTERVAL_THRESHOLD_MS = 1000;
const int SERVO_START_POSITION_DEG = 0;
const int SERVO_UPDATE_THRESHOLD_MS = 15;

void setup() {

  //Setup Serial
  Serial.begin(115200);

  //Setup Pins
  pinMode(LED_PIN, OUTPUT);
  g_servo.attach(SERVO_PIN);

  //Initialize default pin states
  digitalWrite(LED_PIN, LOW);
  g_servo.write(SERVO_START_POSITION_DEG);

  pinMode(BUTTON_PIN, INPUT);

}

void loop() {

  //Check the current time
  int current_time_ms = millis();

  //See if the last print was more than a second ago
  if (current_time_ms - g_last_print_time_ms > PRINT_INTERVAL_THRESHOLD_MS) {

    //Print out waiting every second
    g_last_print_time_ms = millis();
    Serial.println("waiting...");

  }

  //If we have bytes to read
  if (Serial.available() > 0) {

    //Parse the incoming byte and print it
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {

      //Output command to serial
      Serial.printf("command: '%s'\n", g_cmd.c_str());

      //Check if we have a registered command
      g_cmd.toLowerCase();
      if (g_cmd.compareTo("start") == 0) {

        //Change state to running
        g_state = RUN;

      }
      else if (g_cmd.compareTo("stop") == 0) {

        //Change state to stopping
        g_state = STOPPING;

      }
      else {

        //Finally print out an error
        Serial.printf("unrecognized command: '%s'\n", g_cmd);

      }

      //Reset the string for next command
      g_cmd = "";

    }
    else 
      //Continue building the command string until a \n or \r is read
      g_cmd.concat(c);

  }

  //Check button state and change the global state accordingly
  bool is_button_pressed = digitalRead(BUTTON_PIN);

  if (is_button_pressed) {

    g_button_was_pressed = true;

  }
  else {

    if (g_button_was_pressed) {

      switch(g_state) {

        case STOPPED:
           g_state = RUN;
           break;

        case RUN:
          g_state = STOPPING;
          break;
        
        case STOPPING:
          g_state = RUN;
          break;

      }

    }

    g_button_was_pressed = false;

  }

  if (g_state == RUN) 
    start();
  else if (g_state == STOPPING) 
    stop();

}

void start() {

  g_state = RUN;
  int blink_time_ms = 250;
  blink_led(blink_time_ms);
  rotate_servo();

}

void stop() {

  int blink_time_ms = 50;
  blink_led(blink_time_ms);
  rotate_servo_to_zero();

}


void blink_led(int led_blink_threshold_ms) {

  int current_time_ms = millis();
  
  if (current_time_ms - g_led_last_blink_ms > led_blink_threshold_ms) {

    g_led_state = !g_led_state;
    digitalWrite(LED_PIN, g_led_state);
    g_led_last_blink_ms = millis();

  }

}

void rotate_servo() {

  if (g_servo_position > 180)
    g_servo_direction_deg = -1;

  else if (g_servo_position < 0)
    g_servo_direction_deg = 1;

  int current_time_ms = millis();
  if (current_time_ms - g_servo_last_update_ms > SERVO_UPDATE_THRESHOLD_MS) {

    g_servo_position += g_servo_direction_deg;
    g_servo.write(g_servo_position);
    g_servo_last_update_ms = millis();

  }

}

//TODO: just merge these into one function with the capability to move to any position
void rotate_servo_to_zero() {

  if (g_servo_position > 0)
    g_servo_direction_deg = -1;
  else
    g_state = STOPPED;

  int current_time_ms = millis();
  if (current_time_ms - g_servo_last_update_ms > SERVO_UPDATE_THRESHOLD_MS) {

    g_servo_position += g_servo_direction_deg;
    g_servo.write(g_servo_position);
    g_servo_last_update_ms = millis();

  }

}
