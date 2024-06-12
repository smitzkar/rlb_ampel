// connectToWiFi.h

// TODO: for actual use 
// figure out a way to limit WiFI.begin's runtime. Maybe let it try for a min, then wait 10min. 
// It would needlessly block and eat up cpu time, when really, the wifi stuff isn't as critical as the display.

#include <WiFi.h>
#include <ESPmDNS.h>

void connectToWiFiAndSetupMDNS(const char* ssid, const char* password, const char* host, bool initialConnection = false) {
  if (password == NULL) {
    WiFi.begin(ssid);
  } else {
    WiFi.begin(ssid, password);
  }
  Serial.println("Connecting...");

  int counter = 0;
  if (initialConnection){
    counter = 31; // if it's the first setup, it NEEDS to connect, so will try until it does
  }

  while (WiFi.status() != WL_CONNECTED && counter != 20) { // lets this only run 10s at a time
    delay(500);
    Serial.print(".");
    counter++;
  }

  if (counter == 20) {
    Serial.println("Connection attempt failed.");
    return;
  }

  // turn on led to indicate successful connection (NOT FOR THE LED MATRIX ESP32!)
  // const int led = 13;
  // digitalWrite(led, true);

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin(host)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started: http//" + String(host) + ".local");

}