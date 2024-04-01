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
int blinkDuration[] = {20, 40};     // Set the duration (in seconds) for each mode (fast and slow)
// rethought length, so that a full cylce is now 1min 

unsigned long lastBlinkTime = 0; // Keep track of the last time we blinked the LED

int blinks = 0; // Keep track of the number of blinks
int totalBlinks = 0; // Keep track of the total number of blinks

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

  // Create the high-priority task
  xTaskCreatePinnedToCore(
    checkTimeTask,   /* Function to implement the task */
    "checkTime",     /* Name of the task */
    10000,           /* Stack size in words */
    NULL,            /* Task input parameter */
    1,               /* Priority of the task */
    NULL,            /* Task handle. */
    0);              /* Core where the task should run */

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
    Serial.println("Blinks during last 5min: ");
    Serial.println(blinks); // Print the number of blinks during the last 5 minutes
    totalBlinks += blinks; // Add the number of blinks to the total
    blinks = 0; // Reset the number of blinks
    Serial.println("Total blinks: ");
    Serial.println(totalBlinks); // Print the total number of blinks

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

    // Currently, it'll keep going until the number of blinks has been reached.
    // Problem: it's supposed to stop after the duration has passed. Or more accurately, it should stop when it's time to stop! 
    // Solutions: 
    // b) check if there's enough time for another round of blinking. If not, stop the loop.
    // would then need to have a "waiting" state, where it waits for the next interval to start blinking again, so it doesn't speed up, but stays in sync. 

    // Check if the command to stop the blinking has been received. If so, return.
    if (stopBlinking) {
      return;
    }

    digitalWrite(LED_PIN, HIGH); // Turn the LED on
    delay(blinkInterval / 2); // Wait for half the interval
    digitalWrite(LED_PIN, LOW); // Turn the LED off
    delay(blinkInterval / 2); // Wait for the other half of the interval
    blinks += 1;
  }
}



// A high-priority task that sleeps most of the time, but comes online every slightly less than 5min to check the time and control the stopBlinking variable. 
void checkTimeTask(void * parameter) {
  for(;;) { // Infinite loop
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    // Calculate the delay until the next xx:x4:59 or xx:x9:59
    int delay_sec = 60 - timeinfo.tm_sec + (59 - (timeinfo.tm_min % 5)) * 60;
    if (timeinfo.tm_min % 5 >= 4) {
      delay_sec += 5 * 60; // Add 5 minutes if we're past the 4 minute mark
    }

    // Check if the current time is xx:x4:59 or xx:x9:59
    if (delay_sec == 0) {
      stopBlinking = true;
      delay_sec = 5 * 60; // Wait for 5 minutes until the next xx:x4:59 or xx:x9:59
    } else {
      stopBlinking = false;
    }

    vTaskDelay(delay_sec * 1000 / portTICK_PERIOD_MS); // Delay for the calculated amount of time
  }
}