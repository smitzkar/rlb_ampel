// from https://lastminuteengineers.com/esp32-ota-web-updater-arduino-ide/ 
// OTA Web Updater with better look 

// to actually start after upload, press the RESET button on esp32
// remember to actually be on same network...

// either access via IP or http://esp32.local/ (replace esp32 with whatever the host is set up as)

// login for webserver in line: "if(form.userid.value=='admin' && form.pwd.value=='Rlb_Ampel<3')"
// the password doesn't actually do any sort of login, it just leads to ../serverIndex page

// some issues: 
// can't use delay() because it pauses the whole thing, so it might miss an OTA request
// must ALWAYS include the OTA code when uploading new sketch, because it overwrites everything, so if it's not uploaded again, there's no more OTA functionality

// to upload new sketch (remember to include OTA code):
// arduinoIDE > sketch > export compiled binary

#include <WiFi.h>
#include <WebServer.h> // needs to be here for this part: WebServer server(80);
#include "webpages.h" // needs quotation marks if in same directory
#include "connectToWifi.h"
#include "setupServer.h"

// I'm keeping them here for easier adjustment. Could also be moved to connectToWifi.h, then change the connectToWifiAndSetupMDNS function.
const char* host = "esp32";
const char* ssid = "Karl";
const char* password = "Rlb_KsESP";

// for blink test
// variabls for blinking an LED with Millis
const int led = 13; // ESP32 Pin to which onboard LED is connected
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 1000;  // interval at which to blink (milliseconds)
int ledState = LOW;  // ledState used to set the LED

WebServer server(80);


void setup(void) {

  // for blink test
  pinMode(led,  OUTPUT);

  Serial.begin(115200);

  connectToWiFiAndSetupMDNS(ssid, password, host);
  setupServer();
}

void loop(void) {
  server.handleClient();
  delay(1);

  // for blink test 
  // loop to blink without delay
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
  // save the last time you blinked the LED
  previousMillis = currentMillis;
  // if the LED is off turn it on and vice-versa:
  ledState = not(ledState);
  // set the LED with the ledState of the variable:
  digitalWrite(led,  ledState);
  }
}