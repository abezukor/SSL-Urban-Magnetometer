import serial
import datetime

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_TWO,
    bytesize=serial.SEVENBITS
)

ser.isOpen()

input=1
while True:
    out  = b''
    while ser.inWaiting() > 0:
        out += ser.read(1)

    if out != b'' and b':/0 89' in out:
        print(out,datetime.datetime.now().time())
