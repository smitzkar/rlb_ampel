# This file is executed on every boot (including wake-up boot from deepsleep)\n
# import esp
# esp.osdebug(None)

import network
import webrepl
from machine import Pin

# Connect to Wi-Fi
wifi_ssid = "Karl's phone"
wifi_password = 'Rlb_Karl<3'

sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)

try:
    # Code that might raise an exception
    sta_if.connect(wifi_ssid, wifi_password)
except OSError as e:
    # Code to run if an OSError occurs
    print('An error occurred:', e)

try:
    pin = Pin(13, Pin.OUT)
    pin.on()
    webrepl.start() # Seems to start on 'ip_address:8266'
except Exception as e:
    print('An error occurred:', e)

