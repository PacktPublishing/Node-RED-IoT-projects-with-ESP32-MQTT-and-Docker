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
E-INK| V1.0 | 04.2023

*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifimqtt.h"

unsigned long previousMillis = millis();

#include <GxEPD2_BW.h> // including both doesn't use more code or ram
#include <GxEPD2_3C.h> // including both doesn't use more code or ram

GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(/*CS=5*/ 5, /*DC=*/2, /*RST=*/3, /*BUSY=*/4)); // GDEM0213B74 128x250, SSD1680

const char HelloWorld[] = "Hello pixelEDI v2";

void helloWorld_minimum(){ 
   display.init();
  // comment out next line to have no or minimal Adafruit_GFX code
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    // comment out next line to have no or minimal Adafruit_GFX code
    //display.print("Hello World!");
    display.print(HelloWorld);
  }
  while (display.nextPage());
}

void setup()
{
  Serial.begin(115200);

  helloWorld_minimum();

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

    previousMillis = currentMillis;
    Serial.println("blink");
  }
}