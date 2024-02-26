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

for i in range(5):
    if not sta_if.isconnected():
        print('connecting to network...')
        sta_if.connect(wifi_ssid, wifi_password)
        time.sleep(5)
    else:
        try:
            pin = Pin(13, Pin.OUT)
            pin.on()
            webrepl.start() # Seems to start on 'ip_address:8266'
        except Exception as e:
            print('An error occurred:', e)
        break
