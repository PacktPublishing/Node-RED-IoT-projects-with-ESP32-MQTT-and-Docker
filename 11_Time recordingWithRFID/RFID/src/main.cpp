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
RFID with TimeTAGGER | V1.0 | 04.2023

*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include "wifimqtt.h"

// RFID
const int SS_PIN = 26;
const int RST_PIN = 5;
MFRC522 rfid(SS_PIN, RST_PIN);

const byte ledgreen = 21;
const byte ledred = 22;
bool checkIn = true;

unsigned long previousMillis = millis();

void toggleLED()
{
  if (checkIn)
  {
    checkIn = false;
    digitalWrite(ledred, HIGH);
    delay(1000);
    digitalWrite(ledred, LOW);
  }
  else if (!checkIn)
  {
    checkIn = true;
    digitalWrite(ledgreen, HIGH);
    delay(1000);
    digitalWrite(ledgreen, LOW);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledgreen, OUTPUT);
  pinMode(ledred, OUTPUT);

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID Modul is ready");

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

  if (!rfid.PICC_IsNewCardPresent())
  {
    return;
  }
  if (!rfid.PICC_ReadCardSerial())
  {
    return;
  }

  String content = "";
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    // Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    // Serial.print(rfid.uid.uidByte[i], HEX);
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  if (content.substring(1) == "AC 32 E6 8C")
  {
    Serial.println("ESP32");
    client.publish("time/esp", "#esp32");
    toggleLED();
  }
  else if (content.substring(1) == "4C 01 E8 8C")
  {
    Serial.println("blank");
    client.publish("time/blank", "#blank");
    toggleLED();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 5000)
  {
    previousMillis = currentMillis;
    // digitalWrite(led, !digitalRead(led));
    // client.publish("toNodeRED","hello from ESP32");
  }
}