# Description: Morse code "hello world" on the onboard LED
# Currently, it works by displaying an already morse-ified string. Can easily generate those online. Would also be easy (if a bit tedious) to write the letters and such in here. Might do that later, to be able to try turning requests message into morse, or some such. Maybe check out sys args?

import time
from machine import Pin

# Set up the LED (already done no boot, but I'll do it again for good measure)
# I wonder if this messes with anything
led = Pin(13, Pin.OUT)

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
    
    # Morse from online generator, had to remove spaces around "/". Alternatively, adjust the space_word_duration to just dot_duration: 0.3+0.1+0.3 = 0.7
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