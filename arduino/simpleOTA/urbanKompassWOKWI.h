
// TEST FOR USE WITH wokwi.com


/*
Original code from urbanCompass: https://github.com/lucasoldenburg/urbancompass
Renamed to urbanKompass.h, to show that Ks worked on it :)
*/

#include <Arduino.h>
#include "time.h"

// Get this from main.ino (currently simpleOTA.ino)
// TODO: should probably put this in a struct
int globalPhase1Length = 24; // in seconds
int globalPhase2Length = 46;
int totalPhaseLength = globalPhase1Length + globalPhase2Length; // total length of the cycle in seconds
int globalTolerance;
int globalNtpUpdateInterval; // in minutes
int displayChoice; // 1 = urbanKompass, 2 = iterateBitmaps, 
bool stopDisplay = false; // to stop the display early
bool animationDirection; // default is the original top down

int completedCycles = 0;

// where to start the rectangle (32 from phantom half matrix, 23 from bike pictogram) (was originally 55)
// firstRow + rows should be 128 (total "width" of the display, including phantom matrix)
int firstRow = 54; 

int weekdayOffsets[7] = {60, 40, 20, 0, 50, 30, 10}; // in seconds,(0 = Monday, 1 = Tuesday, etc.)

//MARK: getCurrentTimeInSeconds()
// simple helper function which calculates time in seconds since 00:00:00 for modulo calculations (daily drift due to 70s cycle-length)
int getCurrentTimeInSeconds() {
  time_t now = time(NULL);
  struct tm *timeCheck = localtime(&now);
  return timeCheck->tm_hour * 3600 + timeCheck->tm_min * 60 + timeCheck->tm_sec;
}


//MARK: delayUntil
// Delays until specified time, currently with a buffer of 1 second because of the early start of North traffic light
// secondsBuffer isn't currently used
void delayUntil(unsigned int targetHour, unsigned int targetMinute, unsigned int targetSecond, unsigned int secondsBuffer = 0) {
  time_t now;
  struct tm* currentTime;

  targetSecond = targetSecond > 59 ? 59 : targetSecond % 60; // Just in case someone doesn't understand time

  // Adjusts the target time for the early start of north traffic light
  if (targetSecond == 0) {
    targetSecond = 60 - 1; // not sure why, but it kept being off by one second, so I'm compensating for that here 
    if (targetMinute == 0) {
      targetMinute = 59;
      // still getting used to ternary operator. 
      // targetHour is set to:(if targetHour is 0, set it to 23, otherwise set it to targetHour - 1)
      targetHour = targetHour == 0 ? 23 : targetHour - 1; 
    } else {
      targetMinute -= 1;
    }
  }

  Serial.println("Target time: " + String(targetHour) + ":" + String(targetMinute) + ":" + String(targetSecond));
  Serial.println("Waiting for target time...");

  // wait until just before the target time is reached
  do {
    time(&now);
    currentTime = localtime(&now);
    Serial.print(".");
    delay(500); // half a second accuracy is good enough?
  // we can safely use targetSecond -1 because if 0 is the target, it will be set to 59 in the previous block
  } while(!(currentTime->tm_hour == targetHour && currentTime->tm_min == targetMinute && currentTime->tm_sec == targetSecond - 1));
}


//MARK: syncedStart()
//MARK: check if this works
// starts new cycle at proper time, adjusted for day of the week
void syncedStart(){
  time_t now;
  time(&now);
  // get current weekday
  int weekday = localtime(&now)->tm_wday;   // tm_wday is 0-6 (0 = Sunday)
  weekday = weekday == 0 ? 6 : weekday - 1; // convert to 0 = Monday
  // retrieve the offset for the current day
  int offset = weekdayOffsets[weekday];
  if (offset == 0) offset = 60; // to get to next one
  // find next available start time
  int currentTimeInSeconds = getCurrentTimeInSeconds();
  int secondsToNextStart = offset - (currentTimeInSeconds % totalPhaseLength);
  int nextStart = currentTimeInSeconds + secondsToNextStart;
  int nextStartHour = nextStart / 3600;
  int nextStartMinute = (nextStart % 3600) / 60;
  int nextStartSecond = nextStart % 60;
  // delay until next start time
  delayUntil(nextStartHour, nextStartMinute, nextStartSecond);
}


// This function fades a single row from the given color to black over duration in milliseconds and adjusts the number of iterations to a target fps (default 30).
void fadeRow(int r, int g, int b, int i, float fadeRowTime, int targetFps = 30) {

  int fadeSteps = 1 + static_cast<int>(targetFps * fadeRowTime/1000); // a bit complicated, but it's just to round up to the nearest integer, to ensure fps is >= targetFps
  float accumulatedError = 0; // to keep track of the error in converting delayTime to int
  // we could put this in the outer loop, but worst case, we're off by 1ms per row

  // Serial.print("fadeSteps: ");
  // Serial.println(fadeSteps);

  for (int j = 1; j <= fadeSteps; j++) {

    // Check if the command to stop the display has been received. If so, clear the display and return. I placed it here, so that it would be as responsible as possible.
  if (stopDisplay) return; 

    int fadedR = (fadeSteps - j) * r / fadeSteps; // order of operations is crucial here, to make proper use of integer division
    int fadedG = (fadeSteps - j) * g / fadeSteps;
    int fadedB = (fadeSteps - j) * b / fadeSteps;

    // dma_display->drawFastVLine(firstRow + i, 0, 32, dma_display->color565(fadedR, fadedG, fadedB));

    // A bit of math to calculate the delay time for each step. 
    // delay() only accepts integers, so we need to round the float to the nearest integer. To account for the accumulation of the truncated decimals, we keep track of that error and add it to the next delay once it reaches a full millisecond.
    //MARK: TODO:
    // - needs to be adjusted to account for being too slow
    float delayTime_float = fadeRowTime / fadeSteps;
    int delayTime = static_cast<int>(delayTime_float);
    accumulatedError += delayTime_float - delayTime; 
    // if the error is larger than 1ms, we adjust the delayTime
    if (accumulatedError >= 1 || accumulatedError <= -1) {  
      int errorInt = static_cast<int>(accumulatedError); // get the integer part of the error
      delayTime += errorInt; 
      accumulatedError -= errorInt; // subtract the integer part from the error (to not forget 0.99 or such)
    }
    delay(delayTime);

  }
}

// Call this with the RGB values and the number of rows to draw and fade.
// The duration is the total time it should take to fade the rectangle (in seconds).
int drawAndFadeRectangle(int r, int g, int b, size_t rows, unsigned int duration) {

  //MARK: TODO: add this once syncToMinute is implemented
  // IMPORTANT: this needs to account for the 70s thing, so it can only sync to every 7th full minute... -> ignore this for now and come back to it once Ticker.h is set up
  // if (duration == fastBlinkDuration) {
  // int offset = syncToMinute(globalTolerance); // get the offset within tolerance to adjust to it here
  // Serial.print("Offset: ");
  // Serial.println(offset);
  // time_t now;
  // time(&now);
  // // Serial.println(ctime(&now));

  // duration = (duration * 1000  - offset); // account for offset (only for fast blink, as it's the first one)
  // }
  // else {
  // duration *= 1000; // convert to milliseconds
  // }

  // remove this if the above is implemented
  duration *= 1000; // convert to milliseconds

  unsigned long startTime = millis();
  float fadeRowTime = duration * 1.0 / rows; // calculate the time for each row to fade. needs to be cast to float to avoid integer division (and truncating decimals)
  // this NEEDS to NOT be unsigned long, as that's an integer and we want to keep the decimals!

  // Serial.print("Fade row time:");
  // Serial.println(fadeRowTime);

  // dma_display->fillRect(firstRow, 0, rows, 32, dma_display->color565(r, g, b)); //draw full rectangle // playing with 1st parameter, original = 55

  if (!animationDirection) { // Top-down animation
    for (size_t i = 0; i < rows; i++) {
      fadeRow(r, g, b, i, fadeRowTime);
    }
  } else { // Bottom-up animation
    for (size_t i = rows; i > 0; i--) {
      fadeRow(r, g, b, i - 1, fadeRowTime); 
    }
  }

  int actualDuration = millis() - startTime;
  int error = duration - actualDuration;
  if (error > 0) {
    delay(error - 2); // do (error - 2)? Better to be a bit too fast than too slow -> can be fixed with a simple delay in the main loop
  }  
  Serial.print("Phase runtime:");
  Serial.println(actualDuration);
  Serial.print("Adjusted runtime:");
  Serial.println(millis() - startTime);
  return millis() - startTime; // return the total runtime of the function
}

//MARK: loop
void urbanKompassLoop() {

  // keep this here or not?
  // dma_display->drawBitmap(31, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); // draw bike pictogram

  size_t rows = 74; // number of rows

  drawAndFadeRectangle(0, 255, 0, rows, globalPhase1Length); // for green
  // syncToMinute here? to avoid 70s cycle skipping a minute every now and then

  // kind of silly to add this twice, but this is the easiest way 
  if (stopDisplay) return; // needs to be return if this isn't main loop

  drawAndFadeRectangle(255, 0, 0, rows, globalPhase2Length); // for red

  if (stopDisplay) return; // needs to be return if this isn't main loop
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  time_t now;
  time(&now);
  Serial.println("Clock time: " + String(ctime(&now))); // Print the current time to the serial monitor
}

void loop() {
  Serial.print("Start at:");
  unsigned long startTime = millis();
  Serial.println(startTime);

  syncedStart();

  urbanKompassLoop();
  completedCycles++;
  if (completedCycles % 10 == 0) {
    Serial.print("10 cycles done at:");
    Serial.println(millis()-startTime);
  }
  // delay(10); // this speeds up the simulation
}
