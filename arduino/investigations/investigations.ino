#include <Arduino.h>


bool animationDirection = false; // default is the original top down

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  Serial.println("Hello World");

}

void loop() {
  Serial.println("Hello World");
  // put your main code here, to run repeatedly:
  size_t rows = 74; // number of rows
  drawAndFadeRectangle(rows, 20); // for green
  drawAndFadeRectangle(rows, 50); // for red
}


void fadeRow(float duration, int stepSize = 8 ) {

  stepSize = 12; // for 21 steps for 50s -> 31fps
  if (duration < 300) {
    stepSize = 28; // for 9 steps for 20s -> 30fps
  }
  

  for (int brightness = 252; brightness > 0; brightness -= stepSize) { // fade color from max to off

    // A bit of math to calculate the delay time for each step. 
    int delayTime = duration / (252/stepSize);
    delay(delayTime);
  }
}


int drawAndFadeRectangle(size_t rows, unsigned long duration) {

  duration *= 1000; // convert to milliseconds

  unsigned long startTime = millis();
  float fadeRowTime = duration * 1.0 / rows; 

  Serial.print("fadeRowTime:");
  Serial.println(fadeRowTime);

  //MARK: TODO: handle the animation direction oddness

  // first 80% of rows  
  if (!animationDirection) { // Top-down animation
    for (size_t i = 0; i < rows; i++) {
      fadeRow(fadeRowTime);
    }
  } else { // Bottom-up animation
    for (size_t i = rows; i > 0; i--) {
      fadeRow(fadeRowTime); // why -1 again?
    }
  }

  Serial.print("Phase runtime:");
  Serial.println(millis() - startTime);
  return millis() - startTime; // return the total runtime of the function
}
