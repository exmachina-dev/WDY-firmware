"""
ldr.py
Display analog data from Arduino using Python (matplotlib)
Author: Mahesh Venkitachalam
Website: electronut.in
"""

import serial
import argparse
import numpy as np

from collections import deque

import matplotlib.pyplot as plt
import matplotlib.animation as animation


# plot class
class SerialPlotter:
    # constr
    def __init__(self, strPort, baud, nb_axis, maxLen):
        # open serial port
        self.ser = serial.Serial(strPort, baud)

        self.ax = deque([0.0]*maxLen)

        self.ay = list()
        for i in range(nb_axis):
            self.ay.append(deque([0.0]*maxLen))

        self.maxLen = maxLen

        self.filter_str = b'D'
        self.packet_length = 6
        self.ignore_fields = []

    # add to buffer
    def addToBuf(self, buf, val):
        if len(buf) < self.maxLen:
            buf.append(val)
        else:
            buf.pop()
            buf.appendleft(val)

    # add data
    def add(self, data):
        self.addToBuf(self.ax, data[1])

        for i, d in enumerate(data[1::2]):
            if i not in self.ignore_fields:
                self.addToBuf(self.ay[i], d)

    # update plot
    def update(self, frameNum, ax, ay):
        try:
            self.ser.flush()
            line = self.ser.readline()
            data = line.split()
            if len(data) == self.packet_length:
                check, *data = data
                if check == self.filter_str:
                    self.add(data)
                    #ax.set_data(range(self.maxLen), self.ax)
                    for i, a in enumerate(ay):
                        if i not in self.ignore_fields:
                            a[0].set_data(range(len(self.ay[i])), self.ay[i])
                else:
                    print('Skipped packet: {} {}'.format(check, data))
            else:
                print('Skipped packet with invalid length: {}'.format(data))
        except KeyboardInterrupt:
            print('exiting')

        return ax,

    # clean up
    def close(self):
        # close serial
        self.ser.flush()
        self.ser.close()


# main() function
def main():
    # create parser
    parser = argparse.ArgumentParser(description="Graph from serial data")
    # add expected arguments
    parser.add_argument('--port', '-p', dest='port', required=True)
    parser.add_argument('--baud', '-b', dest='baud', required=True)
    parser.add_argument('--filter', '-f', dest='filter')
    parser.add_argument('--length', '-l', type=int, dest='length')
    parser.add_argument('--ignore', '-i', nargs='*', type=int, dest='ignore')

    # parse args
    args = parser.parse_args()

    print('Reading from serial port %s...' % args.port)

    # plot parameters
    if args.length:
        plotter = SerialPlotter(args.port, args.baud, int((args.length - 1) / 2), 1000)
        plotter.packet_length = args.length
    else:
        plotter = SerialPlotter(args.port, args.baud, 1, 1000)
        plotter.packet_length = 3

    if args.filter:
        plotter.filter_str = args.filter.encode()

    if args.length:
        plotter.packet_length = args.length

    if args.ignore:
        plotter.ignore_fields = args.ignore

    print('Plotting data...')

    # set up animation
    fig = plt.figure()
    ax = plt.axes(xlim=(0, 1000), ylim=(0, 300))
    ay = list()
    for i in range(int((args.length - 1) / 2)):
        ay.append(ax.plot([], []))

    anim = animation.FuncAnimation(fig, plotter.update,
            fargs=(ax, ay), interval=10)

    # show plot
    plt.show()

    # clean up
    plotter.close()

    print('Exiting.')


# call main
if __name__ == '__main__':
    main()
