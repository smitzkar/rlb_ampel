# main.py runs after boot.py finishes. It's the main program that runs on the ESP32
# Currently, it blocks the main thread, so it's not ideal. I'll have to figure out how to run it in the background

import time
from machine import Pin

# Set up the LED (already done no boot, but I'll do it again for good measure)
led = Pin(13, Pin.OUT)
 
def main():
    hello_world()


# Just a little blinky thing to show that it's working
def blink():
    for i in range(20):
        led.on()
        time.sleep(0.2)  # Wait for 1 second
        led.off()
        time.sleep(0.2)  # Wait for 1 second

    if sta_if.isconnected():
        led.on()

def hello_world():

    # "hello world" in morse: .... . .-.. .-.. --- / .-- --- .-. .-.. -..
    
    # Morse code timings
    dot_duration = 0.1
    dash_duration = 0.3
    space_letter_duration = 0.3
    space_word_duration = 0.7

    def dot():
        led.on()
        time.sleep(dot_duration)
        led.off()
        time.sleep(dot_duration)
    def dash():
        led.on()
        time.sleep(dash_duration)
        led.off()
        time.sleep(dot_duration)
    def space_letter():
        time.sleep(space_letter_duration-0.1) # -0.1 to account for the time.sleep(dot) at the end of dot() and dash()
    def space_word():
        time.sleep(space_word_duration-0.1) # same as above
    
    def h():
        dot()
        dot()
        dot()
        dot()
        space_letter()
    def e():
        dot()
        space_letter()
    def l():
        dot()
        dash()
        dot()
        dot()
        space_letter()
    def o():
        dash()
        dash()
        dash()
        space_letter()
    def w():
        dot()
        dash()
        dash()
        space_word()
    def r():
        dot()
        dash()
        dot()
        space_letter()
    def d():
        dash()
        dot()
        dot()
        space_word()
    
    
    led.off()
    h()
    e()
    l()
    l()
    o()
    space_word()
    w()
    o()
    r()
    l()
    d()
    space_word()

    led.on()


if __name__ == "__main__":
    main()
