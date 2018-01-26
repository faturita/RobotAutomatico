import serial
import time

ser = serial.Serial(port='/dev/cu.usbmodem1411', baudrate=9600, timeout=0)


ser.write('#4 P2000\r')
ser.write('#5 P900\r')

for i in range(1,4):

    ser.write('#3 P900 T1000\r#2 P900 T1000\r')
    time.sleep(3)
    ser.write('#3 P2000 T1000\r#2 P2000 T1000\r')
    time.sleep(3)



ser.close()
