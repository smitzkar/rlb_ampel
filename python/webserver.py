from machine import Pin, Timer
import network
import socket

# Set WLAN here
ssid = "one_solutionolution"
password = "Lady_pluS_45"

# Connect to Wi-Fi
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect(ssid, password)

while not wlan.isconnected():
    sleep(1)
    print("Connecting to WiFi...")

print("IP Address: ", wlan.ifconfig()[0])

# Initialize the LED_BUILTIN pin as an output
led = Pin(2, Pin.OUT)  # 2 is typically the built-in LED for ESP32

# Define the blink functions
def blink1(timer):
    led.value(not led.value())
    timer.init(period=1000, mode=Timer.ONE_SHOT, callback=blink1)

def blink2(timer):
    led.value(not led.value())
    timer.init(period=500, mode=Timer.ONE_SHOT, callback=blink2)

def blink3(timer):
    led.value(not led.value())
    timer.init(period=250, mode=Timer.ONE_SHOT, callback=blink3)

# Initialize the timer
timer = Timer(-1)

# Start with blink1
timer.init(period=1000, mode=Timer.ONE_SHOT, callback=blink1)

# Create a socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))
s.listen(5)

while True:
    conn, addr = s.accept()
    request = conn.recv(1024)
    request = str(request)
    response = 'HTTP/1.1 200 OK\n\n'
    if '/blink1' in request:
        timer.init(period=1000, mode=Timer.ONE_SHOT, callback=blink1)
        response += 'Running blink1'
    elif '/blink2' in request:
        timer.init(period=500, mode=Timer.ONE_SHOT, callback=blink2)
        response += 'Running blink2'
    elif '/blink3' in request:
        timer.init(period=250, mode=Timer.ONE_SHOT, callback=blink3)
        response += 'Running blink3'
    else:
        response += 'Unknown command'
    conn.send(response)
    conn.close()