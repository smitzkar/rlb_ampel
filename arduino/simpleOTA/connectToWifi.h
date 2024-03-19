// connectToWiFi.h

#include <WiFi.h>
#include <ESPmDNS.h>

// note: WiFi.begin() is really smart. It's non-blocking AND terminates on its own. 
// so... no need to write a complicated function to time and handle this...
// it is possible to limit this endlessly trying to connect (WiFi.disconnect()), but this isn't some super optimised code, anyway, so might as well move on for now

void connectToWiFiAndSetupMDNS(const char* ssid, const char* password, const char* host) {
  WiFi.begin(ssid, password);
  Serial.println("Connecting...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
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

  // turn on led to indicate successful connection
  const int led = 13;
  digitalWrite(led, true);
}