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

    // One could create a proper formula for consistent-ish fps, but this'll do for now
    // takes bigger steps for faster animations, because it's less noticeable and keeps computations down
    if (duration <= 30) {
      stepSize = 16;
    }

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
    dma_display->drawFastVLine(55 + i, 0, 32, dma_display->color565(fadedR, fadedG, fadedB));

    // A bit of math to calculate the delay time for each step. 
    int delayTime = duration / (256/stepSize);
    // Serial.print("delayTime:");
    // Serial.println(delayTime);
    delay(delayTime);
  }
}

// Call this with the RGB values and the number of rows to draw and fade.
// The duration is the total time it should take to fade the rectangle (in seconds).
int drawAndFadeRectangle(int r, int g, int b, size_t rows, unsigned long duration) {

  //MARK: TODO: add this once syncToMinute is implemented
  // if (duration == fastBlinkDuration) {
  // int offset = syncToMinute(globalTolerance); // get the offset within tolerance to adjust to it here
  // Serial.print("Offset: ");
  // Serial.println(offset);
  // time_t now;
  // time(&now);
  // // Serial.println(ctime(&now));

  // duration = (duration - offset); // account for offset (only for fast blink, as it's the first one)
  // }

  unsigned long startTime = millis();
  unsigned long fadeRowTime = duration * 1000.0 / rows; // calculate the time for each row to fade in seconds. needs to be cast to float to avoid integer division (and truncating decimals)
  Serial.print("fadeRowTime:");
  Serial.println(fadeRowTime);
  Serial.print("Expected runtime:");
  Serial.println(fadeRowTime * rows);

  dma_display->fillRect(55, 0, 73, 32, dma_display->color565(r, g, b)); //draw full rectangle

  if (!animationDirection) { // Top-down animation
    for (size_t i = 0; i < rows; i++) {
      fadeRow(r, g, b, i, rows, fadeRowTime);
    }
  } else { // Bottom-up animation
    for (size_t i = rows; i > 0; i--) {
      fadeRow(r, g, b, i - 1, rows, fadeRowTime);
    }
  }

  Serial.print("Phase runtime:");
  Serial.println(millis() - startTime);
  return millis() - startTime; // return the total runtime of the function for offset calculation
}

//MARK: loop
void urbanKompassLoop() {

  // keep this here or not?
  dma_display->drawBitmap(32, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); // draw bike pictogram

  size_t rows = 73; // number of rows
  drawAndFadeRectangle(0, 255, 0, rows, globalPhase1Length); // for green
  if (stopDisplay) return; // needs to be "return" if this isn't actual loop
  drawAndFadeRectangle(255, 0, 0, rows, globalPhase2Length); // for red
}

