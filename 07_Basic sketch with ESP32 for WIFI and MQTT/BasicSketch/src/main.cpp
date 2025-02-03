/*
        _          _ ______ _____ _____
       (_)        | |  ____|  __ \_   _|
  _ __  ___  _____| | |__  | |  | || |
 | '_ \| \ \/ / _ \ |  __| | |  | || |
 | |_) | |>  <  __/ | |____| |__| || |_
 | .__/|_/_/\_\___|_|______|_____/_____|
 | |
 |_|

www.pixeledi.eu | https://linktr.ee/pixeledi
WiFi & MQTT | V1.0 | 04.2023

*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifimqtt.h"

byte led = 26;
unsigned long previousMillis = millis();

void sendMQTTvalues()
{
  StaticJsonDocument<256> doc;

  doc["device"] = "ESP32 is the best MCU";
  doc["temperature"] = random(-20, 40);
  doc["humidity"] = random(10, 99);;
  doc["lux"] = random(20, 850);

  char buff[256];
  serializeJson(doc, buff);
  client.publish("ESPValues",buff);
}

void setup()
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  connectAP();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
  {
    client.connect("ESP32-");
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 5000)
  {
    previousMillis = currentMillis;
    // digitalWrite(led, !digitalRead(led));
    // client.publish("toNodeRED","hello from ESP32");
    sendMQTTvalues();
  }
}