// to actually start after upload, press the RESET button on esp32
// remember to actually be on same network...

// either access via IP or http://esp32.local/ (replace esp32 with whatever the host is set up as)

// login for webserver in line: "if(form.userid.value=='admin' && form.pwd.value=='Rlb_Ampel<3')"
// the password doesn't actually do any sort of login, it just leads to ../serverIndex page

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

// adjust these according to the actual traffic light
const unsigned long greenPhase = 120000;  // 2min
const unsigned long redPhase = 30000;  // 30s

const int led = 13; // ESP32 Pin to which onboard LED is connected
bool ledState = false;  // ledState used to set the LED (int LOW/HIGH is same as bool false/true for digitalWrite())

WebServer server(80);


void setup() {
  pinMode(led,  OUTPUT);  // for blink test
  Serial.begin(115200);   // for debugging
  connectToWiFiAndSetupMDNS(ssid, password, host);   // initial connection to wifi and sets alternative IP -> http://"host".local
  setupServer();          // this one starts the web server, which handles all the OTA stuff

  // starts the two tasks/loops that are always running on specific cores
  // xTaskCreatePinnedToCore(handleServer, "Handle Server", 10000, NULL, 1, NULL, 0);    // 1st Core (last parameter)
  // xTaskCreatePinnedToCore(updateDisplay, "Update Display", 10000, NULL, 1, NULL, 1);  // 2nd Core 
}


// new plan
// instead of all the fancy overthought and overengineered stuff, just put it all in one loop 
void loop() {

  Serial.println("loop");

  // still needs a way to reset it daily -> Time / NTP

  unsigned long t1 = millis();

  // do housekeeping here
  if (WiFi.status() != WL_CONNECTED) {   // check if WiFi is still connected, attempt to reconnect if so
    digitalWrite(led, false);
    connectToWiFiAndSetupMDNS(ssid, password, host);
  }
  server.handleClient();   // check if something from server is requested
  // add whatever else needs to be checked here

  unsigned long houseKeepingTime = millis() - t1;

  // green phase, let's say 2min 
  unsigned long greenPhaseActual = 120000 - houseKeepingTime;  // 2min - time spent on housekeeping

  //Serial.println(greenPhaseActual);
  

  // run phase 1 

  // run phase 2 

  delay(1); // do I need to accound for milliseconds?
}