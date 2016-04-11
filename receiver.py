import time
import serial
import subprocess

class PortReader:

    def __init__(self, portStr):
        """
        Initialize the serial port configurations
        """
        self.ser = serial.Serial(port = portStr,
        # port number, subject to change
            baudrate = 38400,
        #   parity = None,
        #   stopbits = serial.STOPBITS_TWO,
            bytesize = serial.EIGHTBITS
        )

    def openPort(self):
        """
        Open the port
        """
        if not self.ser.isOpen():
            self.ser.open()

    def readBytes(self, numOfBytes=20):
        bytearr = bytearray(numOfBytes)
        cur = 0
        while cur < numOfBytes:
            read = self.ser.read(1)
            if len(read) == 1:
                bytearr[cur] = read[0]
                cur += 1
        return bytearr


    def readPacket(self):
        packet = self.readBytes()
        if packet is None:
            return ""
        else:
            return packet

    def test(self):
        while 1:
            print self.ser.inWaiting()

    def sendData2Plot(self, data2Send, deviceNum):
        """
        This function should call inline C program to use
        heatshrink to decompress the data and send for plot.
        """
        process = subprocess.Popen("decoder", stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=True)
        inpt, output = process.stdin, process.stdout
        inpt.write(data2Send + ' ' + str(len(data2Send)))
        print(output.read())
        inpt.close()
        output.close()
        print "Received complete data " + data2Send, deviceNum

    def run(self, runTime=float('inf'), debug=False):
        """
        Start running the data collection program. The
        data packets are coming in with 20 bytes data each,
        and delimiter packets are 20 bytes with string
        'TKENDTKENDTKENDTKdxx', where 'd' denotes the device
        number and 'xx' is the number of bytes received in
        previous packets, since packets can only come in
        bytes multiple of 20's.
        """
        start = time.time()
        bufferedData = ''
        while 1:
            checkStamp = time.time()
            if (checkStamp - start) > runTime:
                return
            packet = self.readPacket()
            if debug == True:
                print packet + "PACKET"
            if 'TKENDTKENDTKEND' in packet:
                numBytes = 20 #int(packet[-4:-1])
                self.sendData2Plot(bufferedData[:numBytes], 0)#packet[-5])
                bufferedData = ''
            else:
                bufferedData += packet

portReader = PortReader('/dev/tty.usbmodem1411')    # '\\.\COM6'
portReader.openPort()

portReader.run(debug=True)

