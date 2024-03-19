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
const long interval = 500;  // interval at which to blink (milliseconds)
int ledState = LOW;  // ledState used to set the LED

WebServer server(80);


void handleServer(void * parameter) {
  // used for millis() function which is like delay(), but doesn't block the thread
  unsigned long lastCheckTime = 0;
  const unsigned long checkInterval = 10 * 60 * 1000; // 10 minutes in milliseconds

  for (;;) { // uses infinite loop to ensure that it runs forever -> same idea as void loop()

    // Check WiFi connection every 10 minutes
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheckTime >= checkInterval) {
      // Check WiFi connection 
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Lost WiFi connection. Attempting to reconnect...");
        connectToWiFiAndSetupMDNS(ssid, password, host);
      }
      lastCheckTime = currentMillis;
    }

    // Handles all the http stuff -> OTA updates, etc. 
    server.handleClient();
    delay(1);
  }
}

void updateDisplay(void * parameter) {
  for (;;) {
    // Update the display
    // Blink test
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      // if the LED is off turn it on and vice-versa:
      ledState = not(ledState);
      // set the LED with the ledState of the variable:
      digitalWrite(led,  ledState);
    }
    delay(1);
  }
}

void setup() {
  // for blink test
  pinMode(led,  OUTPUT);

  Serial.begin(115200);

  // initial connection and setup 
  connectToWiFiAndSetupMDNS(ssid, password, host);
  setupServer();

  xTaskCreatePinnedToCore(handleServer, "Handle Server", 10000, NULL, 1, NULL, 0);    // 1st Core (last parameter)
  xTaskCreatePinnedToCore(updateDisplay, "Update Display", 10000, NULL, 1, NULL, 1);  // 2nd Core 
}

void loop() {
  // No need to put anything here
  // can still add things I want to play with
}