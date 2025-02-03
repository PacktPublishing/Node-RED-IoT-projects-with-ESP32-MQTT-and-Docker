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
WiFi & MQTT | V1.0 | 01.2023

*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifimqtt.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP085.h>
#include <BH1750.h>

unsigned long previousMillis = millis();

BH1750 lightMeter;
Adafruit_BMP085 bmp;
float temp;
float pressure;
float altitude;
float lux;

byte fan = 18;

void sendSensorValuesMQTT()
{
  DynamicJsonDocument doc(1024);
  doc["temperature"] = temp;
  doc["pressure"] = pressure;
  doc["altitude"] = altitude;
  doc["lux"] = lux;

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish("weatherstation", buffer);
}

void printSensorValue()
{
  Serial.print("Tempterature: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" Meter");

  Serial.print("Illumination: ");
  Serial.print(lux);
  Serial.println(" lux");
}

void setup()
{
  Serial.begin(115200);
  pinMode(fan, OUTPUT);

  if (!bmp.begin())
  {
    Serial.println("BMP could not start!");
    while (1)
    {
    }
  }
  lightMeter.begin();

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
    client.connect("ESP32Weather");

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= (1000 * 3))
  {
    temp = bmp.readTemperature();
    pressure = bmp.readPressure() / 100.0F;
    altitude = bmp.readAltitude();
    lux = lightMeter.readLightLevel();

    printSensorValue();
    sendSensorValuesMQTT();

    previousMillis = currentMillis;
    // Serial.println("blink");
  }
}