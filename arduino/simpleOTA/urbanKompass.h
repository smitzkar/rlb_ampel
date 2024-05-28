/*
Original code from urbanCompass: https://github.com/lucasoldenburg/urbancompass
Renamed to urbanKompass.h, to show that Ks worked on it :)
*/

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>
#include "time.h"

// Get this from main.ino (currently simpleOTA.ino)
// TODO: should probably put this in a struct
extern bool stopDisplay;
extern int globalPhase1Length; // in seconds
extern int globalPhase2Length;
extern int globalTolerance;
extern int globalNtpUpdateInterval; // in minutes
extern int displayChoice; // 1 = urbanKompass, 2 = iterateBitmaps, 
extern bool stopDisplay; // to stop the display early
extern bool animationDirection; // default is the original top down

// where to start the rectangle (32 from phantom half matrix, 23 from bike pictogram) (was originally 55)
// firstRow + rows should be 128 (total "width" of the display, including phantom matrix)
int firstRow = 54; 


// bike is actually only 23x32 or so, rest is empty -> can maybe redraw it to save some space
const uint8_t bike_vertical_mono[] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x10, 0x10, 0x00,
0x00, 0x20, 0x08, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x47, 0x04, 0x00,
0x00, 0x78, 0x04, 0x00, 0x67, 0xc0, 0x04, 0x00, 0x5b, 0x20, 0x08, 0x00, 0x42, 0x90, 0x10, 0x00,
0x02, 0x48, 0x20, 0x00, 0x02, 0x27, 0xc0, 0x00, 0x02, 0x10, 0x00, 0x00, 0x02, 0x08, 0x00, 0x00,
0x02, 0x07, 0x00, 0x00, 0x12, 0xf9, 0x00, 0x00, 0x17, 0x07, 0xc0, 0x00, 0x19, 0x09, 0x20, 0x00,
0x10, 0xd1, 0x10, 0x00, 0x00, 0x21, 0x08, 0x00, 0x00, 0x59, 0x04, 0x00, 0x00, 0x47, 0x04, 0x00,
0x00, 0x41, 0x04, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x20, 0x08, 0x00,
0x00, 0x10, 0x10, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00
};


//MARK: TODO 
// there's an issue where it fades two rows at once on the border of the two displays 

//MARK: helper functions
// New helper function to fade the rows

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

    dma_display->drawFastVLine(firstRow + i, 0, 32, dma_display->color565(fadedR, fadedG, fadedB));

    // A bit of math to calculate the delay time for each step. 
    // delay() only accepts integers, so we need to round the float to the nearest integer. To account for the accumulation of the truncated decimals, we keep track of that error and add it to the next delay once it reaches a full millisecond.
    //MARK: TODO:
    // - needs to be adjusted to account for being too slow
    // - needs to be adjusted to account for big errors (higher than 1)
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

  dma_display->fillRect(firstRow, 0, rows, 32, dma_display->color565(r, g, b)); //draw full rectangle // playing with 1st parameter, original = 55

  if (!animationDirection) { // Top-down animation
    for (size_t i = 0; i < rows; i++) {
      fadeRow(r, g, b, i, fadeRowTime);
    }
  } else { // Bottom-up animation
    for (size_t i = rows; i > 0; i--) {
      fadeRow(r, g, b, i - 1, fadeRowTime); 
    }
  }

  // check the runtime of the phase and adjust if necessary
  // currently only accounts for it being too fast, not too slow, but the difference should be minimal and be caught on the next NTP sync in houseKeeping()
  // maybe with the adjustments above it now is an issue if slow? 
  int actualDuration = millis() - startTime;
  int error = (globalPhase1Length+globalPhase2Length) - actualDuration;
  if (error > 0) {
    delay(error - 2); // do (error - 2)? Better to be a bit too fast than too slow -> can be fixed with a simple delay in the main loop
  }  
  Serial.print("Phase runtime:");
  Serial.println(actualDuration);
  Serial.print("Adjusted runtime:");
  Serial.println(millis() - startTime);
}

//MARK: loop
void urbanKompassLoop() {

  // keep this here or not?
  dma_display->drawBitmap(31, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); // draw bike pictogram

  size_t rows = 74; // number of rows

  drawAndFadeRectangle(0, 255, 0, rows, globalPhase1Length); // for green
  // syncToMinute here? to avoid 70s cycle skipping a minute every now and then

  Serial.println("Phase 1 done");

  // kind of silly to add this twice, but this is the easiest way 
  if (stopDisplay) return; // needs to be return if this isn't main loop

  Serial.println("Phase 2");

  drawAndFadeRectangle(255, 0, 0, rows, globalPhase2Length); // for red

  if (stopDisplay) return; // needs to be return if this isn't main loop
}

