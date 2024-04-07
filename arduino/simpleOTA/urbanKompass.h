/*
Original code from urbanCompass: https://github.com/lucasoldenburg/urbancompass
Renamed to urbanKompass.h, to show that Ks worked on it :)
*/

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>

// Get this from main.ino (currently simpleOTA.ino)
extern bool stopDisplay;
extern int globalPhase1Length; // in seconds
extern int globalPhase2Length;
extern int globalTolerance;
extern int globalNtpUpdateInterval; // in minutes
extern int displayChoice; // 1 = urbanKompass, 2 = iterateBitmaps, 
extern bool animationDirection; // default is the original top down


// /*--------------------- DEBUG  -------------------------*/
// #define Sprintln(a) (Serial.println(a))
// #define SprintlnDEC(a, x) (Serial.println(a, x))

// #define Sprint(a) (Serial.print(a))
// #define SprintDEC(a, x) (Serial.print(a, x))


// /*--------------------- RGB DISPLAY PINS -------------------------*/
// #define R1_PIN 25
// #define G1_PIN 26
// #define B1_PIN 27
// #define R2_PIN 14
// #define G2_PIN 12
// #define B2_PIN 13 // what is this for??
// #define A_PIN 23
// #define B_PIN 19
// #define C_PIN 5
// #define D_PIN 17
// #define E_PIN -1
// #define LAT_PIN 4
// #define OE_PIN 15
// #define CLK_PIN 16


// /*--------------------- MATRIX LILBRARY CONFIG -------------------------*/
// #define PANEL_RES_X 64     // Number of pixels wide of each INDIVIDUAL panel module. 
// #define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
// #define PANEL_CHAIN 2      // Total number of panels chained one to another
 
// MatrixPanel_I2S_DMA *dma_display = nullptr;

// // Module configuration
// HUB75_I2S_CFG mxconfig(
//   PANEL_RES_X,   // module width
//   PANEL_RES_Y,   // module height
//   PANEL_CHAIN    // Chain length
// );

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

//MARK: helper functions
// New helper function to fade the rows
void fadeRow(int r, int g, int b, size_t i, size_t rows, unsigned long startTime, unsigned long duration, int stepSize = 16) {
  for (int brightness = 255; brightness > 0; brightness -= stepSize) { // fade color from max to off

    // Check if the command to stop the display has been received. If so, clear the display and return.
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

//MARK: setup
void urbanKompassSetup() {
  // Ks: Might need to move this to main.ino, but remove the drawBitmap(.. bike_vertical_mono ..) part

  // put your setup code here, to run once:
  // delay(1000); Serial.begin(115200); delay(200); // what is this for??

  delay(2000); // to fix the display not starting properly

  /************** DISPLAY **************/
  Sprintln("...Starting Display");
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(255); //0-255

  // add some delay to maybe give it time to properly initialise everything -> prevent faulty early display loops 
  delay(2000);

  // draw bike pictogram
  // does this need to be called on every iteration? Doesn't really update anything
  dma_display->drawBitmap(33, 0, bike_vertical_mono, 32, 32, dma_display->color565(220,220,220));

}

//MARK: loop
void urbanKompassLoop() {

  // // draw bike pictogram
  // // does this need to be called on every iteration? Doesn't really update anything
  // // moved it out of here and to setup
  // dma_display->drawBitmap(33, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255));

  size_t rows = 73; // number of rows
  drawAndFadeRectangle(0, 255, 0, rows, globalPhase1Length); // for green
  if (stopDisplay) return; // needs to be return if this isn't main loop
  drawAndFadeRectangle(255, 0, 0, rows, globalPhase2Length); // for red
}

