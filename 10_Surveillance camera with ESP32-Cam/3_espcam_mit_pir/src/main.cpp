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
ESP32 CAM to Node-RED | V1.0 | 12.2022

adapted from:
* https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/Camera/CameraWebServer
* https://github.com/luisomoreau/ESP32-cam-MQTT

How to flash
* Board: AI Thinker ESP32-CAM
* GPIO 0 and GND connection
* Rest button for flashing mode
* after flashing open gpio 0 and GND connection
* restart and let`s start

*/

#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"               // SD Card ESP32
#include "SD_MMC.h"           // SD Card ESP32
#include "soc/soc.h"          // Disable brownour problems
#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "driver/rtc_io.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "credentials.h"

#include <EEPROM.h> // read and write from flash memory
// define the number of bytes you want to access
#define EEPROM_SIZE 1

RTC_DATA_ATTR int bootCount = 0;
const char *mqttServer = "5.75.238.225";
const char *HostName = "pixelEDI Node-RED";
const char *mqttUser = "pixeledi";
const char *mqttPassword = "noderedkurs#2023";

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

const int MAX_PAYLOAD = 60000;
WiFiClient espClient;
PubSubClient client(espClient);
int pictureNumber = 0;

void sendMQTT(const uint8_t *buf, uint32_t len)
{
  Serial.println("Sending picture...");
  if (len > MAX_PAYLOAD)
  {
    Serial.print("Picture too large, increase the MAX_PAYLOAD value");
  }
  else
  {
    Serial.print("Picture sent ? : ");
    Serial.println(client.publish("sendPicture", buf, len, false));
  }
}

void connectWIFIandMQTT()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  // Set MQTT Connection
  client.setServer(mqttServer, 1883);
  client.setBufferSize(MAX_PAYLOAD); // This is the maximum payload length
  // client.setCallback(callback);
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(HostName, mqttUser, mqttPassword))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void take_picture()
{
  camera_fb_t *fb = NULL;

  Serial.println("Taking picture");

  fb = esp_camera_fb_get(); // used to get a single picture.

  if (!fb)
  {
    Serial.println("Camera capture failed");
    return;
  }
  Serial.println("Picture taken");

  esp_camera_fb_return(fb);
  delay(100);
  fb = esp_camera_fb_get();
  esp_camera_fb_return(fb);
  delay(100);
  fb = esp_camera_fb_get();
  delay(500);

  connectWIFIandMQTT();

  sendMQTT(fb->buf, fb->len);
  delay(300);
  esp_camera_fb_return(fb); // must be used to free the memory allocated by esp_camera_fb_get().
}

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
  Serial.begin(115200);

  Serial.setDebugOutput(true);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  pinMode(4, INPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_dis(GPIO_NUM_4);

  config.frame_size = FRAMESIZE_VGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.println("Starting SD Card");

  delay(500);
  if (!SD_MMC.begin())
  {
    Serial.println("SD Card Mount Failed");
    // return;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE)
  {
    Serial.println("No SD Card attached");
    return;
  }

  camera_fb_t *fb = NULL;

  // Take Picture with Camera
  fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return;
  }
  take_picture();
  esp_camera_fb_return(fb);

  delay(1000);

  // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0);

  Serial.println("Going to sleep now");
  delay(1000);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop()
{
}