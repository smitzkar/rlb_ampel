/*
Original code from urbanCompass: https://github.com/lucasoldenburg/urbancompass
Renamed to urbanKompass.h, to show that Ks worked on it :)
*/

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>
#include "time.h"

// Get this from main.ino (currently simpleOTA.ino)
extern bool stopDisplay;
extern int globalPhase1Length; // in seconds
extern int globalPhase2Length;
extern int globalTolerance;
extern int globalNtpUpdateInterval; // in minutes
extern int displayChoice; // 1 = urbanKompass, 2 = iterateBitmaps, 
extern bool animationDirection; // default is the original top down


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
void fadeRow(int r, int g, int b, int i, float duration, int targetFps = 30) {

  int iterations = 1 + static_cast<int>(targetFps * duration/1000); // a bit complicated, but it's just to round up to the nearest integer, to ensure fps is >= targetFps

  for (int j = 1; j <= iterations; j++) {

    // Check if the command to stop the display has been received. If so, clear the display and return. I placed it here, so that it would be as responsible as possible.
    // Isn't really used, yet. 
    if (stopDisplay) {
      dma_display->fillScreen(0); 
      return;
    }

    int fadedR = (iterations - j) * r / iterations; // order of operations is crucial here, to make proper use of integer division
    int fadedG = (iterations - j) * g / iterations;
    int fadedB = (iterations - j) * b / iterations;

    dma_display->drawFastVLine(55 + i, 0, 32, dma_display->color565(fadedR, fadedG, fadedB));

    // A bit of math to calculate the delay time for each step. 
    int delayTime = duration / iterations;
    delay(delayTime);
  }
}

// Call this with the RGB values and the number of rows to draw and fade.
// The duration is the total time it should take to fade the rectangle (in seconds).
int drawAndFadeRectangle(int r, int g, int b, size_t rows, unsigned int duration) {

  //MARK: TODO: add this once syncToMinute is implemented
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

  dma_display->fillRect(55, 0, rows, 32, dma_display->color565(r, g, b)); //draw full rectangle

  //MARK: TODO: handle the animation direction oddness

  if (!animationDirection) { // Top-down animation
    for (size_t i = 0; i < rows; i++) {
      fadeRow(r, g, b, i, fadeRowTime);
    }
  } else { // Bottom-up animation
    for (size_t i = rows; i > 0; i--) {
      fadeRow(r, g, b, i - 1, fadeRowTime); // why -1 again?
    }
  }

  Serial.print("Phase runtime:");
  Serial.println(millis() - startTime);
  return millis() - startTime; // return the total runtime of the function
}

//MARK: loop
void urbanKompassLoop() {

  // keep this here or not?
  // dma_display->drawBitmap(31, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); // draw bike pictogram

  size_t rows = 74; // number of rows
  drawAndFadeRectangle(0, 255, 0, rows, globalPhase1Length); // for green
  if (stopDisplay) return; // needs to be return if this isn't main loop
  // syncToMinute here? to avoid 70s cycle skipping a minute every now and then
  drawAndFadeRectangle(255, 0, 0, rows, globalPhase2Length); // for red
}

