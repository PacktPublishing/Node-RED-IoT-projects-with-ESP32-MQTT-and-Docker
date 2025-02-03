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
#include "globals.h"
#include "wifimqtt.h"
#include "ultrasonic.h"

void sendNewPumpInfos()
{
  StaticJsonDocument<48> doc;

  doc["distance1"] = distance1;
  doc["distance2"] = "22";

  char buffer_1[256];
  serializeJson(doc, buffer_1);

  client.publish("WaterDistance", buffer_1);
  Serial.println("distance transmitted");
}

void startPumping()
{

  unsigned long currentMillisPumpen = millis();
  digitalWrite(relaisPumpGluecksfeder, HIGH);

  if (currentMillisPumpen - previousMillis >= (1000 * 3))
  {
    digitalWrite(relaisPumpGluecksfeder, LOW);
    Serial.println("pump is off");

    getDistance();
    Serial.println(distance1);
    sendNewPumpInfos();

    state = TIMEFORSLEEP;
  }
}

void getData()
{
  client.publish("pumpInfo", "give me Information");
  Serial.println("mqtt publish pumpInfo - now wait");
  state = WAIT;
}

void setup()
{
  Serial.begin(115200);

  state = GETDATA;
  pinMode(relaisPumpGluecksfeder, OUTPUT);
  pinMode(trigger1, OUTPUT);
  pinMode(echo1, INPUT);

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

  switch (state)
  {
  case GETDATA:
    getData();
    break;
  case PUMPING:
    startPumping();
    break;
  case WAIT:
    // nothing to do just waiting
    break;
  case TIMEFORSLEEP:
    // get to sleep
    Serial.println("I am going to sleep ZzzZZzZZZZzZZ");
    client.publish("recorddeepsleep", "zzZZzzZZ");
    delay(1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    //ESP.deepSleep(59*60e6);
    delay(1000*100);
    state = GETDATA;
    break;
  }

  // unsigned long currentMillis = millis();

  // if (currentMillis - previousMillis >= (1000 * 3))
  // {

  //   previousMillis = currentMillis;
  //   Serial.println("blink");
  // }
}