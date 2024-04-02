#include <WebServer.h>
#include <Update.h>

// the OTA webupdater
extern WebServer server;

// "import" the html files from webpages.h
extern const char* loginIndex;
extern const char* serverIndex;
extern const char* addition;

void serverSetup() {
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
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
      server.send(200, "text/html", addition);
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