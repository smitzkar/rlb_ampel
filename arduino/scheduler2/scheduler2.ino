#include <WiFi.h>
#include "time.h"

#define LED_PIN 13 // for the onboard LED

const char* ssid = "Karl";
const char* password = "Rlb_KsESP";
const char* ntpServer = "0.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

int lastNtpUpdateMin = -1; // Initialised to an invalid value to force an update on the first loop iteration
const int ntpUpdateInterval = 5; // Update the time from the NTP server every 5 minutes

bool stopBlinking = false; // used to interrupt the blinking loop

// Haven't used arrays in a while, so...
int blinkCount[] = {60, 20};        // Set the number of blinks for each mode (fast and slow)
int blinkDuration[] = {20, 40};     // Set the duration (in seconds) for each mode (fast and slow)
// rethought length, so that a full cylce is now 1min 


unsigned long lastBlinkTime = 0;
int blinks = 0;
int totalBlinks = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.begin(115200);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void syncTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  int currentMin = timeinfo.tm_min;
  if (currentMin != lastNtpUpdateMin) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    lastNtpUpdateMin = currentMin;
  }
}

void blinkLED() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  int currentSec = timeinfo.tm_sec;
  if (currentSec == 0) {
    blinks = 0;
    totalBlinks = 0;
  }
  if (blinks < blinkCount[totalBlinks]) {
    digitalWrite(LED_PIN, HIGH);
    delay(blinkDuration[totalBlinks]);
    digitalWrite(LED_PIN, LOW);
    delay(blinkDuration[totalBlinks]);
    blinks++;
  } else {
    totalBlinks = (totalBlinks + 1) % 2;
    if (currentSec != 0) {
      Serial.println("Blinking is out of sync. Waiting for the next minute to restart.");
      while (currentSec != 0) {
        delay(1000);
        if(!getLocalTime(&timeinfo)){
          Serial.println("Failed to obtain time");
          return;
        }
        currentSec = timeinfo.tm_sec;
      }
    }
  }
}

void loop() {
  syncTime();
  blinkLED();
}