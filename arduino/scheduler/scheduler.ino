#include <WiFi.h>
#include "time.h"

#define LED_PIN 13 // for the onboard LED

const char* ssid = "Karl";
const char* password = "Rlb_KsESP";
const char* ntpServer = "0.pool.ntp.org";
const long  gmtOffset_sec = 3600; // Offset for your timezone in seconds
const int   daylightOffset_sec = 3600; // Offset for daylight saving time in seconds

int lastNtpUpdateMin = -1; // Initialised to an invalid value to force an update on the first loop iteration


enum BlinkState {
  BLINK_FAST,
  BLINK_SLOW
};
unsigned long lastBlinkTime = 0; // Keep track of the last time we blinked the LED
BlinkState blinkState = BLINK_SLOW;


void setup()
{

  // WARNING: do NOT use this on the inbuilt ESP
  pinMode(LED_PIN, OUTPUT); // Set the LED pin as output


  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  Serial.begin(115200);   // for output to serial monitor

  // Configure NTP and sends first request for syncing the time (actually runs asynchronously in the background, handled by FreeRTOS?)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


  // ...
}

void loop()
{
  time_t now;
  time(&now); // Call the time function, which checks the current time and stores it in the variable now (we pass the address to the function, so it can both access and modify the variable). Is C stuff, just go with it for now :)

  // Update the time from the NTP server every 5 minutes (for testing purposes only!)
  if (localtime(&now)->tm_min % 5 == 0 && lastNtpUpdateMin != localtime(&now)->tm_min) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    lastNtpUpdateMin = localtime(&now)->tm_min;
    Serial.println(ctime(&now)); // Print the current time to the serial monitor
  }

  // Change the blink state every 3 minutes to fast and every 2 minutes to slow
  if (localtime(&now)->tm_min % 5 < 3) {
    blinkState = BLINK_FAST;
  } else {
    blinkState = BLINK_SLOW;
  }

  switch (blinkState) {
    case BLINK_FAST:
      blinkFast();
      break;
    case BLINK_SLOW:
      blinkSlow();
      break;
  }
}


void blinkFast() {
  if (millis() - lastBlinkTime >= 250) {
    lastBlinkTime = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle the LED state
  }
}

void blinkSlow() {
  if (millis() - lastBlinkTime >= 500) {
    lastBlinkTime = millis();
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle the LED state
  }
}