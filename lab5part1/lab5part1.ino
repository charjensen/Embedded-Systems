/*
 * lab5part1.ino
 *
 * Listens for JSON messages over Serial, expecting: {"mtype":"ping", "value":<int>}.
 * Responds with a 'pong' message that echoes the value incremented by 1.
 *
 * Author: Charles Jensen
 * Created: 2025-04-19
 * Last Modified: 2025-04-19
 */

#include <ArduinoJson.h>

// Global variables
int g_global_count = 0; // Counter used in loop_serialize()

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  static String s_json_buffer = ""; // Accumulates incoming characters

  if (Serial.available() > 0)
  {
    char c = Serial.read();

    if (c == '\n' || c == '\r')
    {
      if (s_json_buffer.isEmpty())
        return;

      JsonDocument doc_input;
      DeserializationError error = deserializeJson(doc_input, s_json_buffer);

      if (error)
      {
        Serial.print(F("Error: Failed to parse JSON: "));
        Serial.println(error.f_str());
        s_json_buffer = "";
        return;
      }

      // Validate "mtype" field
      if (!doc_input.containsKey("mtype"))
      {
        Serial.println(F("Error: JSON does not contain 'mtype' key."));
        s_json_buffer = "";
        return;
      }

      const char *mtype = doc_input["mtype"];
      if (String(mtype) != "ping")
      {
        Serial.println(F("Error: mtype key value is not correct"));
        s_json_buffer = "";
        return;
      }

      // Validate "value" field
      if (!doc_input.containsKey("value"))
      {
        Serial.println(F("Error: 'ping' message missing 'value' key."));
        s_json_buffer = "";
        return;
      }

      JsonVariant val = doc_input["value"];
      if (!val.is<int>())
      {
        Serial.println(F("Error: 'value' must be an integer."));
        s_json_buffer = "";
        return;
      }

      int value = val.as<int>();

      // Create pong response
      JsonDocument doc_output;
      doc_output["mtype"] = "pong";
      doc_output["value"] = value + 1;
      doc_output["millis"] = millis();

      char output[256];
      serializeJson(doc_output, output);
      Serial.println(output);

      s_json_buffer = ""; // Clear input buffer
    }
    else
    {
      s_json_buffer.concat(c); // Append character to JSON buffer
    }
  }
}

// Alternate loop for periodic pong message generation (not used in main loop)
void loop_serialize()
{
  JsonDocument doc;
  doc["mtype"] = "pong";
  doc["value"] = ++g_global_count;
  doc["millis"] = millis();

  char output[256];
  serializeJson(doc, output);
  Serial.println(output);

  delay(500);
}
