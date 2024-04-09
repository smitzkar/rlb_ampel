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
void fadeRow(int r, int g, int b, size_t i, size_t rows, unsigned long duration, int stepSize = 8) {
  for (int brightness = 255; brightness > 0; brightness -= stepSize) { // fade color from max to off

    // Check if the command to stop the display has been received. If so, clear the display and return.
    // Isn't really used, yet. 
    if (stopDisplay) {
      dma_display->fillScreen(0); 
      return;
    }

    // It's hideous, but I'll let the compiler handle it. Should probably modulo this. 
    int fadedR = (r * brightness) / 255;
    if (fadedR < stepSize) fadedR = 0;
    int fadedG = (g * brightness) / 255;
    if (fadedG < stepSize) fadedG = 0; 
    int fadedB = (b * brightness) / 255;
    if (fadedB < stepSize) fadedB = 0;
    dma_display->drawFastVLine(56 + i, 0, 32, dma_display->color565(fadedR, fadedG, fadedB));

    // A bit of math to calculate the delay time for each step. 
    int delayTime = duration / (255/stepSize);
    delay(delayTime);
  }
}

// Call this with the RGB values and the number of rows to draw and fade.
// The duration is the total time it should take to fade the rectangle (in seconds).
int drawAndFadeRectangle(int r, int g, int b, size_t rows, unsigned long duration) {
  dma_display->fillRect(56, 0, 73, 32, dma_display->color565(r, g, b)); //draw full rectangle

  //MARK: TODO: add this once syncToMinute is implemented
  // if (duration == fastBlinkDuration) {
  // int offset = syncToMinute(globalTolerance); // get the offset within tolerance to adjust to it here
  // Serial.print("Offset: ");
  // Serial.println(offset);
  // time_t now;
  // time(&now);
  // // Serial.println(ctime(&now));

  // duration = (duration - offset) * 1000; // account for offset (only for fast blink, as it's the first one)
  // }
  // else {
  //   duration *= 1000; // convert to milliseconds
  // }
  duration *= 1000; // remove this line if the above code is uncommented

  unsigned long startTime = millis();
  unsigned long fadeRowTime = duration / rows; // calculate the time for each row to fade

  // Splitting it up into two parts, so I can continually adjust the time and avoid overflow, even if execution time is different than expected. Really, I only want this for the 2nd phase, but it doesn't hurt to have it for the first phase as well and lets me use the same function for both phases.
  int partARows = 4 * rows / 5; // 80% of the rows using integer division
  int partBRows = rows - partARows; // the remaining 20% of the rows



  // first 80% of rows  
  if (!animationDirection) { // Top-down animation
    for (size_t i = 0; i < partARows; i++) {
      fadeRow(r, g, b, i, partARows, fadeRowTime);
    }
  } else { // Bottom-up animation
    for (size_t i = partARows; i > 0; i--) {
      fadeRow(r, g, b, i - 1, partARows, fadeRowTime);
    }
  }

  // calculate how long it took and how much time is left
  unsigned long endOfA = millis();
  unsigned long runtimeA = endOfA - startTime;
  fadeRowTime = (duration - runtimeA) / partBRows; // adjust accordingly for the remaining rows

  // remaining 20% of rows
  if (!animationDirection) { // Top-down animation
    for (size_t i = partARows; i < rows; i++) {
      fadeRow(r, g, b, i, partBRows, fadeRowTime);
    }
  } else { // Bottom-up animation
    for (size_t i = rows; i > partARows; i--) {
      fadeRow(r, g, b, i - 1, partBRows, fadeRowTime);
    }
  }

  Serial.print("Phase runtime:");
  Serial.println(millis() - startTime);
  return millis() - startTime; // return the total runtime of the function
}

//MARK: loop
void urbanKompassLoop() {

  size_t rows = 73; // number of rows
  drawAndFadeRectangle(0, 255, 0, rows, globalPhase1Length); // for green
  if (stopDisplay) return; // needs to be return if this isn't main loop
  drawAndFadeRectangle(255, 0, 0, rows, globalPhase2Length); // for red
}

