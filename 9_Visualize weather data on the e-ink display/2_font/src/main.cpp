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
#include <Fonts/FreeMonoBold9pt7b.h>

GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(/*CS=5*/ 5, /*DC=*/2, /*RST=*/3, /*BUSY=*/4)); // GDEM0213B74 128x250, SSD1680

void helloWorld_minimum(String einkText)
{
  display.init();
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.getTextBounds(einkText, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x,y);
    // display.print("Hello World!");
    display.print(einkText);
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