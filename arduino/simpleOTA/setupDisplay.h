// moved this into it's own file, so that it is done once, even if different display options are chosen and initialised


#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>


/*--------------------- DEBUG  -------------------------*/
#define Sprintln(a) (Serial.println(a))
#define SprintlnDEC(a, x) (Serial.println(a, x))

#define Sprint(a) (Serial.print(a))
#define SprintDEC(a, x) (Serial.print(a, x))


/*--------------------- RGB DISPLAY PINS -------------------------*/
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13 // what is this for??
#define A_PIN 23
#define B_PIN 19
#define C_PIN 5
#define D_PIN 17
#define E_PIN -1
#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16


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