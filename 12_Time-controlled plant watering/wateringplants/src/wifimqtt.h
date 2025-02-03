
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "credentials.h"
#include "globals.h"

// MQTT
String clientId = "ESP32-";
const char *mqtt_server = "5.75.238.225";
const char *mqtt_user = "pixeledi";
const char *mqtt_password = "noderedkurs#2023";
;
WiFiClient espClient;
PubSubClient client(espClient);

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
      client.subscribe("pumpStart");
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

  if (String(topic) == "pumpStart")
  {
    if (messageTemp == "1")
    {
      Serial.println("pumping...");
      previousMillis=millis();
      state = PUMPING;
    }
    else if (messageTemp == "0")
    {
      Serial.println("sleeping...");
      state = TIMEFORSLEEP;
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
