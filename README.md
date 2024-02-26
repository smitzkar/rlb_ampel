file:///Users/smitzkat/VSCode/webrepl/webrepl.html#192.168.178.107:8266
(let's see if this will work)

2024-02-25 Hm...
seems like boot.py is NOT run on boot
I HAD CHANGED THE GOD DAMN PASSWORD AND FORGOT ABOUT IT!!!!! AAAARGH!!!!!

it now connects to wifi on boot, but doesn't seem to run any code below 
I DID IT!!! 
The sta_if.connect(...) threw an error, which prevented the rest of the code from running. Put it in a try/except block and now it works! (noob mistake, but I should probably take a break/sleep)

22:07 It works

# rlb_ampel

Holy heck. This took some work.


Flash esp32 with esptool.py


somehow, these two docs need to be read together to actually get things to work  
https://docs.micropython.org/en/latest/esp32/quickref.html#webrepl-web-browser-interactive-prompt  
https://docs.micropython.org/en/latest/esp8266/tutorial/repl.html  

clone webrepl to machine and start it by opening webrepl.html  
https://github.com/micropython/webrepl?tab=readme-ov-file 

some more info: https://learn.adafruit.com/adafruit-esp32-feather-v2/micropython-setup


Important note for future me (or others reading this):
### connect to the thing via 
% picocom /dev/cu.usbserial-0289C4D4 -b115200 # (replace port according to system) -> opens python repl

### connect to wifi
>> import network
>> sta_if = network.WLAN(network.STA_IF) # sta_if for connecting with router
>> ap_if = network.WLAN(network.AP_IF)   # ap_if for using the eps32's own access point

below works for both ap and sta
check if active: 
>> sta_if.active() # sta_if.active(True) to activate

>> sta_if.connect('wifi_ssid','wifi_password')
for some reason needs to be run twice, sometimes (throws an error)

check ip adress:
>> sta_if.ifconfig()


### start the webrepl
>> import webrepl_setup # (follow instructions, set password)
>> webrepl.start() 
// WebREPL server started on http://... 

Open webrepl.html in browser, connect to ip address http://... (including the port)
enter password
success!

## to actually do stuff 

### file system  
>> import os
>> os.listdir()
>> os.mkdir('dir') 
etc.

accessing files works as usual via open(), close()
(with webrepl, one can transfer files from laptop!) -> could also use thonny IDE

"There are two files that are treated specially by the ESP8266 when it starts up: boot.py and main.py. The boot.py script is executed first (if it exists) and then once it completes the main.py script is executed. You can create these files yourself and populate them with the code that you want to run when the device starts up."

### run .py
exec(open('file.py').read())