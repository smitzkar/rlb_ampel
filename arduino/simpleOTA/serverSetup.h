#include <WebServer.h>
#include <Update.h>

// the OTA webupdater
extern WebServer server;
// other gloabl variables
extern int globalPhase2Length; 
extern int globalPhase1Length; 
extern int globalTolerance; 
extern int globalNtpUpdateInterval; 
extern int displayChoice;
extern bool animationDirection;
extern bool startAtSpecificTime;
extern int startHour;
extern int startMinute;



// "import" the html files from webpages.h
extern const char* loginIndex;
extern const char* otaIndex;
extern const char* additionIndex;
extern const char* controlDisplayIndex;

void serverSetup() {



  // the login page
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });


  // the web interface for controlling the display
  server.on("/controlDisplay", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", controlDisplayIndex);
  });
  server.on("/getCurrentValues", HTTP_GET, []() {
    // Create a JSON object with the current values
    String json = "{";
    json += "\"phase1Length\":" + String(globalPhase1Length) + ",";
    json += "\"phase2Length\":" + String(globalPhase2Length) + ",";
    json += "\"ntpUpdateInterval\":" + String(globalNtpUpdateInterval) + ",";
    json += "\"tolerance\":" + String(globalTolerance) + ",";
    json += "\"displayChoice\":" + String(displayChoice) + ",";
    json += "\"animationDirection\":" + String(animationDirection);
    json += "}";

    // Send the JSON object back to the browser
    server.send(200, "application/json", json);
  });
  server.on("/updateDisplayChoice", HTTP_GET, []() {
    // Get the choice from the request
    String choice = server.arg("choice");
    
    // Update the global variable
    displayChoice = choice.toInt();
    
    // Send a response back to the browser
    server.send(200, "text/plain", "Display choice updated successfully");
  });
    server.on("/updateDirection", HTTP_GET, []() {
    // Get the choice from the request
    String direction = server.arg("direction");
    
    // Update the global variable 
    animationDirection = (direction == "true"); // this is a bit odd, but it basically checks if the string is "true" and then sets the variable to true, else false
    
    // Send a response back to the browser
    server.send(200, "text/plain", "Animation direction updated successfully");
  });
server.on("/updateParameters", HTTP_GET, []() {
  // Get the parameters from the request
  String phase1Length = server.arg("phase1Length");
  String phase2Length = server.arg("phase2Length");
  String ntpUpdateInterval = server.arg("ntpUpdateInterval");
  String tolerance = server.arg("tolerance");
  String setTime = server.arg("setTime");

  // Convert to int and store in the global variables
  globalPhase1Length = phase1Length.toInt();
  globalPhase2Length = phase2Length.toInt();
  globalNtpUpdateInterval = ntpUpdateInterval.toInt();
  globalTolerance = tolerance.toInt();

  // Handle the setTime parameter
  if (setTime == "") {
    startAtSpecificTime = false;
  } else {
    startAtSpecificTime = true;
    int colonPos = setTime.indexOf(':');
    startHour = setTime.substring(0, colonPos).toInt();
    startMinute = setTime.substring(colonPos + 1).toInt();
  }
  // Send a response back to the browser
  server.send(200, "text/plain", "Parameters updated successfully");  
});



  // the OTA webupdater
  server.on("/ota", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", otaIndex);
  });
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  // end of the OTA webupdater


  // my old addition code
  server.on("/addition", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", additionIndex);
  });
  server.on("/calculate", HTTP_GET, []() {
    // Get the numbers from the request
    String num1 = server.arg("num1");
    String num2 = server.arg("num2");
    
    // Convert to float and calculate the sum
    float number1 = num1.toFloat();
    float number2 = num2.toFloat();
    float sum = number1 + number2;
    
    // Send the sum back to the browser
    server.send(200, "text/plain", String(sum));
  });
  // end of my old addition code


  server.begin();
}