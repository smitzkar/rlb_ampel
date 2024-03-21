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
#define B2_PIN 13
#define A_PIN 23
#define B_PIN 19
#define C_PIN 5
#define D_PIN 17
#define E_PIN -1
#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16


/*--------------------- MATRIX LILBRARY CONFIG -------------------------*/
#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 2      // Total number of panels chained one to another
 
MatrixPanel_I2S_DMA *dma_display = nullptr;

// Module configuration
HUB75_I2S_CFG mxconfig(
	PANEL_RES_X,   // module width
	PANEL_RES_Y,   // module height
	PANEL_CHAIN    // Chain length
);

/* Bitmaps */
#include <bitmaps.h>

int current_bitmap = 0;
static int num_of_bitmaps = 4;

const uint8_t *bitmap_bits[3] = {
  bezirke_quer_mono,
  uhrzeit_quer_mono,
  sights_quer_mono
};

void drawUBahn() { //build multicolor display by layering monochrome bitmaps
  dma_display->drawBitmap(33, 0, u1_mono, 96, 32, dma_display->color565(85,242,0));
  dma_display->drawBitmap(33, 0, u5_mono, 96, 32, dma_display->color565(102, 64, 25));
  dma_display->drawBitmap(33, 0, u8_mono, 96, 32, dma_display->color565(0, 97, 218));
  dma_display->drawBitmap(33, 0, ubahn_text_mono, 96, 32, dma_display->color565(255,255,255));
};


void setup() {
  delay(1000); Serial.begin(115200); delay(200);

  /************** DISPLAY **************/
  Sprintln("...Starting Display");
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(255); //0-255
}


void loop() { //iterrate through bitmaps

  if (current_bitmap == 3) {// exit for layerd U-Bahn display
    drawUBahn();
  } else { //exit for single monochrome bitmaps
  dma_display->drawBitmap(33, 0, bitmap_bits[current_bitmap], 96, 32, dma_display->color565(255,255,255));
  }

  current_bitmap = (current_bitmap  + 1 ) % num_of_bitmaps; //set counter for next bitmap
  delay(5000);
  dma_display->clearScreen();
  
}