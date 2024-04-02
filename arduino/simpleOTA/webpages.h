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
      window.open('/serverIndex');
    } else {
      alert('Error Password or Username');
    }
  }
</script>
)" + style;
// convert to char* so that it can be used by the WebServer
const char* loginIndex = loginIndexStr.c_str();
 
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