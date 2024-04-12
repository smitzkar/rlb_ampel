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
  // at 74 rows, at optimal runtime it will have 292ms buffer per cycle
  // if not enough, at 73: 1234ms 
  drawAndFadeRectangle(rows, 20); // for green
  drawAndFadeRectangle(rows, 50); // for red
}


void fadeRow(float duration, int targetFps = 30) {

  int iterations = 1 + static_cast<int>(targetFps * duration/1000); // a bit complicated, but it's just to round up to the nearest integer, to ensure fps is >= targetFps

  for (int j = 1; j <= iterations; j++) {
    // A bit of math to calculate the delay time for each step. 
    int delayTime = duration / iterations;
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
