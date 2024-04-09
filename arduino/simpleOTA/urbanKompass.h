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
void fadeRow(int r, int g, int b, size_t i, size_t rows, unsigned long startTime, unsigned long duration, int stepSize = 8) {
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
    unsigned long elapsedTime = millis() - startTime;
    unsigned long remainingTime = duration > elapsedTime ? duration - elapsedTime : 0;
    unsigned long completedSteps = animationDirection ? (rows - i - 1) * 255/stepSize + (255 - brightness)/stepSize : i * 255/stepSize + (255 - brightness)/stepSize;
    unsigned long remainingSteps = rows * 255/stepSize - completedSteps;
    unsigned long delayTime = remainingSteps > 0 ? remainingTime / remainingSteps : 0;
    delay(delayTime);
  }
}

// Call this with the RGB values and the number of rows to draw and fade.
// The duration is the total time it should take to fade the rectangle (in seconds).
void drawAndFadeRectangle(int r, int g, int b, size_t rows, unsigned long duration) {
  dma_display->fillRect(56, 0, 73, 32, dma_display->color565(r, g, b)); //draw full rectangle
  unsigned long startTime = millis();
  duration *= 1000; // convert to milliseconds
  
  if (!animationDirection) { // Top-down animation
    for (size_t i = 0; i < rows; i++) {
      fadeRow(r, g, b, i, rows, startTime, duration);
    }
  } else { // Bottom-up animation
    for (size_t i = rows; i > 0; i--) {
      fadeRow(r, g, b, i - 1, rows, startTime, duration);
    }
  }
}

//MARK: loop
void urbanKompassLoop() {

  size_t rows = 73; // number of rows
  drawAndFadeRectangle(0, 255, 0, rows, globalPhase1Length); // for green
  if (stopDisplay) return; // needs to be return if this isn't main loop
  drawAndFadeRectangle(255, 0, 0, rows, globalPhase2Length); // for red
}

