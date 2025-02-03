#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"

extern byte led;

// MQTT
String clientID = "ESP32-";
const char *mqtt_server = "5.75.238.225";
const char *mqtt_user = "pixeledi";
const char *mqtt_password = "noderedkurs#2023";
WiFiClient espClient;
PubSubClient client(espClient);

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        clientID += String(random(0xffff), HEX);
        if (client.connect(clientID.c_str(), mqtt_user, mqtt_password))
        {
            Serial.println("connect to MQTT");
            // client.subscribe("fromNodeRED");
            // client.subscribe("ledState");
            // client.subscribe("JSONfromNodeRED");
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
    // Serial.print("Message arrived on topic: ");
    // Serial.print(topic);
    // Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++)
    {
        // Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }

    if (String(topic) == "fromNodeRED")
    {
        Serial.println(messageTemp);
    }



   
}

void connectAP()
{
    Serial.println("Connect to my WiFi");
    WiFi.begin(ssid, password);
    byte cnt = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
        cnt++;

        if (cnt > 30)
        {
            ESP.restart();
        }
    }
}