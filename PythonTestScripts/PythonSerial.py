from serial import Serial
import time

ser = Serial('/dev/ttyACM0', 115200, timeout = 1) # ttyACM1 for Arduino board

readOut = 0   #chars waiting from laser range finder

print ("Starting up")
connected = False

while True:
    try:
        if(ser.in_waiting):
            readOut = ser.read(ser.in_waiting).decode('ascii')
            print(readOut,end=" ")
    except Exception as e:
        print(e)
        pass