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

//MARK: RUNNING ON AMPEL: 
// "Change PINs" (worked, but with bad timer)
// the currently active one here 2024-05-27 18:20
// seems to just constantly restart green phase, also no longer reachable via webserver
// 2024-05-30 15:12 "Fix syncedStart logical error..."

// 2024-05-30 14:46:46 to green  47:10 to red 
// should have been 14:46:40 to green  
// trying to restart over webinterface -> started at 14:50:00 -> 53400%70 = 60 (Monday, should have been 0)
// 52:20 



// I'm keeping them here for easier adjustment. Could also be moved to connectToWifi.h, then change the connectToWifiAndSetupMDNS function.
// maybe use https://github.com/tzapu/WiFiManager ? (don't have to hardcode the ssid and password, can be set up via webserver. But no one can read out the code from esp32, anyway... and it's just for the open Freifunk network.) 
//TODO: add option to automatically switch between Freifunk and Rlb_Ampel -> force hotspot if available, continuously check
const char* host = "ampel";
const char* ssid = "berlin.freifunk.net"; // "radbahn.freifunk.berlin"; // "Rlb_Ampel"; // "Freifunk" for Freifunk?
const char* password = NULL; // ""; // "Rlb_Ampel<3"; // NULL for Freifunk
// for use with Freifunk, simply omit the password

// can't access webserver at 17:30  
// no clue why. might be due to some client protection stuff on the public wifi 
// -> how to handle this?
// 1) add function to continuously check if hotspot seen, switch to it if so 
// 2) for troubleshooting, add little pixel display to see if connected to wifi

const char* ntpServer = "0.pool.ntp.org"; // change to pool.ntp.org ? Maybe smarter to let it figure out the best one itself.
const long  gmtOffset_sec = 3600;    // Offset for your timezone in seconds
int   daylightOffset_sec = 3600;     // Offset for daylight saving time in seconds (starts with DST)


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
bool forceSync = true; // start with true to make sure that it syncs urbanKompass on first run
bool stopDisplay = false; // used to interupt the display loop. Not currently being used
bool animationDirection = true; // false is the original top down. using a boolean to keep it simple

// maybe replace this with a proper function that accounts for the day of the week? 
bool startAtSpecificTime = false; 
int startHour = 18;
int startMinute = 52; 
int startSecond = 0;

WebServer server(80);

int lastNtpUpdate = -1; // Initialised to an invalid value to force an update on the first loop iteration

// HARDCODED array of offsets per day of the week (time in seconds % 70)
// TODO: need to create a proper function to be adaptable to other phaselenghts
int weekdayOffsets[7] = {60, 40, 20, 0, 50, 30, 10}; // in seconds,(0 = Monday, 1 = Tuesday, etc.), offset 0 == 70 


//MARK: houseKeeping()
// Basically the housekeeping function
// moved to separate core to ensure that even if the wifi function is stuck, the display will still update
void houseKeeping(void * parameter) {
  for (;;) { // Infinite loop to ensure that it runs forever -> same idea as void loop()

    // Update the time from the NTP server every globalNtpUpdateInterval hours 
    // TODO: use ticker library
    //TODO: use forceSync every hour to sync to internal clock? or just do it every x cycles? -> easier!
    time_t now;
    time(&now); 
    int currentHour = localtime(&now)->tm_hour;
    if (currentHour % globalNtpUpdateInterval == 0 && lastNtpUpdate != currentHour) {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      lastNtpUpdate = currentHour; //this could be an issue!
      Serial.println("Synced to NTP at: " + String(ctime(&now))); // Print the current time to the serial monitor
      forceSync = true; // forceSync in loop for urbanKompass
    }

    // Check if DST has changed and adjust the time accordingly
    // not pretty, but it works
    if (localtime(&now)->tm_mon == 3 && localtime(&now)->tm_mday == 31) {
      daylightOffset_sec = 3600; // Offset for daylight saving time in seconds
    }
    if (localtime(&now)->tm_mon == 10 && localtime(&now)->tm_mday == 27) {
      daylightOffset_sec = 0; // Offset for daylight saving time in seconds
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
  struct tm *timeCheck = localtime(&now);
  return timeCheck->tm_hour * 3600 + timeCheck->tm_min * 60 + timeCheck->tm_sec;
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

  // wait until just before the target time is reached
  do {
    time(&now);
    currentTime = localtime(&now);
    delay(100); // 1/10th of a second accuracy is good enough?
  // we can safely use targetSecond -1 because if 0 is the target, it will be set to 59 in the previous block
  } while(!(currentTime->tm_hour == targetHour && currentTime->tm_min == targetMinute && currentTime->tm_sec == targetSecond - 1));
}


//MARK: syncedStart()
// starts new cycle at proper time, adjusted for day of the week
void syncedStart(){
  time_t now;
  time(&now);
  // get current weekday
  int weekday = localtime(&now)->tm_wday;   // tm_wday is 0-6 (0 = Sunday)
  weekday = weekday == 0 ? 6 : weekday - 1; // convert to 0 = Monday
  // retrieve the offset for the current day
  int offset = weekdayOffsets[weekday];
  if (offset == 0) offset = 70; // to get to next one 
  // find next available start time
  int currentTimeInSeconds = getCurrentTimeInSeconds();
  int secondsToNextStart = offset - (currentTimeInSeconds % totalPhaseLength); 
  if (secondsToNextStart <= 0) secondsToNextStart += totalPhaseLength; // if it's already past the start time or very close to it

  int nextStart = currentTimeInSeconds + secondsToNextStart;
  int nextStartHour = nextStart / 3600;
  int nextStartMinute = (nextStart % 3600) / 60;
  int nextStartSecond = nextStart % 60;
  // delay until next start time
  delayUntil(nextStartHour, nextStartMinute, nextStartSecond);
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

  //MARK: TESTING 
  // put this here to show something when it first starts (because of the delayUntil() testing)
  dma_display->drawBitmap(32, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); 

  connectToWiFiAndSetupMDNS(ssid, password, host);   // initial connection to wifi and sets alternative IP -> http://"host".local
  serverSetup(); // this one starts the web server task, which handles all the OTA stuff
  // I'm not quite sure what happens when it's not connected, but so far that wasn't an issue.

  Serial.println("Waiting for initial NTP sync... (Can take a min or two)");

  // Configure NTP and sends first request for syncing the time (actually runs asynchronously in the background, handled by FreeRTOS?)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  time_t now;
  time(&now);
  int test = 0;
  while (localtime(&now)->tm_year == 70 ) {   // Wait until time is actually updated (this seems to be 60s by default)
    time(&now);
    Serial.println(test++);
    Serial.println(ctime(&now));
    delay(1000); 
  }

  // should probably reset the time here, now that the program knows the date
  //MARK: DST, then reset time

  time(&now); 
  Serial.println(ctime(&now)); // Print the current time to the serial monitor

  // starts the two tasks/loops that are always running on specific cores
  xTaskCreatePinnedToCore(houseKeeping, "House-Keeping", 10000, NULL, 1, NULL, 0);    // 1st Core (last parameter)
  // xTaskCreatePinnedToCore(updateDisplay, "Update Display", 10000, NULL, 1, NULL, 1);  // 2nd Core 


}

//MARK: loop
void loop() {
  // maybe remove the updateDisplay function and just put it in here

  if (changedDisplayChoice) {
    Serial.println("Display choice changed!");
  }

  // needs to be initialised (with value!) outside of switch case
  time_t now;
  time(&now);
  // int timeInSeconds = 0;
  // int currentOffset = 0;
  // int drift = 0;

  Serial.println("Starting new main loop run at:");
  Serial.println(ctime(&now)); // Print the current time to the serial monitor

  stopDisplay = false;        // reset the stopDisplay variable 
  dma_display->clearScreen(); // tabula rasa

  switch (displayChoice) {
    case 1: // urbanKompass
      // display the bike pictogram so even if waiting for the start, something is visible
      dma_display->drawBitmap(31, 0, bike_vertical_mono, 32, 32, dma_display->color565(255,255,255)); // 31 seems to work perfectly! is at the very edge of the display, cutting off that one empty row from the bitmap

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
      // also to force regular syncing (currently to internal clock only)
      if (changedDisplayChoice || forceSync) { 
        syncedStart(); // sync to next available start time

        changedDisplayChoice = false; // reset the flags
        forceSync = false;
      }
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