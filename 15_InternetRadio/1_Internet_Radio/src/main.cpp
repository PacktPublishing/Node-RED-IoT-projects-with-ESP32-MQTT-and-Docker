
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
Internet Radio | V1.0 | 01.2023


Library download and copy to src: https://github.com/schreibfaul1/ESP32-audioI2S
*/
#include <Arduino.h>
#include "Audio.h"
#include "wifimqtt.h"

// Define I2S connections
#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

// Create audio object
Audio audio;

void setup()
{
  Serial.begin(115200);

  connectAP();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  // 0-100
  audio.setVolume(10);

  // radio station
  audio.connecttohost("http://onair.krone.at/kronehit.mp3");
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP32Radio");

  audio.loop();
}

// Audio status functions
void audio_info(const char *info)
{
  Serial.print("info        ");
  Serial.println(info);
}
void audio_id3data(const char *info)
{ // id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ // end of file
  Serial.print("eof_mp3     ");
  Serial.println(info);
}
void audio_showstation(const char *info)
{
  Serial.print("station     ");
  Serial.println(info);
}
void audio_showstreaminfo(const char *info)
{
  Serial.print("streaminfo  ");
  Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
  Serial.print("streamtitle ");
  Serial.println(info);
  client.publish("iradio/streamtitle", info);
}
void audio_bitrate(const char *info)
{
  Serial.print("bitrate     ");
  Serial.println(info);
}
void audio_commercial(const char *info)
{ // duration in sec
  Serial.print("commercial  ");
  Serial.println(info);
}
void audio_icyurl(const char *info)
{ // homepage
  Serial.print("icyurl      ");
  Serial.println(info);
}
void audio_lasthost(const char *info)
{ // stream URL played
  Serial.print("lasthost    ");
  Serial.println(info);
}
void audio_eof_speech(const char *info)
{
  Serial.print("eof_speech  ");
  Serial.println(info);
}