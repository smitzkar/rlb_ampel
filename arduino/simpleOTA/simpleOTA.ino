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
// #include <WiFiClient.h> // don't need this 
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "webpages.h" // needs quotation marks if in same directory
#include "connectToWifi.h"

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


  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
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