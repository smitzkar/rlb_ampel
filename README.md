# NOTES FOR SMITZKAR 
(not a proper README, yet)


## Update 2024-03-12  

I guess I have to go back to Arduino, after all. The esp32 I have isn't supported by the RGBMatrix library. 

There's OTA stuff for Arduino, so while I won't have the usability and flexibility of python and the webREPL, I'll be able to update the code without having to touch the Ampel. 
https://lastminuteengineers.com/esp32-ota-web-updater-arduino-ide/ 



## 2024-03-15 update I sent to Mh, Nh

Hey ihr beiden,

Ich hatte am Dienstag mein Ampel-Zoom-Meeting mit Sara und hier ist ein kleines Update/Ergebnis.

Sie hatte versprochen/erwähnt/in Aussicht gestellt, dass sie uns einen Haufen Elektronik-Teile bringen würde, damit ich damit herumspielen und lernen kann, ohne direkt an der Ampel zu hantieren. Daraus wird jetzt leider nichts, weil es wohl im Labor der Uni zu Personalwechsel kam und sie jetzt auch schon wieder in Barcelona ist.
Wir sind was Elektronik angeht also auf die Ampel selbst, einen extra Microcontroller und die Kamera beschränkt. Die Kamera ist damit erst einmal vom Tisch (die braucht extra Kabel und Krams), aber das war denke ich sowieso abzusehen.

Auf dem 2. Microcontroller (im Prinzip ein extremst simpler Computer) habe ich in meiner Freizeit herumgespielt und hatte schon einen Plan, den ich jetzt aber verwerfen musste. Mein Ziel dabei war es die Ampel über eine komplett andere Sprache zu kontrollieren, mit der wir viel schneller neue (und mächtigere) Programme, Veränderungen, etc. aufspielen hätten können. Das ist ohne extra-Equipment mit dem ich zuhause experimentieren kann leider nicht realistisch.
Ich habe Sara eine alternative Idee vorgestellt (eine Art alternatives Betriebssystem, welche aber die gleiche Sprache benutzt) und sie schien sehr optimistisch, dass meine Lösung funktionieren wird und hat mir ihre Hilfe angeboten, sollte ich irgendwo stecken bleiben. Ich habe es auf dem extra Microcontroller schon getestet und es funktioniert!

Sie hat noch ein paar häufige Fehlerquellen und bekannte Eigenarten der Elektronik erklärt und ein paar Tipps gegeben, wie man damit am besten umgeht. Es scheint sehr viel trial and error zu sein, aber das bin ich gewohnt.

Zuletzt haben wir dann noch zusammen überlegt, wie wir die Ampel am besten mit Freifunk verbinden (besonders bzgl. Sicherheit, damit da keiner Hakenkreuze drauf spielt). Das scheint relativ einfach zu sein.

---

Es wird also leider etwas schwieriger als erwartet, ich bin aber zuversichtlich, dass ich es trotzdem zum Laufen bringe.

Einen wirklichen Zeitplan habe ich noch nicht, weil ich mich erst einmal auf den Audioguide fokussieren möchte.
Offene Aufgaben:
[ ] neues "Betriebssystem" auf Ampel-Microcontroller spielen (damit wir sie via wifi programmieren können)
[ ] wifi und minimalen webserver einrichten (um zwischen verschiedenen Anzeigevarianten via smartphone zu wechseln oder das Ampel-timing manuell/automatisch zu resetten)
[ ] ein paar Sensoren-Anzeigevarianten schreiben (fürs Erste: fake it!-> Lautstärke, Luftqualität, etc., ohne wirklichen Input von möglichen Sensoren, einfach statische Anzeige um zu zeigen, wie es aussehen könnte)
[ ] sobald das mit Airly (oder etwas anderem fest steht), die Anzeige realisieren
[ ] Ampel timing messen (wollte da jemand mal bei der Stadt fragen? Ich könnte auch mal schauen, von wo ich da Infos kriegen könnte.)
[ ] die alternative Ampel-Anzeige schreiben
[ ] Varianten vorstellen, feedback bekommen, anpassen

Es wirkt erstmal wie sehr viel (und ist es auch), Vieles davon ist aber sehr spannend und ich bin motiviert es fertigzustellen. Falls es zeitlich doch zu knapp wird, kann ich etwas pragmatischer werden und mich auf die wesentlichen Dinge beschränken. Durch mein neues "Betriebsystem" sollten wir in der Lage sein an der Ampel auch nach dem Aufhängen zu arbeiten, bzw. die Anzeige anzupassen.


So viel erstmal von mir.


PS: Sara meinte, dass es sein könnte, dass das derzeitige Netzteil überfordert werden könnte (es ist wohl eigentlich etwas zu schwach für die zwei LED-Matrizen). Da bräuchten wir unter Umständen ein etwas Mächtigeres. Das sollte aber nicht viel kosten und ich werde erst einmal schauen, ob ich es mit dem Alten schaffe ohne die Anzeige zu sehr zu beschränken. Falls wir doch noch irgendwas mit der Kamera machen wollen, brauchen wir definitiv ein bisschen Equipment (Kabel, etc.). Eventuell könnte auch was an der Ampel-Verkabelung kaputt gehen, weil die Kabel dort auch relativ lavede sind. Da bewegen wir uns aber auch im ein- bis niedrigen zweistelligen Eurobereich.




## Python Experimenting


file:///Users/smitzkat/VSCode/webrepl/webrepl.html#192.168.178.107:8266
(let's see if this will work)

2024-02-25 Hm...
seems like boot.py is NOT run on boot
I HAD CHANGED THE GOD DAMN PASSWORD AND FORGOT ABOUT IT!!!!! AAAARGH!!!!!

it now connects to wifi on boot, but doesn't seem to run any code below 
I DID IT!!! 
The sta_if.connect(...) threw an error, which prevented the rest of the code from running. Put it in a try/except block and now it works! (noob mistake, but I should probably take a break/sleep)

22:07 It works

2024-02-26 
messed around with led to display morse code "hello world"
learned quite a bit on how to work with the thing, by doing this little exercise
I think I'm comfortable with it, now
next task: async O.o

2024-02-07
webserver.py is running 
to switch between 3 blink programs, run: 
% curl http://192.168.43.185/blink2 

TIMER DOESN'T BLOCK THE REPL!!
from machine import Timer

def print_message(timer):
    print("One second has passed")

timer = Timer(-1)
timer.init(period=1000, mode=Timer.PERIODIC, callback=print_message)

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