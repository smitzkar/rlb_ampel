// Contents: HTML code for the webpages
// a bit of a mess, using std:string to handle easy concatenation, then conversion to char* for the WebServer
// also has login and password in plain text xD (ToDO: maaybe change that before going live)

#include <string>

/* Style */
std::string style = R"(
<style>
  #file-input, input {
    width: 100%;
    height: 44px;
    border-radius: 4px;
    margin: 10px auto;
    font-size: 15px;
  }
  input {
    background: #f1f1f1;
    border: 0;
    padding: 0 15px;
  }
  body {
    background: #3498db;
    font-family: sans-serif;
    font-size: 14px;
    color: #777;
  }
  #file-input {
    padding: 0;
    border: 1px solid #ddd;
    line-height: 44px;
    text-align: left;
    display: block;
    cursor: pointer;
  }
  #bar, #prgbar {
    background-color: #f1f1f1;
    border-radius: 10px;
  }
  #bar {
    background-color: #3498db;
    width: 0%;
    height: 10px;
  }
  form {
    background: #fff;
    max-width: 258px;
    margin: 75px auto;
    padding: 30px;
    border-radius: 5px;
    text-align: center;
  }
  .btn {
    background: #3498db;
    color: #fff;
    cursor: pointer;
  }
</style>
)";

/* Login page */
std::string loginIndexStr = R"(
<form name=loginForm>
<h1>ESP32 Login</h1>
<input name=userid placeholder='User ID'> 
<input name=pwd placeholder=Password type=Password> 
<input type=submit onclick=check(this.form) class=btn value=Login>
</form>
<script>
  function check(form) {
    if (form.userid.value=='admin' && form.pwd.value=='admin') {
      window.open('/ota');
    } else {
      alert('Error Password or Username');
    }
  }
</script>
)" + style;
// convert to char* so that it can be used by the WebServer
const char* loginIndex = loginIndexStr.c_str();


/* Web Interace */
std::string controlDisplayIndexStr = R"(
  <!-- 
  the simple Webinterface

  Page to choose between display options, control the Ampel, etc. 
  (no access to underlying code here, so safe to play with)
-->

<div id='choice'>
  <h1>Choose Display Option</h1>
  <button id='btn1' onclick='updateDisplayChoice(1)'>Gruene Welle</button>
  <button id='btn2' onclick='updateDisplayChoice(2)'>Airly</button>
  <button id='btn3' onclick='updateDisplayChoice(3)'>cycleImages</button>
</div>

<div id='parameters'>
  <h2>Enter parameters for Ampel-Schaltung</h2>
  <label for='phase1Length'>Phase 1 Length (in seconds):</label>
  <input type='text' id='phase1Length' name='phase1Length'><br><br>
  <label for='phase2Length'>Phase 2 Length (in seconds):</label>
  <input type='text' id='phase2Length' name='phase2Length'><br><br>
  <label for='ntpUpdateInterval'>NTP Update Interval (in minutes):</label>
  <input type='text' id='ntpUpdateInterval' name='ntpUpdateInterval'><br><br>
  <label for='tolerance'>Tolerance (in seconds, 0-20):</label>
  <input type='text' id='tolerance' name='tolerance'><br><br>
  <button onclick='sendParameters()'>Send Parameters</button><span id='response'></span>
</div>


<style>
  .active {
    border: 2px solid green;
    background-color: lightgreen;
  }
</style>

<script>

// Function to send the display choice
function updateDisplayChoice(choice) {
  // Send a GET request to the server with the choice as a parameter
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/updateDisplayChoice?choice=' + choice, true);
  xhr.send();
  
  // Remove the 'active' class from all buttons
  document.getElementById('btn1').classList.remove('active');
  document.getElementById('btn2').classList.remove('active');
  document.getElementById('btn3').classList.remove('active');
  
  // Add the 'active' class to the clicked button
  document.getElementById('btn' + choice).classList.add('active');

  // Update the display of the 'parameters' div
  updateParametersDisplay(choice);
}

// Function to fetch the current values and pre-fill the input fields
function fetchCurrentValues() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/getCurrentValues', true);
  xhr.send();
  
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      console.log('Response from server:', this.responseText);
      // Parse the response
      var currentValues = JSON.parse(this.responseText);

      // Pre-fill the input fields
      document.getElementById('phase1Length').value = currentValues.phase1Length;
      document.getElementById('phase2Length').value = currentValues.phase2Length;
      document.getElementById('ntpUpdateInterval').value = currentValues.ntpUpdateInterval;
      document.getElementById('tolerance').value = currentValues.tolerance;

      // Remove the 'active' class from all buttons
      document.getElementById('btn1').classList.remove('active');
      document.getElementById('btn2').classList.remove('active');
      document.getElementById('btn3').classList.remove('active');

      // Add the 'active' class to the current choice button
      document.getElementById('btn' + currentValues.displayChoice).classList.add('active');

      // Update the display of the 'parameters' div
      updateParametersDisplay(currentValues.displayChoice);
    }
  };
}
// Call the function when the page loads
window.onload = fetchCurrentValues;

// Function to update the display of the 'parameters' div
function updateParametersDisplay(choice) {
  // Hide the 'parameters' div if the 'Grüne Welle' button is not active
  if (choice != 1) {
    document.getElementById('parameters').style.display = 'none';
  } else {
    // Show the 'parameters' div if the 'Grüne Welle' button is active
    document.getElementById('parameters').style.display = 'block';
  }
}


// Function to send the parameters for the Ampel-Schaltung 
function sendParameters() {
  // clear the response field
  document.getElementById('response').innerHTML = '';

  var phase1Length = document.getElementById('phase1Length').value;
  var phase2Length = document.getElementById('phase2Length').value;
  var ntpUpdateInterval = document.getElementById('ntpUpdateInterval').value;
  var tolerance = document.getElementById('tolerance').value;
  
  // Send a GET request to the ESP32
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/updateParameters?phase1Length=' + phase1Length + '&phase2Length=' + phase2Length + '&ntpUpdateInterval=' + ntpUpdateInterval + '&tolerance=' + tolerance, true);
  xhr.send();
  
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      // Handle the response from the ESP32
      console.log(this.responseText);
      document.getElementById('response').innerHTML = this.responseText; 
    }
  };
}
</script>
)" + style;
// convert to char* so that it can be used by the WebServer
const char* controlDisplayIndex = controlDisplayIndexStr.c_str();


 
/* OTA update Page */
std::string otaIndexStr = R"(
<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>
  <input type='file' name='update' id='file' onchange='sub(this)' style=display:none>
  <label id='file-input' for='file'>   Choose file...</label>
  <input type='submit' class=btn value='Update'>
  <br><br>
  <div id='prg'></div>
  <br><div id='prgbar'><div id='bar'></div></div><br>
</form>
<script>
  function sub(obj){
    var fileName = obj.value.split('\\\\');
    document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];
  };
  $('form').submit(function(e){
    e.preventDefault();
    var form = $('#upload_form')[0];
    var data = new FormData(form);
    $.ajax({
      url: '/update',
      type: 'POST',
      data: data,
      contentType: false,
      processData:false,
      xhr: function() {
        var xhr = new window.XMLHttpRequest();
        xhr.upload.addEventListener('progress', function(evt) {
          if (evt.lengthComputable) {
            var per = evt.loaded / evt.total;
            $('#prg').html('progress: ' + Math.round(per*100) + '%');
            $('#bar').css('width',Math.round(per*100) + '%');
          }
        }, false);
        return xhr;
      },
      success:function(d, s) {
        $('#prg').html('Success!');
        $('#bar').css('width', '100%');
        console.log('success!') 
      },
      error: function (a, b, c) {
      }
    });
  });
</script>)" + style;
// convert to char* so that it can be used by the WebServer
const char* otaIndex = otaIndexStr.c_str();




// my old addition code
std::string additionIndexStr = R"(
<!-- Adjusted HTML and JavaScript -->
<label for='num1'>Number 1:</label>
<input type='text' id='num1' name='num1'><br><br>
<label for='num2'>Number 2:</label>
<input type='text' id='num2' name='num2'><br><br>
<button onclick='sendNumbers()'>Send Numbers</button>
<p>Sum: <span id='sum'>0</span></p>

<script>
function sendNumbers() {
  var num1 = document.getElementById('num1').value;
  var num2 = document.getElementById('num2').value;
  
  // Send a GET request to the ESP32
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/calculate?num1=' + num1 + '&num2=' + num2, true);
  xhr.send();
  
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      // Display the sum received from the ESP32
      document.getElementById('sum').innerText = this.responseText;
    }
  };
}
</script>)" + style;
// convert to char* so that it can be used by the WebServer
const char* additionIndex = additionIndexStr.c_str();