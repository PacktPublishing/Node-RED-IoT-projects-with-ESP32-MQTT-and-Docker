
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "credentials.h"

// MQTT
String clientId = "ESP32-";
const char *mqtt_server = "5.75.238.225";
const char *mqtt_user = "pixeledi";
const char *mqtt_password = "noderedkurs#2023";

WiFiClient espClient;
PubSubClient client(espClient);

extern void showImage();

struct einkData_template
{
  String temperature;
  String weather;
  String eday;
  String edate;
  String windspeed;
  String humidity;
  String sunset;
  String sunrise;
  String updated;
} einkData;

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password))
    {
      Serial.println("connected");
      // for this example not necessary
      client.subscribe("eink");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "eink")
  {
    // Stream& input;

    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, messageTemp);

    if (error)
    {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    float temperature = doc["temperature"]; // 7.53
    int humidity = doc["humidity"];         // 60
    const char *weather = doc["weather"];   // "few clouds"
    const char *sunrise = doc["sunrise"];   // "6:26"
    const char *sunset = doc["sunset"];     // "18:19"
    const char *update = doc["update"];     // "10:34"
    bool show = doc["show"];                // true
    const char *eday = doc["day"];           // "Thursday"
    const char *edate = doc["date"];         // "16.3.23"

    einkData.temperature = String(temperature);
    einkData.humidity = String(humidity);
    einkData.weather = String(weather);
    einkData.edate = String(edate);
    einkData.eday = String(eday);
    einkData.updated = String(update);
    einkData.sunset = String(sunset);
    einkData.sunrise = String(sunrise);

    if(show){
      showImage();
    }
  }
}

void connectAP()
{

  Serial.println("Connecting to WiFi..");
  WiFi.begin(ssid, password);

  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    cnt++;

    if (cnt > 30)
      ESP.restart();
  }
  Serial.println(WiFi.localIP());
}
