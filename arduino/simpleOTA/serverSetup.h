#include <WebServer.h>
#include <Update.h>

// the OTA webupdater
extern WebServer server;
extern const char* loginIndex;
extern const char* serverIndex;
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
      String html = R"rawliteral(
      <!DOCTYPE html>
      <html>
      <body>
        <!-- Adjusted HTML and JavaScript -->
        <label for="num1">Number 1:</label>
        <input type="text" id="num1" name="num1"><br><br>
        <label for="num2">Number 2:</label>
        <input type="text" id="num2" name="num2"><br><br>
        <button onclick="sendNumbers()">Send Numbers</button>
        <p>Sum: <span id="sum">0</span></p>

        <script>
        function sendNumbers() {
          var num1 = document.getElementById("num1").value;
          var num2 = document.getElementById("num2").value;
          
          // Send a GET request to the ESP32
          var xhr = new XMLHttpRequest();
          xhr.open("GET", "/calculate?num1=" + num1 + "&num2=" + num2, true);
          xhr.send();
          
          xhr.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
              // Display the sum received from the ESP32
              document.getElementById("sum").innerText = this.responseText;
            }
          };
        }
        </script>
      </body>
      </html>
      )rawliteral";

      server.send(200, "text/html", html);
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

    // led control code 
    // DO NOT USE THIS WITH THE LED MATRIX! -> half will be blue
    // server.on("/ledOn", HTTP_GET, []() {
    //   digitalWrite(LED_BUILTIN, true); // Turn the LED on
    //   server.send(200, "text/plain", "LED is ON");
    //   Serial.println("LED turned ON");
    // });
    // server.on("/ledOff", HTTP_GET, []() {
    //   digitalWrite(LED_BUILTIN, false); // Turn the LED off
    //   server.send(200, "text/plain", "LED is OFF");
    //   Serial.println("LED turned OFF");
    // });


  server.begin();
}