# -*- coding: utf-8 -*-
# Load the Pandas libraries with alias 'pd'
from __future__ import print_function
import pandas as pd
import struct
import serial
import time
import sys
import getopt
import sys
import argparse

# Read data from file 'filename.csv'
# (in the same directory that your python process is based)
# Control delimiters, rows, column names with read_csv (see later)
# Instruction to pack data
'''
timestamp   = 32bits   int
accel       = 32*3bits int
gyro        = 32*3bits int
pressure    = 32bits   int
temperature = 32bits   int
'''


class DataFeeder:
    def __init__(self, serial_port, baud_rate, file_name):
        self.SERIALPORT = serial_port
        self.BAUDRATE = baud_rate
        self.file = file_name

    def set_baud_rate(self, baudrate):
        self.BAUDRATE = baudrate

    def set_serial_port(self, port):
        self.SERIALPORT = port

    def set_csv_file(self, file):
        self.file = file

    def start(self):
        print('Trying to read file: ' + self.file + '...')
        try:
            data = pd.read_csv(self.file, usecols=[0, 1, 2, 3, 4, 5, 6, 7, 8])
        except Exception as e:
            eprint(str(e) + '\n')
            exit()

        print('CSV has been successfully loaded!')

        print('Setting Up Serial Connection: Serial Port=' + self.SERIALPORT + ', baudrate=' + str(self.BAUDRATE))
        ser = serial.Serial(self.SERIALPORT, self.BAUDRATE)
        print('Starting Up Serial Monitor.')
        try:
            ser.open()
        except Exception as e:
            eprint(str(e) + '\n')
            exit()

        if ser.isOpen():
            try:
                ser.flushInput()  # flush input buffer, discarding all its contents
                ser.flushOutput()  # flush output buffer, aborting current output
                ser.write("datafeeder --start".encode())
                reply = ser.read()
                if reply != 'STARTED':
                    print("bad reply: " + reply + '\r')
                    exit()
                time.sleep(0.5)
                number_of_ine = 0
                for index, row in data.iterrows():
                    values = row.to_list()
                    buffer = struct.pack('%si' % len(values), *values)
                    ser.write(buffer)
                    number_of_ine = number_of_ine + 1
                ser.write('--stop'.encode())
                ser.close()
            except Exception as e:
                eprint(str(e) + '\n')
        else:
            eprint("Cannot open serial port" + '\n')


def main():
    parser = argparse.ArgumentParser(
        description='DataFeeder sends fake data to the flight computer to simulate the flight.')

    parser.add_argument('-p', '--serial-port',  action="store",
                        help='Specify the USB Serial port you have connected the Flight Computer to')

    parser.add_argument('-r', '--baud-rate',    action="store",
                        help='Specify the baudrate of the current UART configurations set on the Flight Computer.')

    parser.add_argument('-f', '--csv-file',     action="store",
                        help='Specify the absolute path to the Flight Log data CSV file')

    if len(sys.argv) < 4:
        parser.print_help()
        sys.exit(0)

    opts = parser.parse_args()

    feeder = DataFeeder(opts.serial_port, opts.baud_rate, opts.csv_file)
    feeder.start()


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


if __name__ == '__main__':
    main()
