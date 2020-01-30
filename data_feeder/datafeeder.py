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
import threading

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

zero = 0
END = zero.to_bytes(36, byteorder='little')


class DataFeeder:
    def __init__(self, serial_port, baud_rate, file_name):
        self.SERIALPORT = serial_port
        self.BAUDRATE = baud_rate
        self.file = file_name
        self.ser = serial.Serial(self.SERIALPORT, self.BAUDRATE)

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
        print('Starting Up Serial Monitor.')
        try:
            self.ser.close()
            self.ser.open()
        except Exception as e:
            eprint(str(e) + '\n')
            exit()

        COMM = "datafeeder ---start\r"
        if self.ser.isOpen():
            try:
                self.ser.flushInput()  # flush input buffer, discarding all its contents
                self.ser.flushOutput()  # flush output buffer, aborting current output
                time.sleep(0.5)
                number_of_ine = 0

                self.ser.write(COMM.encode())
                reply = self.ser.read(len(COMM))
                reply = self.ser.read(2)
                if reply != b'OK':
                    print("bad reply: " + str(reply) + '\n')
                    self.ser.close()
                    exit()

                print('Flight-Computer [DataFeeder] has been STARTED...')

                buffer = ''
                for index, row in data.iterrows():
                    values = row.to_list()
                    buffer = bytearray(0)
                    buffer += values[0].to_bytes(4, byteorder='little')   # timestamp
                    buffer += values[1].to_bytes(4, byteorder='little', signed=True)   # accel [1] - int32 or int16
                    buffer += values[2].to_bytes(4, byteorder='little', signed=True)   # accel [2] - int32 or int16
                    buffer += values[3].to_bytes(4, byteorder='little', signed=True)   # accel [3] - int32 or int16
                    buffer += values[4].to_bytes(4, byteorder='little', signed=True)   # gyro [1]  - int32 or int16
                    buffer += values[5].to_bytes(4, byteorder='little', signed=True)   # gyro [2]  - int32 or int16
                    buffer += values[6].to_bytes(4, byteorder='little', signed=True)   # gyro [3]  - int32 or int16
                    buffer += values[7].to_bytes(4, byteorder='little')   # pressure
                    buffer += values[8].to_bytes(4, byteorder='little')   # temperature

                    self.ser.write(buffer)
                    time.sleep(0.05)
                    number_of_ine = number_of_ine + 1

                self.ser.write(END)
                reply = self.ser.read_all()
                if b'OK' not in reply:
                    print("bad reply: " + str(reply) + '\n')
                    self.ser.close()
                    exit()

                print('Flight-Computer [DataFeeder] has been STOPPED...')
                self.ser.flushInput()  # flush input buffer, discarding all its contents
                self.ser.flushOutput()  # flush output buffer, aborting current output
                self.ser.close()
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


