# Serial Loopback. Used for testing serial communication.

import serial

def startSerial(ser, port):
	ser.baudrate = 9600
	ser.port = port
	ser.timeout = 20
	ser.open()
	ser.flushInput()
	print "Serial Settings: " + str(ser)
	print "Prepared To Start Communication"

ser = serial.Serial()
startSerial(ser, "COM4")

ser.write("This is a test of the endurance of the serial communication!\n")

while(True):
	if(ser.inWaiting() > 0):
		temp = ser.readline()
		print temp
		ser.write(temp)