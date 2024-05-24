/* Temporary notes

remember to actually be on same network...

either access via IP or http://esp32.local/ (replace esp32 with whatever the host is set up as)

login for webserver in line: "if(form.userid.value=='admin' && form.pwd.value=='Rlb_Ampel<3')"
the password doesn't actually do any sort of login, it just leads to ../serverIndex page
-> should figure out a way to actually do a login, but for now it keeps out the casual user

must ALWAYS include the OTA code when uploading new sketch, because it overwrites everything, so if it's not uploaded again, there's no more OTA functionality
to upload new sketch (remember to include OTA code):
arduinoIDE > sketch > export compiled binary


*/
// existing libraries
#include <WiFi.h>
#include <WebServer.h>  // needs to be here for this part: WebServer server(80);
#include "webpages.h"   // needs quotation marks if in same directory
#include "time.h"
// #include "TickTwo.h" // to replace delay(): https://github.com/sstaub/TickTwo

// custom files
#include "connectToWifi.h"
#include "serverSetup.h"
#include "setupDisplay.h"
#include "urbanKompass.h"
#include "bitmaps.h"        // various preconfigured bitmaps (only include this once!)
#include "iterateBitmaps.h" // functions to iterate over bitmaps
#include "airlyBitmaps.h"
#include "iterateAirlyBitmaps.h"
#include "krueneWelle.h"

//MARK: CURRENTLY RUNNING ON AMPEL: 
// "Change PINs"


// I'm keeping them here for easier adjustment. Could also be moved to connectToWifi.h, then change the connectToWifiAndSetupMDNS function.
// maybe use https://github.com/tzapu/WiFiManager ? (don't have to hardcode the ssid and password, can be set up via webserver. But no one can read out the code from esp32, anyway... and it's just for the open Freifunk network.) 
const char* host = "ampel";
const char* ssid = "Rlb_Ampel";
const char* password = "Rlb_Ampel<3";
const char* ntpServer = "0.pool.ntp.org";
const long  gmtOffset_sec = 3600;       // Offset for your timezone in seconds
const int   daylightOffset_sec = 3600;  // Offset for daylight saving time in seconds
WebServer server(80);


// TODO: refactor this into some stuct GlobalConfig for easier handling 
// adjust these according to the actual traffic light (or let the web interface do it)
int globalPhase1Length = 24; // in seconds
int globalPhase2Length = 46;
int totalPhaseLength = globalPhase1Length + globalPhase2Length;
int globalTolerance = 3; // not currently used
//MARK: TODO:
int delayFromTrafficlight = 20; // TODO: in seconds, to adjust the blinking to the actual traffic light. (20s is 96m at 18km/h)

int globalNtpUpdateInterval = 6; // in hours, how often it syncs the internal clock to the NTP server
int displayChoice = 1; // 1 = urbanKompass, 2 = airly, 3 = iterateBitmaps, 4 = krueneWelle/testImages
bool changedDisplayChoice = false; // to see if the display choice was changed during runtime
bool stopDisplay = false; // used to interupt the display loop. Not currently being used
bool animationDirection = true; // false is the original top down. using a boolean to keep it simple

// maybe replace this with a proper function that accounts for the day of the week? 
bool startAtSpecificTime = false; 
int startHour = 18;
int startMinute = 52; 
int startSecond = 0;

int lastNtpUpdate = -1; // Initialised to an invalid value to force an update on the first loop iteration

// HARDCODED array of offsets per day of the week (time in seconds % 70)
// TODO: need to create a proper function to be adaptable to other phaselenghts
int weekdayOffsets[7] = {60, 40, 20, 0, 50, 30, 10}; // in seconds,(0 = Monday, 1 = Tuesday, etc.)


//MARK: handleServer()
// should rename this to houseKeeping() or such
// Basically the housekeeping function
// moved to separate core to ensure that even if the wifi function is stuck, the display will still update
void handleServer(void * parameter) {
  for (;;) { // Infinite loop to ensure that it runs forever -> same idea as void loop()

    // time keeping (to be moved to a separate function / file)
    // Update the time from the NTP server every globalNtpUpdateInterval hours
    // TODO: use ticker library
    time_t now;
    time(&now); 
    if (localtime(&now)->tm_hour % globalNtpUpdateInterval == 0 && lastNtpUpdate != localtime(&now)->tm_hour) {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      lastNtpUpdate = localtime(&now)->tm_hour;
      Serial.println("Synced to NTP at: " + String(ctime(&now))); // Print the current time to the serial monitor
    }

    // check if WiFi is still connected, attempt to reconnect if not. Not sure if this is necessary or if FreeROTS handles it, but it doesn't hurt to have it here.
    if (WiFi.status() != WL_CONNECTED) {   
      connectToWiFiAndSetupMDNS(ssid, password, host);
    }
    // all the server stuff is handled in the serverSetup() function
    server.handleClient();

    delay(1); // apparently this is good practice. Not sure, but I'll keep it for now
  }
}


// CAN'T THINK!!! redo this when the office is silent 
// -> easier to use this to just call delayUntil() ?
//MARK: dailyReset() (INACTIVE)
// to replace syncToMinute()
// Resets the display at a specific time every day to prevent time drift from potential inaccuracies 
// To return the drift for analysis and adjustment
// int dailyReset(int resetHour = 7, int resetMinute = 0) { // only use hours%7=0 to make things easy
//   if (resetHour % 7 != 0) {
//     Serial.println("Please use an hour that is a multiple of 7!");
//     resetHour = 7; // default to 7 am if the input is invalid
//   }

//   time_t now;
//   struct tm* currentTime;

//   int minute = resetMinute - ?; // just to make it easier to read

//   do {
//     time(&now);
//     currentTime = localtime(&now);
//     Serial.println("Waiting for reset time...");
//     delay(500); // half a second is good enough 
//   } while (!(currentTime->tm_hour == resetHour 
//           && currentTime->tm_min  == minute 
//           && currentTime->tm_sec  == weekdayOffsets[currentTime->tm_wday]));

//   return 0; // I want this to return the drift, but haven't implemented that yet
// }


//MARK: getCurrentTimeInSeconds()
// simple helper function which calculates time in seconds since 00:00:00 for modulo calculations (daily drift due to 70s cycle-length)
int getCurrentTimeInSeconds() {
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  return timeinfo->tm_hour * 3600 + timeinfo->tm_min * 60 + timeinfo->tm_sec;
}


//MARK: delayUntil
// Delays until specified time, currently with a buffer of 1 second because of the early start of North traffic light
// secondsBuffer isn't currently used
void delayUntil(unsigned int targetHour, unsigned int targetMinute, unsigned int targetSecond, unsigned int secondsBuffer = 0) {
  time_t now;
  struct tm* currentTime;

  targetSecond = targetSecond > 59 ? 59 : targetSecond % 60; // Just in case someone doesn't understand time

  // Adjusts the target time for the early start of north traffic light
  if (targetSecond == 0) {
    targetSecond = 60 - 1; // not sure why, but it kept being off by one second, so I'm compensating for that here 
    if (targetMinute == 0) {
      targetMinute = 59;
      // still getting used to ternary operator. 
      // targetHour is set to:(if targetHour is 0, set it to 23, otherwise set it to targetHour - 1)
      targetHour = targetHour == 0 ? 23 : targetHour - 1; 
    } else {
      targetMinute -= 1;
    }
  }

  Serial.println("Target time: " + String(targetHour) + ":" + String(targetMinute) + ":" + String(targetSecond));
  Serial.println("Waiting for target time...");

  do {
    time(&now);
    currentTime = localtime(&now);
    Serial.print(".");
    delay(500);
  // we can use targetSecond -1 because if 0 is the target, it will be set to 59 in the previous block
  } while(!(currentTime->tm_hour == targetHour && currentTime->tm_min == targetMinute && currentTime->tm_sec == targetSecond - 1));

  // Execution will continue here at the target time (or just use it to delay the loop)
}





//MARK: setup
void setup() {
  
  Serial.begin(115200);   // for output to serial monitor

   // Moved this from urbanKompass.h, because I think it only needs to run once and is required for all display options
  /************** DISPLAY **************/
  Serial.println("...Starting Display"); 
  dma_display = new MatrixPanel_I2S_DMA(mxconfig); // I think it just grabs mxconfig from the setupDisplay.h file
  dma_display->begin();
  dma_display->setBrightness8(255); //0-255

  connectToWiFiAndSetupMDNS(ssid, password, host);   // initial connection to wifi and sets alternative IP -> http://"host".local
  serverSetup(); // this one starts the web server task, which handles all the OTA stuff
  // I'm not quite sure what happens when it's not connected, but so far that wasn't an issue.

  // Configure NTP and sends first request for syncing the time (actually runs asynchronously in the background, handled by FreeRTOS?)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  time_t now;
  time(&now);
  while (localtime(&now)->tm_year == 70 ) {   // Wait until time is actually updated
    time(&now);
    delay(1000); 
  }

  time(&now); 
  Serial.println(ctime(&now)); // Print the current time to the serial monitor

  // starts the two tasks/loops that are always running on specific cores
  xTaskCreatePinnedToCore(handleServer, "Handle Server", 10000, NULL, 1, NULL, 0);    // 1st Core (last parameter)
  // xTaskCreatePinnedToCore(updateDisplay, "Update Display", 10000, NULL, 1, NULL, 1);  // 2nd Core 



  //MARK: TESTING 
  // put this here to show something when it first starts (because of the delayUntil() testing)
  dma_display->drawBitmap(32, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); 

}

//MARK: loop
void loop() {
  // maybe remove the updateDisplay function and just put it in here

  // needs to be initialised (with value!) outside of switch case
  time_t now;
  time(&now);
  // int timeInSeconds = 0;
  // int currentOffset = 0;
  // int drift = 0;

  stopDisplay = false;        // reset the stopDisplay variable 
  dma_display->clearScreen(); // tabula rasa

  switch (displayChoice) {
    case 1: // urbanKompass

      // moved this here, because it's currently only relevant for the urbanKompass display
      if (startAtSpecificTime) {
        delayUntil(startHour, startMinute, startSecond);
        Serial.println("Starting new cycle of blinking at ");
        time(&now);
        Serial.println(ctime(&now));
        startAtSpecificTime = false; // Reset the flag
      }

      //MARK: IMPORTANT
      // functionality to sync at any time! 
      // because we might be switching between display options, killing the loop and restarting it
      if (changedDisplayChoice) { 
        // sync to next available start time 
        // ...
        //MARK: TODO
        // get current time
        // round up to closest 10s 
        // compare the modulo to the expected one for the weekday
        // timeInSeconds = getCurrentTimeInSeconds(); 
        // currentOffset = timeInSeconds % totalPhaseLength; // 70s cycle
        // compare to the expected offset for the day of the week
        // figure out next available start time
        // delayUntil(nextAvailableHour, nextAvailableMinute, nextAvailableSecond);
        // // calculate drift for QA?
        // drift = weekdayOffsets[localtime(&now)->tm_wday] - currentOffset; // in seconds
        // Serial.print("Drift: ");
        // Serial.println(drift); // still need to figure out how to use it 
        changedDisplayChoice = false; // reset the flag
      }


      dma_display->drawBitmap(31, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); // 31 seems to work perfectly! is at the very edge of the display, cutting off that one empty row from the bitmap
      urbanKompassLoop(); // I decided not to call it with parametres, it just uses global variables as set above
      break;
    case 2: // airly
      iterateAirlyBitmapsLoop();
      // for displays: can use 
      // void setRotation(uint8_t rotation);
      // to set the rotation of the display -> set it once to handle the current oddness! (need to adjust all the urbanCompass stuff...)
      break;
    case 3: // iterateBitmaps
      iterateBitmapsLoop();
      break;
    case 4: // experimental
      // krueneWelle();
      // might have to move startAtSpecificTime here, too
      dma_display->drawBitmap(32, 0, NUMBERS_bits, 96, 32, dma_display->color565(255,255,255)); // to show the full display, it needs to start at 32+1 ("phantom" 1st half of 64x32 matrix) -> but index 0 is the first pixel, so it's actually 32?
      // shouldn't it be 32? pixel coordinates start 0,0 
      //dma_display->drawPixel(33, 1, dma_display->color565(0,255,255)); // direct pixel control
      fillRect(33, 5, 5, 5, dma_display->color565(0,255,0)); // from krueneWelle.h
      fillRect(39, 5, 5, 5, dma_display->color565(20,0,0)); // from krueneWelle.h
      delay(5000);
      fillRect(33, 5, 5, 5, dma_display->color565(0,20,0)); // from krueneWelle.h
      fillRect(39, 5, 5, 5, dma_display->color565(255,0,0)); // from krueneWelle.h
      delay(5000);
      break;
    default:
      Serial.println("Make a choice!");
      urbanKompassLoop(); // default
  } 
}