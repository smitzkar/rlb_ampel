#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>

//#include "airlyBitmaps.h" // must only be included once. here or in main.ino (simpleOTA.ino)

int current_bitmap2 = 0;
const int num_of_airlyBitmaps = 4; // set this to 4 again 

const uint8_t *airly_bitmap_bits[4] = { // set this to 4 again
  AIRLY_HAPPY_bits,
  AIRLY_Neutral_bits,
  AIRLY_SAD_bits,
  AIRLY_SAD2_bits,
};


void iterateAirlyBitmapsLoop() { //iterrate through bitmaps


  // might have to steal the layered solution from iterateBitmaps.h
  dma_display->drawBitmap(33, 0, airly_bitmap_bits[current_bitmap2], 96, 32, dma_display->color565(150,150,150));


  current_bitmap2 = (current_bitmap2  + 1 ) % num_of_airlyBitmaps; //set counter for next bitmap
  delay(5000);
  dma_display->clearScreen();
  
}