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
    def __init__(self, **kwargs):
        # open serial port
        self.ser = serial.Serial(kwargs['port'], kwargs['baud'])

        self.sample_length = kwargs.get('sample_length', 10)

        self.ax = deque([0.0] * self.sample_length)

        self.ay = list()
        self.nb_axis = kwargs.get('nb_axis', 1)
        for i in range(self.nb_axis):
            self.ay.append(deque([0.0] * self.sample_length))


        self.filter_str = kwargs.get('filter_str', None)
        self.packet_length = kwargs.get('packet_length', 1)

        self.fields = kwargs.get('fields', range(self.nb_axis))
        self.sample_index = 0

        self._buffer = b''

    # add to buffer
    def addToBuf(self, buf, val):
        if len(buf) < self.sample_length:
            buf.append(val)
        else:
            buf.pop()
            buf.appendleft(val)

    # add data
    def add(self, data):
        self.sample_index += 1
        self.addToBuf(self.ax, self.sample_index)

        for i, f in enumerate(self.fields):
            self.addToBuf(self.ay[i], data[f])

    # update plot
    def update(self, frameNum, ax, ay):
        try:
            rdata = self.ser.read(self.ser.in_waiting or 1)
            self._buffer += (rdata)
            lines = self._find_lines()
            for l in lines:
                data = l.split()
                if len(data) == self.packet_length:
                    if self.filter_str:
                        check, *data = data
                        if check == self.filter_str:
                            print('{} {}'.format(self.sample_index, data))
                            self.add(data)

                            for i, f in enumerate(self.fields):
                                ay[i].set_data(range(len(self.ay[i])), self.ay[i])
                        else:
                            print('Skipped packet: {} {}'.format(check, data))
                    else:
                        self.add(data)

                        for i, a in enumerate(ay):
                            if i in self.fields:
                                a.set_data(range(len(self.ay[i])), self.ay[i])
                else:
                    print('Skipped packet with invalid length: {}'.format(data))
        except KeyboardInterrupt:
            print('exiting')

        return ax,

    def _find_lines(self):
        packets = list()
        pos = self._buffer.find(b'\r\n')
        while pos > 0:
            pos = self._buffer.find(b'\r\n')
            if pos >= 0:
                packet, self._buffer = self._buffer[:pos+2], self._buffer[pos+2:]
                packets.append(packet)

        return packets

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
    parser.add_argument('--fields', '-F', nargs='*', type=int, dest='fields')
    parser.add_argument('--field-names', '-n', nargs='*', type=str)
    parser.add_argument('--startup-cmd', '-S', type=str, dest='startup_cmd')
    parser.add_argument('--ymax', '-y', type=int, default=3000, dest='ymax')

    # parse args
    args = parser.parse_args()

    print('Reading from serial port %s...' % args.port)

    # plot parameters
    kwargs = {}

    kwargs['port'] = args.port
    kwargs['baud'] = args.baud
    kwargs['sample_length'] = 500

    if args.length:
        kwargs['packet_length'] = args.length
    else:
        kwargs['packet_length'] = 3

    kwargs['nb_axis'] = kwargs['packet_length']

    if args.filter:
        kwargs['filter_str'] = args.filter.encode()
        kwargs['nb_axis'] -= 1

    if args.fields:
        kwargs['nb_axis'] = len(args.fields)
        kwargs['fields'] = args.fields

    plotter = SerialPlotter(**kwargs)

    if args.startup_cmd:
        print('Sending startup command.')
        plotter.ser.write((args.startup_cmd + '\r\n').encode())

    print('Plotting data...')

    # set up animation
    fig = plt.figure()
    ax = plt.axes(xlim=(0, plotter.sample_length), ylim=(0, args.ymax))
    ay = list()
    for i in range(plotter.nb_axis):
        line, = ax.plot([], [])
        if args.field_names and len(args.field_names) > i:
            line.set_label(args.field_names[i])
        else:
            line.set_label(str(i))
        ay.append(line)

    anim = animation.FuncAnimation(fig, plotter.update,
            fargs=(ax, ay), interval=100)

    # show plot
    ax.legend()
    plt.show()

    # clean up
    plotter.close()

    print('Exiting.')


# call main
if __name__ == '__main__':
    main()
