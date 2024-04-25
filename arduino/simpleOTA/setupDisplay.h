// moved this into it's own file, so that it is done once, even if different display options are chosen and initialised


#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>


// I don't use this stuff, but I'll keep it for now, in case I missed deleting it somewhere
/*--------------------- DEBUG  -------------------------*/
#define Sprintln(a) (Serial.println(a))
#define SprintlnDEC(a, x) (Serial.println(a, x))

#define Sprint(a) (Serial.print(a))
#define SprintDEC(a, x) (Serial.print(a, x))


/*--------------------- RGB DISPLAY PINS -------------------------*/
// #define R1_PIN 25
// #define G1_PIN 26
// #define B1_PIN 27
// #define R2_PIN 14
// #define G2_PIN 12
// #define B2_PIN 13 
// #define A_PIN 23
// #define B_PIN 19
// #define C_PIN 5
// #define D_PIN 17
// #define E_PIN -1 // to make sure that it doesn't get used?
// #define LAT_PIN 4
// #define OE_PIN 15
// #define CLK_PIN 16

// for new board (HUZZAH32 ESP32 Feather)
// (currently just put them in whereever, clearly needs to be done with some plan -> most likely the last 3 pins, maybe A through D. Or just avoid certain pins? Avoid input only and maybe the named ones)
// #define R1_PIN 21 // 39 // input only
// #define G1_PIN 17 // 36 // input only
// #define B1_PIN 4
// #define R2_PIN 5  // bootstrapping? maybe okay?
// #define G2_PIN 18
// #define B2_PIN 19 
// #define A_PIN 23 // SDA // 13
// #define B_PIN 12
// #define C_PIN 27
// #define D_PIN 33
// #define E_PIN -1 // to make sure that it doesn't get used? 
// #define LAT_PIN 32
// #define OE_PIN 14
// #define CLK_PIN 16 // bootstrapping? 

//16 and 17 seems to fuck with booting?

// https://github.com/adafruit/Adafruit_Protomatter/blob/master/examples/simple/simple.ino
// uint8_t rgbPins[]  = {4, 12, 13, 14, 15, 21};
// uint8_t addrPins[] = {16, 17, 25, 26};
// uint8_t clockPin   = 27; // Must be on same port as rgbPins
// uint8_t latchPin   = 32;
// uint8_t oePin      = 33;

// Replace the old pin definitions with the new ones
#define R1_PIN 4
#define G1_PIN 12
#define B1_PIN 13
#define R2_PIN 14
#define G2_PIN 15
#define B2_PIN 21
#define A_PIN 16
#define B_PIN 17
#define C_PIN 25
#define D_PIN 26
#define E_PIN -1 // to make sure that it doesn't get used? 
#define LAT_PIN 32
#define OE_PIN 33
#define CLK_PIN 27
// Fuck. 
/* from protomatter example: 
  uint8_t clockPin   = 27; // Must be on same port as rgbPins
  uint8_t latchPin   = 32;
  uint8_t oePin      = 33;
The order is different!! */

// https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA?tab=readme-ov-file#how-can-i-configure-it-to-work-with-an-off-the-shelf-boardshield-with-hub75-connector-eg-adafruit-matrixportal
// info on pins 


// from https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/esp32-faq
/*
A2 / I34 - this pin is an input only! You can use it as an analog input so we suggest keeping it for that purpose
A3 / I39 - this pin is an input only! You can use it as an analog input so we suggest keeping it for that purpose
A4 / I36 - this pin is an input only! You can use it as an analog input so we suggest keeping it for that purpose
IO12 - this pin has an internal pulldown, and is used for booting up. We recommend not using it or if you do use it, as an output only so that nothing interferes with the pulldown when the board resets
A13 / I35 - this pin is not exposed, it is used only for measuring the voltage on the battery. The voltage is divided by 2 so be sure to double it once you've done the analog reading
*/




/*--------------------- MATRIX LILBRARY CONFIG -------------------------*/
#define PANEL_RES_X 64     // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 2      // Total number of panels chained one to another
 
MatrixPanel_I2S_DMA *dma_display = nullptr;

// Module configuration
HUB75_I2S_CFG mxconfig(
  PANEL_RES_X,   // module width
  PANEL_RES_Y,   // module height
  PANEL_CHAIN    // Chain length
);