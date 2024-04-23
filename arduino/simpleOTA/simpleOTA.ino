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
#include "TickTwo.h" // to replace delay(): https://github.com/sstaub/TickTwo

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

/*
Since I'm still struggling a bit with the FreeRTOS stuff: 
> loop() as a Task: Treat loop() as a lower priority FreeRTOS task under the operating system's management.
*/


// I'm keeping them here for easier adjustment. Could also be moved to connectToWifi.h, then change the connectToWifiAndSetupMDNS function.
// maybe use https://github.com/tzapu/WiFiManager ? (don't have to hardcode the ssid and password, can be set up via webserver. But no one can read out the code from esp32, anyway... and it's just for the open Freifunk network.) 
const char* host = "ampel";
const char* ssid = "Rlb_Ampel";
const char* password = "Rlb_Ampel<3";
const char* ntpServer = "0.pool.ntp.org";
const long  gmtOffset_sec = 3600;       // Offset for your timezone in seconds
const int   daylightOffset_sec = 3600;  // Offset for daylight saving time in seconds


// TODO: refactor this into some stuct GlobalConfig for easier handling 
// adjust these according to the actual traffic light (or let the web interface do it)
int globalPhase1Length = 24; // in seconds
int globalPhase2Length = 46;
int totalPhaseLength = globalPhase1Length + globalPhase2Length;
int globalTolerance = 3;
//MARK: TODO:
int delayFromTrafficlight = 20; // TODO: in seconds, to adjust the blinking to the actual traffic light. (20s is 96m at 18km/h)

int globalNtpUpdateInterval = 60; // in minutes (was 5 for testing, but 60 is more reasonable, maybe higher? Then needs to have the code adjusted to handle hours instead of minutes)
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

// hardcoded array of offsets per day of the week (time in seconds % 70)
// TODO: need to create a proper function to be adaptable to other phaselenghts
int weekdayOffsets[7] = {60, 40, 20, 0, 50, 30, 10}; // in seconds,(0 = Monday, 1 = Tuesday, etc.)


//MARK: Für die Testfelderöffnung
bool openingDay = false; 
int openingHour = 12;
int openingMinute = 1;


// WARNING: Do NOT use any sort of LED stuff for troubleshooting on the esp32 wired up to the Ampel!
// built-in LED on the other one is power only
// const int led = 13;     // ESP32 Pin to which onboard LED is connected
// bool ledState = false;  // ledState used to set the LED (int LOW/HIGH is same as bool false/true for digitalWrite())

WebServer server(80);


//MARK: handleServer()
// Basically the housekeeping function
// moved to separate core to ensure that even if the wifi function is stuck, the display will still update
void handleServer(void * parameter) {
  for (;;) { // Infinite loop to ensure that it runs forever -> same idea as void loop()

    if (WiFi.status() != WL_CONNECTED) {   // check if WiFi is still connected, attempt to reconnect if so
      connectToWiFiAndSetupMDNS(ssid, password, host);
    }
    server.handleClient();
    delay(1); // was thinking of replacing this with vTaskDelay(), but ESP32 already uses FreeRTOS, so it's the same thing
  }
}


// CAN'T THINK!!! rethink this when the office is silent 
//MARK: dailyReset()
// to replace syncToMinute()
// Resets the display at a specific time every day to prevent time drift from potential inaccuracies 
// int dailyReset(int resetHour = 7, int resetMinute = 0) { // only use hours%7=0
//   if (resetHour % 7 != 0) {
//     Serial.println("Please use an hour that is a multiple of 7!");
//     resetHour = 7; // default to 7
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

//   return 0;
// }


// simple function which calculates time in seconds since 00:00:00 for modulo calculations (daily drift due to 70s cycle-length)
int getCurrentTimeInSeconds() {
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  return timeinfo->tm_hour * 3600 + timeinfo->tm_min * 60 + timeinfo->tm_sec;
}


//MARK: syncToMinute()
// Checks current offset from the start of the minute. If within tolerance, returns the offset. Otherwise, delays until the start of the next minute. 
//MARK: TODO: 
// - add the delayFromTrafficlight as 2nd parameter? 
int syncToMinute(int tolerance) {
  time_t now;
  time(&now);
  struct tm* currentTime = localtime(&now);
  int seconds = currentTime->tm_sec;

  if (tolerance < seconds && seconds < (60 - tolerance)) {
    // Calculate the remaining seconds until the next minute (can't remember why I went with 59 instead of 60))
    int remainingSeconds = 59 - seconds;
    Serial.print("Syncing to minute. Starting in:");
    Serial.println(remainingSeconds);

    // Delay for the remaining time
    delay(remainingSeconds * 1000);
    Serial.println("Synced to minute");
    return 0;
  }
  // Could do this with ternary, but it's 2:55 am
  else {
    if (seconds <= 30){
      return seconds;
    }
    else {
      return 60 - seconds;
    }
  }
}

//MARK: delayUntil
// Delays until specified time, with optional buffer in seconds (don't use a buffer larger than 59 seconds. I could write some code handling this case, but this buffer is hardcoded and not accessible to the user, so dear reader... if you want a larger buffer, implement it yourself!).
//TODO: 
void delayUntil(int targetHour, int targetMinute, int secondsBuffer = 0) {
  time_t now;
  struct tm* currentTime;

  int targetSecond = 0; 

  secondsBuffer = secondsBuffer > 59 ? 59 : secondsBuffer; // Just in case someone tries to use a buffer larger than 59 seconds

  // Adjusts the target time if a buffer is specified
  if (secondsBuffer > 0) {
    targetSecond = 60 - secondsBuffer - 1; // not sure why, but it kept being off by one second, so I'm compensating for that here // doesn't actually change anything, the issue is somewhere else
    if (targetMinute == 0) {
      targetMinute = 59;
      // still getting used to ternary operator. 
      // targetHour is set to:(if targetHour is 0, set it to 23, otherwise set it to targetHour - 1)
      targetHour = targetHour == 0 ? 23 : targetHour - 1; 
    } else {
      targetMinute -= 1;
    }
  }

  do {
    time(&now);
    currentTime = localtime(&now);
    Serial.println("Waiting for target time...");
    delay(500);
  } while(!(currentTime->tm_hour == targetHour && currentTime->tm_min == targetMinute && currentTime->tm_sec == targetSecond));

  startAtSpecificTime = false; // Reset the flag, so that the loop doesn't get stuck in this function

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

  // removed a bunch of delay(2000) here, let's see if it still works
  connectToWiFiAndSetupMDNS(ssid, password, host);   // initial connection to wifi and sets alternative IP -> http://"host".local
  serverSetup(); // this one starts the web server task, which handles all the OTA stuff

  // Configure NTP and sends first request for syncing the time (actually runs asynchronously in the background, handled by FreeRTOS?)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // Wait until time is actually updated
  time_t now;
  time(&now);
  while (localtime(&now)->tm_year == 70 ) {
    time(&now);
    delay(1000); 
  }

  time(&now); 
  Serial.println(ctime(&now)); // Print the current time to the serial monitor



  // Hm... 
  // This won't run if the choice is changed during runtime. 
  // Need to do some more finagling.
  // Might not actually need to run any setup function for these?
  // switch (displayChoice) {
  //   case 1:
  //     urbanKompassSetup();
  //     break;
  //   case 2: 
  //     iterateAirlyBitmapsLoop();
  //     break;
  //   case 3: 
  //     iterateBitmapsLoop();
  //     break;
  //   case 4:
  //     // krueneWelle();
  //     Serial.println("Kruene Welle not implemented yet");
  //     delay(5000);
  //     break;
  //   default:
  //     Serial.println("Defaulting to Urban Kompass!");
  //     urbanKompassSetup();
  // } 

  // starts the two tasks/loops that are always running on specific cores
  xTaskCreatePinnedToCore(handleServer, "Handle Server", 10000, NULL, 1, NULL, 0);    // 1st Core (last parameter)
  // xTaskCreatePinnedToCore(updateDisplay, "Update Display", 10000, NULL, 1, NULL, 1);  // 2nd Core 



  //MARK: TESTING 
  // put this here to show something when it first starts (because of the delayUntil testing)
  dma_display->drawBitmap(32, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); 

}

//MARK: loop
void loop() {
  // maybe remove the updateDisplay function and just put it in here

  time_t now;
  time(&now); 

  // Update the time from the NTP server every ntpUpdateInterval minutes (currently only works for minutes and only from 0 to 60. change it to hours to update every other hour, etc.)
  // MOVE THIS INTO handleServer
  if (localtime(&now)->tm_min % globalNtpUpdateInterval == 0 && lastNtpUpdate != localtime(&now)->tm_min) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    lastNtpUpdate = localtime(&now)->tm_min;
    Serial.println(ctime(&now)); // Print the current time to the serial monitor
    // syncToMinute(globalTolerance); // Delay until just before the start of the next full minute
  }

  if (startAtSpecificTime) {
    delayUntil(startHour, startMinute); //MARK: FOR TESTING ONLY?
    Serial.println("Starting new cycle of blinking at ");
    time(&now);
    Serial.println(ctime(&now));
  }

  // Für die Testfelderöffnung 2024-04-25
  if (openingDay) {
    delayUntil(openingHour, openingMinute); //MARK: FOR TESTING ONLY?
    Serial.println("Starting new cycle of blinking at ");
    time(&now);
    Serial.println(ctime(&now));
  }

  // int timeInSeconds = 0;
  // int currentOffset = 0;
  // int drift = 0;

  // NEED TO ADD A CHECK IF CHOICE CHANGED BETWEEN LOOPS?
  // IF YES -> RUN THE SETUP FOR THE NEW CHOICE!!! 
  // edit: no longer required, because I removed the setup functions
  dma_display->clearScreen(); // tabula rasa
  switch (displayChoice) {
    case 1:

      // if (changedDisplayChoice) { // I don't remember what this was for
      //   dma_display->drawBitmap(31, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); // moved this here, maybe this is the easiest way // maybe start at 32? there is one empty row at the top that I'm happy to hide, but no more than that (ALSO CHANGE IN urbanKompass.h)
      //   changedDisplayChoice = false;
      // }


  //MARK: IMPORTANT
  // functionality to sync at any time! 
  // because we might be switching between display options, killing the loop and restarting it

      //MARK: TODO
      // // figure out the current time drift 
      // // compare the modulo to the expected one for the weekday
      // timeInSeconds = getCurrentTimeInSeconds(); 
      // currentOffset = timeInSeconds % totalPhaseLength; // 70s cycle
      // drift = weekdayOffsets[localtime(&now)->tm_wday] - currentOffset; // in seconds
      // Serial.print("Drift: ");
      // Serial.println(drift); // still need to figure out how to use it 


      dma_display->drawBitmap(31, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); // 31 seems to work perfectly! is at the very edge of the display, cutting off that one empty row from the bitmap
      urbanKompassLoop(); // I decided not to call it with parametres, it just uses global variables as set above
      break;
    case 2: 
      iterateAirlyBitmapsLoop();
      // for displays: can use 
      // void setRotation(uint8_t rotation);
      // to set the rotation of the display -> set it once to handle the current oddness!
      break;
    case 3:
      iterateBitmapsLoop();
      break;
    case 4:
      // krueneWelle();
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