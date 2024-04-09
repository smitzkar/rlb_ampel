/*
A simple set of utilities to support time based tasks for the ESP32.

At this state: Requires the time library and functioning wifi connection.
*/


#include <WiFi.h>
#include "time.h"

// 
void timerKsSetup(int lastNtpUpdate, int ntpUpdateInterval){

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


};
