// Simple sketch to test web connectivity and control. 
//
// 1) Set wlan settings below libraries.
// 2) Upload, then check serial monitor. If it shows IP, it works. If not, try reuploading. 
// 3) Type IPAddress into browser -> should display "Hello from ESP32!"
// 4) To control LED, change URL to */led/on or */led/off
//
// TODO: I believe that to use it from outside the network (control from home or office), I would have to set up VPN. 

// from https://lastminuteengineers.com/esp32-ota-web-updater-arduino-ide/


#include <WiFi.h>
#include <WebServer.h>

// Set WLAN here
const char* ssid = "Karl's phone";
const char* password = "Rlb_Karl<3";

WebServer server(80); // Object for WebServer(HTTP port, 80 is default)


// Variables to store the numbers and their sum
float numbers[2]; // Array to store two numbers
float sum = 0;    // Variable to store the sum


// Simple example on how to host a webpage with javascript to sum two numbers
void handleRoot() {
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
}

void handleCalculate() {
  // Get the numbers from the request
  String num1 = server.arg("num1");
  String num2 = server.arg("num2");
  
  // Convert to float and calculate the sum
  numbers[0] = num1.toFloat();
  numbers[1] = num2.toFloat();
  sum = numbers[0] + numbers[1];
  
  // Print to Serial Monitor
  Serial.print("Number 1: ");
  Serial.print(numbers[0]);
  Serial.print(", Number 2: ");
  Serial.print(numbers[1]);
  Serial.print(", Sum: ");
  Serial.println(sum);
  
  // Send the sum back to the browser
  server.send(200, "text/plain", String(sum));
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Calls handleRoot function when webserver is called on root -> ip address without anything past /
  server.on("/", handleRoot);

  server.on("/calculate", handleCalculate);

  // Old controls for LED
  /*
  server.on("/", HTTP_GET, []() {
     server.send(200, "text/plain", "Hello from ESP32!");
  });

  server.on("/led/on", HTTP_GET, []() {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
    server.send(200, "text/plain", "LED is ON");
  });

  server.on("/led/off", HTTP_GET, []() {
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
    server.send(200, "text/plain", "LED is OFF");
  });
  */

  // Start server
  server.begin();
}

void loop() {
  server.handleClient();
}
