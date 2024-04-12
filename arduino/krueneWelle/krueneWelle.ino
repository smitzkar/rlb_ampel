#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>
#include "setupDisplay.h"



void setup() {

  Serial.begin(115200);   // for output to serial monitor

  // Moved this from urbanKompass.h, because I think it only needs to run once and is required for all display options
  /************** DISPLAY **************/
  Serial.println("...Starting Display"); 
  dma_display = new MatrixPanel_I2S_DMA(mxconfig); // I think it just grabs mxconfig from the setupDisplay.h file
  dma_display->begin();
  dma_display->setBrightness8(255); //0-255
  delay(2000);

}

void loop() {
  // put your main code here, to run repeatedly:

  dma_display->drawBitmap(33, 0, NUMBERS_bits, 96, 32, dma_display->color565(255,255,255)); // to show the full display, it needs to start at 32+1 ("phantom" 1st half of 64x32 matrix)
  //dma_display->drawPixel(33, 1, dma_display->color565(0,255,255)); // direct pixel control
  fillRect(33, 5, 5, 5, dma_display->color565(0,255,0)); // from krueneWelle.h
  fillRect(39, 5, 5, 5, dma_display->color565(20,0,0)); // from krueneWelle.h
  delay(5000);
  fillRect(33, 5, 5, 5, dma_display->color565(0,20,0)); // from krueneWelle.h
  fillRect(39, 5, 5, 5, dma_display->color565(255,0,0)); // from krueneWelle.h
  delay(5000);


}


void fillRect(int startX, int startY, int width, int height, uint16_t color) {
  for (int y = startY; y < startY + height; y++) {
    for (int x = startX; x < startX + width; x++) {
      dma_display->drawPixel(x, y, color);
    }
  }
}


