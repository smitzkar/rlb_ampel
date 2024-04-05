#include <WiFi.h>
#include "time.h"
#include <cstdlib> // for rng
#include <ctime> // for rng
//#include "timerKs.h"

#define LED_PIN 13 // for the onboard LED

const char* ssid = "Karl";
const char* password = "Rlb_KsESP";
const char* ntpServer = "0.pool.ntp.org";
const long  gmtOffset_sec = 3600;       // Offset for your timezone in seconds
const int   daylightOffset_sec = 3600;  // Offset for daylight saving time in seconds

int lastNtpUpdate = -1; // Initialised to an invalid value to force an update on the first loop iteration
const int ntpUpdateInterval = 5; // Update the time from the NTP server every 5 minutes

bool stopBlinking = false; // used to interrupt the blinking loop

// Not using array, because C++ can't pass it to functions...
// Could use an object, but that's overkill for this simple use case
int fastBlinkCount = 40;
int slowBlinkCount = 20;
int fastBlinkDuration = 20; // in seconds
int slowBlinkDuration = 40;

// rethought length, so that a full cylce is now 1min 

unsigned long lastBlinkTime = 0; // Keep track of the last time we blinked the LED

int blinks = 0; // Keep track of the number of blinks
int totalBlinks = 0; // Keep track of the total number of blinks

int tolerance = 3; // Tolerance for syncing to the start of the minute in seconds

// For trial to start at specific time 
bool startAtSpecificTime = false;
int startHour = 0;
int startMinute = 0;


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
  // Wait until time is updated (not the epoch time)
  while (time(nullptr) <= 60 * 60 * 24) { // 60*60*24 is the number of seconds in a day
    delay(1000); // wait for 1 second before trying again
  }
}

void loop()
{
  time_t now;
  time(&now); // Call the time function, which checks the current time and stores it in the variable now (we pass the address to the function, so it can both access and modify the variable). Is C stuff, just go with it for now :)

  // Adds a 30s buffer to the start time, so that the first cycle starts at the specified time. 
  // (as long as the sync to full minute function is also used!)
  if (startAtSpecificTime) {
    if (startMinute == 0) {
      startMinute = 59;
      // still getting used to ternary operator. 
      // startHour is set to:(if startHour is 0, set it to 23, otherwise set it to startHour - 1)
      startHour = startHour == 0 ? 23 : startHour - 1; 
      delayUntil(startHour, startMinute, 30); 
    } else {
      startMinute -= 1;
      delayUntil(startHour, startMinute, 30); 
    }
  }

  // Update the time from the NTP server every ntpUpdateInterval minutes
  if (localtime(&now)->tm_min % ntpUpdateInterval == 0 && lastNtpUpdate != localtime(&now)->tm_min) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    lastNtpUpdate = localtime(&now)->tm_min;
    Serial.println(ctime(&now)); // Print the current time to the serial monitor
    Serial.println("Blinks during last 5min: ");
    Serial.println(blinks); // Print the number of blinks during the last 5 minutes
    totalBlinks += blinks; // Add the number of blinks to the total
    blinks = 0; // Reset the number of blinks
    Serial.println("Total blinks: ");
    Serial.println(totalBlinks); // Print the total number of blinks

    syncToMinute(tolerance); // Delay until just before the start of the next full minute
  }

  Serial.println("Starting new cycle of blinking at ");
  Serial.println(ctime(&now));
  

  blinkCycle(fastBlinkCount, slowBlinkCount, fastBlinkDuration, slowBlinkDuration);
  
}


void blinkCycle(int fastBlinkCount, int slowBlinkCount, int fastBlinkDuration, int slowBlinkDuration) {
  // Blink the LED fast for fastBlinkDuration seconds
  Serial.println("Fast blink actual runtime:");
  Serial.println(blinkControl(fastBlinkCount, fastBlinkDuration));
  // Blink the LED slow for slowBlinkDuration seconds
  Serial.println("Slow blink actual runtime:");
  Serial.println(blinkControl(slowBlinkCount, slowBlinkDuration));
}



// Once again, I'm overthinking this... 
// Simpler solution: 
// 1) Start the loop with duration as desired
// 2) run it like that for some interval (10min on first setup, then move to hourly)
// 3) calculate an average actual runtime and adjust the duration accordingly 
// (maybe store this adjustment somewhere FOR SCIENCE?)

unsigned long blinkControl(int numberOfBlinks, unsigned long duration) {

  if (duration == fastBlinkDuration) {
    int offset = syncToMinute(tolerance); // get the offset within tolerance to adjust to it here
    Serial.print("Offset: ");
    Serial.println(offset);
    time_t now;
    time(&now);
    // Serial.println(ctime(&now));

    duration = (duration - offset) * 1000; // account for offset (only for fast blink, as it's the first one)
  }
  else {
    duration *= 1000; // convert to milliseconds
  }

  unsigned long startTime = millis();
  unsigned long blinkInterval = duration / numberOfBlinks; // calculate the interval between each blink

  // Splitting it up into two parts, so I can continually adjust the blink interval and avoid overflow, even if execution time is different than expected. Really, I only want this for the 2nd phase, but it doesn't hurt to have it for the first phase as well and lets me use the same function for both phases.
  int a = 4 * numberOfBlinks / 5; // 80% of the blinks using integer division
  int b = numberOfBlinks - a; 

  // I could of course reduce the copy paste, but I'm too lazy to do that right now
  for (int i = 0; i < a; i++) {
    // Check if the command to stop the blinking has been received. If so, return.
    if (stopBlinking) {
      return 1;
    }
    digitalWrite(LED_PIN, HIGH); 
    delay(blinkInterval / 2); 
    digitalWrite(LED_PIN, LOW); 
    delay(blinkInterval / 2); 
    blinks += 1;
  }

  // delay(rand() % 4000); // Random delay between the two phases 

  unsigned long endOfA = millis();
  unsigned long runtimeA = endOfA - startTime;
  // float factor = runtimeA / (endOfA - startTime);
  // // The factor checks how much faster or slower the execution was than expected, so that the remaining blinks can be adjusted, with the expectance that the execution time will be similar
  // blinkInterval = ((duration - runtimeA) / factor) / b; 
  blinkInterval = (duration - runtimeA) / b; // Adjust the blink interval for the remaining blinks

  for (int i = 0; i < b; i++) {
    // Check if the command to stop the blinking has been received. If so, return.
    if (stopBlinking) {
      return 1;
    }
    digitalWrite(LED_PIN, HIGH); 
    delay(blinkInterval / 2); 
    digitalWrite(LED_PIN, LOW); 
    delay(blinkInterval / 2); 
    blinks += 1;
  }
  return millis() - startTime; // return the total runtime of the function
}

// Checks current offset from the start of the minute. If within tolerance, returns the offset. Otherwise, delays until the start of the next minute. 
int syncToMinute(int tolerance) {
  time_t now;
  time(&now);
  struct tm* currentTime = localtime(&now);
  int seconds = currentTime->tm_sec;

  if (tolerance < seconds && seconds < (60 - tolerance)) {
    // Calculate the remaining seconds and milliseconds in the current minute
    int remainingSeconds = 59 - seconds;
    long remainingMillis = 1000 - (millis() % 1000);
    Serial.println("Syncing to minute");
    Serial.println(remainingSeconds);

    // Delay for the remaining time
    delay(remainingSeconds * 1000 + remainingMillis);
    Serial.println("Synced to minute");
    return 0;
  }
  // Could do this with ternary, but it's 2:55 am
  else {
    if (seconds <= 30){
      return seconds;
    }
    else {
      return 60 - seconds;
    }
  }
}

// Delays until specified time
void delayUntil(int targetHour, int targetMinute) {
  time_t now;
  struct tm* currentTime;

  do {
    time(&now);
    currentTime = localtime(&now);
  } while(currentTime->tm_hour != targetHour || currentTime->tm_min != targetMinute);

  // Execution will continue here at the target time (ur just use it to delay the loop)
}