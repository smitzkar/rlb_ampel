#include <WiFi.h>
#include "time.h"

#define LED_PIN 13 // for the onboard LED

const char* ssid = "Karl";
const char* password = "Rlb_KsESP";
const char* ntpServer = "0.pool.ntp.org";
const long  gmtOffset_sec = 3600;       // Offset for your timezone in seconds
const int   daylightOffset_sec = 3600;  // Offset for daylight saving time in seconds

int lastNtpUpdateMin = -1; // Initialised to an invalid value to force an update on the first loop iteration
const int ntpUpdateInterval = 5; // Update the time from the NTP server every 5 minutes

bool stopBlinking = false; // used to interrupt the blinking loop

// Haven't used arrays in a while, so...
int blinkCount[] = {60, 20};        // Set the number of blinks for each mode (fast and slow)
int blinkDuration[] = {30, 60};     // Set the duration (in seconds) for each mode (fast and slow)
// at this pattern of 90s total, it'll run for 40 cycles per hour and restarts every hour

unsigned long lastBlinkTime = 0; // Keep track of the last time we blinked the LED


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
  // will be 01.01.1970 01:00:00 on first Serial.println(ctime(&now)); (not worth the effort to fix this, as it's just a minor inconvenience. Fixes itself on first update.)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


  // ...
}

void loop()
{
  time_t now;
  time(&now); // Call the time function, which checks the current time and stores it in the variable now (we pass the address to the function, so it can both access and modify the variable). Is C stuff, just go with it for now :)

  // Update the time from the NTP server every ntpUpdateInterval minutes
  if (localtime(&now)->tm_min % ntpUpdateInterval == 0 && lastNtpUpdateMin != localtime(&now)->tm_min) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    lastNtpUpdateMin = localtime(&now)->tm_min;
    Serial.println(ctime(&now)); // Print the current time to the serial monitor
  }

  // the modulo solution was Copilot. I love it!
  const int totalBlink = blinkDuration[0] + blinkDuration[1];
  if (localtime(&now)->tm_min % totalBlink < blinkDuration[0]) {
    blinkControl(blinkCount[0], blinkDuration[0]);
  } else {
    blinkControl(blinkCount[1], blinkDuration[1]); 
  }
}



void blinkControl(int numberOfBlinks, unsigned long duration) {
  unsigned long startTime = millis();
  duration *= 1000; // convert to milliseconds
  unsigned long blinkInterval = duration / numberOfBlinks; // calculate the interval between each blink

  for (int i = 0; i < numberOfBlinks; i++) {

    // Check if the command to stop the blinking has been received. If so, return.
    if (stopBlinking) {
      return;
    }

    digitalWrite(LED_PIN, HIGH); // Turn the LED on
    delay(blinkInterval / 2); // Wait for half the interval
    digitalWrite(LED_PIN, LOW); // Turn the LED off
    delay(blinkInterval / 2); // Wait for the other half of the interval
  }
}