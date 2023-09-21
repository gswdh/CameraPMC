import serial

ser = serial.Serial("/dev/tty.usbserial-D30H6TGJ", baudrate=921600)

while True:
    data = ser.read().decode()
    if data:
        print(data, end="")
