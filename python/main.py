# main.py runs after boot.py finishes. It's the main program that runs on the ESP32
# Currently, it blocks the main thread, so it's not ideal. I'll have to figure out how to run it in the background

import time
from machine import Pin
import morse

# Set up the LED (already done no boot, but I'll do it again for good measure)
led = Pin(13, Pin.OUT)
 
def main():
    print("Running morse.py.")
    morse.hello_world()
    print("Done, waiting for input.")
    # blink()


# Just a little blinky thing to show that it's working
def blink():
    for i in range(20):
        led.on()
        time.sleep(0.2)  
        led.off()
        time.sleep(0.2) 

    if sta_if.isconnected():
        led.on()


if __name__ == "__main__":
    main()
