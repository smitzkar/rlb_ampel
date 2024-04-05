/* Temporary notes

to actually start after upload, press the RESET button on esp32
remember to actually be on same network...

either access via IP or http://esp32.local/ (replace esp32 with whatever the host is set up as)

login for webserver in line: "if(form.userid.value=='admin' && form.pwd.value=='Rlb_Ampel<3')"
the password doesn't actually do any sort of login, it just leads to ../serverIndex page

must ALWAYS include the OTA code when uploading new sketch, because it overwrites everything, so if it's not uploaded again, there's no more OTA functionality
to upload new sketch (remember to include OTA code):
arduinoIDE > sketch > export compiled binary

according to this https://www.reddit.com/r/esp32/comments/poogbr/better_explanation_for_vtaskdelay_freertos/ delay() on the esp32 actually wraps vTaskDelay. So... is nonblocking by default. 

*/

#include <WiFi.h>
#include <WebServer.h>  // needs to be here for this part: WebServer server(80);
#include "webpages.h"   // needs quotation marks if in same directory
#include "connectToWifi.h"
#include "serverSetup.h"
#include "setupDisplay.h"
#include "urbanKompass.h"
#include "bitmaps.h"        // various preconfigured bitmaps (only include this once!)
#include "iterateBitmaps.h" // functions to iterate over bitmaps

/*
Since I'm still struggling a bit with the FreeRTOS stuff: 
> loop() as a Task: Treat loop() as a lower priority FreeRTOS task under the operating system's management.
*/


// I'm keeping them here for easier adjustment. Could also be moved to connectToWifi.h, then change the connectToWifiAndSetupMDNS function.
// maybe use https://github.com/tzapu/WiFiManager ? (don't have to hardcode the ssid and password, can be set up via webserver. But no one can read out the code from esp32, anyway... and it's just for the open Freifunk network.) 
const char* host = "esp32";
const char* ssid = "Karl";
const char* password = "Rlb_KsESP";

bool stopDisplay = false; // used to interupt the display loop 


// adjust these according to the actual traffic light (or let the web interface do it)
int globalPhase1Length = 20; // in seconds
int globalPhase2Length = 50;
int globalTolerance = 3;
int globalNtpUpdateInterval = 5; // in minutes
int displayChoice = 1; // 1 = urbanKompass, 2 = iterateBitmaps, 
bool animationDirection = false; // default is the original top down

// WARNING: Do NOT use any sort of LED stuff for troubleshooting on the esp32 wired up to the Ampel!
// built-in LED on the other one is power only
// const int led = 13;     // ESP32 Pin to which onboard LED is connected
// bool ledState = false;  // ledState used to set the LED (int LOW/HIGH is same as bool false/true for digitalWrite())

WebServer server(80);



// Basically the housekeeping function
// moved to separate core to ensure that even if the wifi function is stuck, the display will still update
void handleServer(void * parameter) {
  for (;;) { // Infinite loop to ensure that it runs forever -> same idea as void loop()

    if (WiFi.status() != WL_CONNECTED) {   // check if WiFi is still connected, attempt to reconnect if so
      // digitalWrite(led, false);
      connectToWiFiAndSetupMDNS(ssid, password, host);
    }
    server.handleClient();
    delay(1); // was thinking of replacing this with vTaskDelay(), but ESP32 already uses FreeRTOS, so it's the same thing
  }
}


void setup() {
  
  Serial.begin(115200);   // for output to serial monitor

  delay(2000); // added a bunch of delays to hopefully fix the display not starting properly

  connectToWiFiAndSetupMDNS(ssid, password, host);   // initial connection to wifi and sets alternative IP -> http://"host".local

  delay(2000); 

  serverSetup();          // this one starts the web server task, which handles all the OTA stuff

  delay(2000); 



  // Hm... 
  // This won't run if the choice is changed during runtime. 
  // Need to do some more finagling.
  switch (displayChoice) {
    case 1:
      urbanKompassSetup();
      break;
    // case 2: 
    //   iterateBitmapsSetup();
    //   break;
    default:
      Serial.println("Make a choice!");
  } 

  delay(2000); 

  // starts the two tasks/loops that are always running on specific cores
  xTaskCreatePinnedToCore(handleServer, "Handle Server", 10000, NULL, 1, NULL, 0);    // 1st Core (last parameter)
  // xTaskCreatePinnedToCore(updateDisplay, "Update Display", 10000, NULL, 1, NULL, 1);  // 2nd Core 

}


void loop() {
  // can put stuff in here 
  // maybe remove the updateDisplay function and just put it in here

  Serial.println(displayChoice);

  // NEED TO ADD A CHECK IF CHOICE CHANGED BETWEEN LOOPS
  // IF YES -> RUN THE SETUP FOR THE NEW CHOICE!!! 
  switch (displayChoice) {
    case 1:
      urbanKompassLoop(); // I decided not to call it with parametres, it uses global variables as set above
      break;
    case 2: 
      // airly();
      Serial.println("Airly not implemented yet");
      delay(5000); // just for the serial print
      break;
    case 3:
      // iterateBitmapsLoop();
      Serial.println("cycleImages not implemented yet");
      delay(5000);
      break;
    case 4:
      // krueneWelle();
      Serial.println("Kruene Welle not implemented yet");
      delay(5000);
      break;
    default:
      Serial.println("Make a choice!");
  } 
}