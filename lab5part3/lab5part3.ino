/*
 * lab5part3.ino
 *
 * Starts a Wi-Fi access point, listens on HTTP port 80 for /ping requests.
 * Responds with a JSON-encoded 'pong' message including incremented value and millis().
 *
 * Author: Charles Jensen
 * Created: 2025-04-19
 * Last Modified: 2025-04-19
 */

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>

// Constants
const int BAUD_RATE = 115200;
const String WIFI_SSID = "lol!!!";
const String WIFI_PASSWORD = "trentonis";
const int WIFI_WAIT_MS = 100;

// Web server instance
WebServer g_server(80);

void setup()
{
  // Set up serial I/O
  Serial.begin(BAUD_RATE);
  while (!Serial)
  {
    continue;
  }
  Serial.println("Serial connected");

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(WIFI_WAIT_MS);
    Serial.print(".");
  }
  Serial.println("connected");

  // Print IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure HTTP routes
  g_server.on("/ping", HTTP_handlePing);
  g_server.onNotFound(HTTP_notFound);
  g_server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  g_server.handleClient(); // Handle incoming HTTP requests
}

// Handle unknown routes (404)
void HTTP_notFound()
{
  Serial.printf("%lu\t404\tNot Found\t%s\n", millis(), g_server.uri().c_str());
  g_server.send(404, "text/html", "404 Not Found");
}

// Handle valid /ping requests
void HTTP_handlePing()
{
  Serial.printf("%lu\t200\t%s\n", millis(), g_server.uri().c_str());

  // Get query parameter 'value'
  String value_str = g_server.hasArg("value") ? g_server.arg("value") : "unknown";
  Serial.printf("DEBUG: the value is: '%s'\n", value_str.c_str());

  // Build JSON response
  JsonDocument doc;
  doc["mtype"] = "pong";
  doc["value"] = atoi(value_str.c_str()) + 1;
  doc["millis"] = millis();

  char output[256];
  serializeJson(doc, output);

  // Send JSON response
  g_server.setContentLength(strlen(output));
  g_server.send(200, "application/json", output);

  delay(1); // Optional delay for transmission stability
}
