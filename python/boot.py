# This file is executed on every boot (including wake-up boot from deepsleep)\n
# import esp
# esp.osdebug(None)

import network
import webrepl
import time
from machine import Pin

# Connect to Wi-Fi
wifi_ssid = "Karl's phone"
wifi_password = 'Rlb_Karl<3'

sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)

led = Pin(13, Pin.OUT)

# Try to connect to Wi-Fi (and start webrepl) 5 times
for i in range (5):
    print('connecting to network...')
    try:
        sta_if.connect(wifi_ssid, wifi_password)
        webrepl.start() # tried to pull it out of here, but it didn't work, so this'll have to do
        led.on()
        break
    # If it fails for some reason, blink a bit and try again
    except Exception as e:
        print('An error occurred:', e)
        for j in range(3):
            led.on()
            time.sleep(1)
            led.off()
            time.sleep(1)
        continue

