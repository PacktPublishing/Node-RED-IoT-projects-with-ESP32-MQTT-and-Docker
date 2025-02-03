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
WiFi & MQTT with TLS | V1.0 | 04.2023

*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifimqtt.h"

unsigned long previousMillis = millis();

void setup()
{
  Serial.begin(115200);

  connectAP();
  espClient.setCACert(letsencrypt_root_ca);
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP32Weather");

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= (1000 * 3))
  {
    previousMillis = currentMillis;
    client.publish("esp32tls","it works!");
  }
}