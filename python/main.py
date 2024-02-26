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
    factor = 2 # to make it easier to change the timings
    dot_duration = 0.1 * factor
    dash_duration = 0.3 * factor
    space_letter_duration = 0.3 * factor
    space_word_duration = 0.7 * factor

    def signal(length):
        led.on()
        time.sleep(length)
        led.off()
        time.sleep(dot_duration) # as per the standard, the space between signals is the length of a dot

    # Turn off LED to properly show first signal
    led.off()
    time.sleep(1) 
    
    # Morse from online generator, had to remove space around "/"
    message = ".... . .-.. .-.. ---/.-- --- .-. .-.. -.." # "hello word"
    for char in message:
        if char == '.':
            signal(dot_duration)
        elif char == '-':
            signal(dash_duration)
        elif char == ' ':
            time.sleep(space_letter_duration - dot_duration) # account for the dot duration already being part of the end of the signal
        elif char == '/':
            time.sleep(space_word_duration - dot_duration)
        else:
            print("Invalid character in morse code string: " + char)

    led.on()


if __name__ == "__main__":
    main()
