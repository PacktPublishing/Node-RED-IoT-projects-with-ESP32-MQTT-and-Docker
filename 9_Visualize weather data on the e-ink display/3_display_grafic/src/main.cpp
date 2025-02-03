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
#include "einkimage.h"

unsigned long previousMillis = millis();

#include <GxEPD2_BW.h> // including both doesn't use more code or ram
#include <GxEPD2_3C.h> // including both doesn't use more code or ram
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/TomThumb.h>

GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(/*CS=5*/ 5, /*DC=*/2, /*RST=*/3, /*BUSY=*/4)); // GDEM0213B74 128x250, SSD1680

void helloWorld_minimum(String einkText)
{
  display.init();
  display.setRotation(1);
  display.setFullWindow();

  do
  {

    display.fillScreen(GxEPD_WHITE);
    display.drawInvertedBitmap(0, 0, eink_image, 250, 128, GxEPD_BLACK);

    display.setFont(&FreeSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(60,35);
    display.print("25");

    display.setCursor(60,95);
    display.print("cloudy");

    display.setFont(&FreeSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(170, 30);
    display.print("Thursday");
    display.setFont(&TomThumb);
    display.setCursor(170,40);
    display.print("05.04.2023");

  } while (display.nextPage());
}

void setup()
{
  Serial.begin(115200);

  helloWorld_minimum("Hello pixelEDI");

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