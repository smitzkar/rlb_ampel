import network
from machine import Pin

# Connect to Wi-Fi
wifi_ssid = 'one_solution_revolution'
wifi_password = 'Lady_pluS_45'

sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect(wifi_ssid, wifi_password)
while not sta_if.isconnected():
    pass
print("Connected to Wi-Fi")

pin = Pin(13, Pin.OUT)
pin.on()