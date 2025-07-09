#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
const int BAUD_RATE = 115200;
const String SSID = "lol!!!";
const String PASSWORD = "trentonis";
const int WIFI_WAIT_MS = 100;
WebServer g_server(80);
void setup()
{
    // Set up Serial I/O
    Serial.begin(BAUD_RATE);
    while (!Serial)
    {
        continue;
    }
    Serial.println("Serial connected");
    // Connect to the WiFi
    WiFi.begin(SSID, PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(WIFI_WAIT_MS);
        Serial.print(".");
    }
    Serial.println("connected");
    // print out the IP Address
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    // configure the web server routes
    g_server.on("/ping", HTTP_handlePing);
    g_server.onNotFound(HTTP_notFound);
    g_server.begin();
    g_server.begin();
    Serial.println("HTTP server started");
}
void loop()
{
    g_server.handleClient();
}
void HTTP_notFound(void)
{
    Serial.printf("%d\t404\tNot Found\t%s\n", millis(), g_server.uri());
    g_server.send(404, "application/json", "404 Not Found");
}
void HTTP_handlePing(void)
{
    Serial.printf("%d\t200\t%s\n", millis(), g_server.uri());
    String value = "unknown";
    if (g_server.hasArg("value"))
    {
        value = g_server.arg("value");
    }
    Serial.printf("DEBUG: the value is: '%s'\n", value.c_str());
    
    JsonDocument doc;

    doc["mtype"] = "pong";
    doc["value"] = atoi(value.c_str()) + 1;
    doc["millis"] = millis();

    char output[256];
    serializeJson(doc, output);
    // Serial.println(output);

    int length = sizeof(output) / sizeof(output[0]);

    g_server.setContentLength(length);
    g_server.send(200, "application/json", output);
    // this seems to help
    delay(1);
}