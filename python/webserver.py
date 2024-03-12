from machine import Pin, Timer
import socket

led = Pin(13, Pin.OUT)  

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
timer = Timer(-1) # -1 is for virtual timer, can set to 0, 1, 2, or 3 for hardware timers

# Start with blink1
timer.init(period=1000, mode=Timer.ONE_SHOT, callback=blink1)

# Create a socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))
s.listen(5)


# To actually use this, send "curl http://192.168.43.185/blink2" from the command line
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
    elif '/stop' in request:
        response += 'Stopping server'
        timer.deinit() # Stop the timer (otherwise it just keeps going? could I use the timer thingy then to run the display?!)
        conn.send(response)
        conn.close()
        break
    else:
        response += 'Unknown command'
    conn.send(response)
    conn.close()