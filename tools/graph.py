"""
grap.py

Originally taken from https://gist.github.com/electronut/d5e5f68c610821e311b0
Original author: Mahesh Venkitachalam

This code has been fully rewrited to support many features. For a complete
description, see ./graph.py --help
"""

import serial
import argparse

from collections import deque

import matplotlib.pyplot as plt
import matplotlib.animation as animation


class DataPlotter(object):
    def __init__(self, **kwargs):

        self.sample_length = kwargs.get('sample_length', 10)

        self.ax = deque([0.0] * self.sample_length)

        self.ymax = kwargs.get('ymax', None)
        self.nb_axis = kwargs.get('nb_axis', 1)

        self.ay = list()
        for i in range(self.nb_axis):
            self.ay.append(deque([0.0] * self.sample_length))

        self.filter_str = kwargs.get('filter_str', None)
        self.packet_length = kwargs.get('packet_length', 1)

        self.fields = kwargs.get('fields', range(self.nb_axis))
        self.field_names = kwargs.get('field_names', list(map(str, range(self.nb_axis))))
        self.sample_index = 0
        self.ngraphs = kwargs.get('ngraphs', 1)
        self.plot_to = kwargs.get('plot_to', None)

        self.input_file = kwargs.get('input_file', None)

        self._animate = False

    def add_to_buffer(self, buf, val):
        if len(buf) < self.sample_length:
            buf.append(val)
        else:
            buf.pop()
            buf.appendleft(val)

    def add(self, data):
        self.sample_index += 1
        self.add_to_buffer(self.ax, self.sample_index)

        for i, f in enumerate(self.fields):
            self.add_to_buffer(self.ay[i], data[f])

    def update(self, frameNum):
        try:
            lines = self.input_file.readlines()
            flines = list()
            for l in lines:
                data = l.split()
                if len(data) == self.packet_length:
                    if self.filter_str:
                        if data[0] == self.filter_str:
                            flines.append(l)
                    else:
                        flines.append(l)

            self.sample_length = len(flines)

            self.ay = list()
            for i in range(self.nb_axis):
                self.ay.append(deque([0.0] * self.sample_length))

            for l in flines:
                data = l.split()
                self.add(data)

                for i, f in enumerate(self.fields):
                    self._lines[i].set_data(range(len(self.ay[i])), self.ay[i])

        except KeyboardInterrupt:
            print('exiting')

    def setup(self):

        self._fig, self._axes = plt.subplots(nrows=self.ngraphs, ncols=1, sharex=True)
        if self.ngraphs == 1:
            self._axes = list((self._axes,))

        self._lines = list()
        for i, f in enumerate(self.fields):
            if self.ngraphs > 1 and self.plot_to and f in self.plot_to and self.plot_to[f] > 0:
                line, = self._axes[self.plot_to[f]].plot([], [])
            else:
                line, = self._axes[0].plot([], [])
            if i < len(self.field_names):
                line.set_label(self.field_names[i])
            else:
                line.set_label(str(i))
            self._lines.append(line)

        if self._animate:
            # FuncAnimation must be stored
            self._animation = animation.FuncAnimation(self._fig, self.update, interval=100)
        else:
            self.update(0)

        for ax in self._axes:
            ax.set_autoscale_on(True)
            ax.relim()
            ax.margins(y=1)
            ax.autoscale_view(True, True, True)
            ax.legend()

        plt.xlim(0, self.sample_length)
        plt.xlabel('samples')

    def show(self):
        self._fig.set_tight_layout(True)
        plt.autoscale(enable=True, axis='y')
        plt.show()

    def close(self):
        if self.input_file:
            self.input_file.close()


class SerialPlotter(DataPlotter):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.ser = serial.Serial(kwargs['port'], kwargs['baud'])

        self.output_file = kwargs.get('output_file', None)

        self._buffer = b''
        self._animate = True

    def update(self, frameNum):
        try:
            rdata = self.ser.read(self.ser.in_waiting or 1)
            self._buffer += (rdata)
            lines = self._find_lines()
            for l in lines:
                data = l.split()
                if len(data) == self.packet_length:
                    if self.filter_str:
                        if data[0] == self.filter_str:
                            print('{} {}'.format(self.sample_index, data))
                            self.add(data)

                            for i, f in enumerate(self.fields):
                                self._lines[i].set_data(range(len(self.ay[i])), self.ay[i])
                    else:
                        self.add(data)

                        for i, f in enumerate(self.fields):
                                self._lines[i].set_data(range(len(self.ay[i])), self.ay[i])
        except KeyboardInterrupt:
            print('exiting')

        return self.ax,

    def _find_lines(self):
        packets = list()
        pos = self._buffer.find(b'\r\n')
        while pos > 0:
            pos = self._buffer.find(b'\r\n')
            if pos >= 0:
                packet, self._buffer = self._buffer[:pos+2], self._buffer[pos+2:]
                packets.append(packet)

        if self.output_file:
            try:
                for p in packets:
                    self.output_file.write(p.decode())
            except Exception:
                print('Unable to store data.')

        return packets

    def close(self):
        super().close()

        if self.output_file:
            self.output_file.close()

        self.ser.flush()
        self.ser.close()


# main() function
def main():
    parser = argparse.ArgumentParser(description="Graph from serial data")

    parser.add_argument('--port', '-p', help='port to listen to')
    parser.add_argument('--baud', '-b', type=int, default=115200, help='baudrate of the serial port')
    parser.add_argument('--filter', '-f', dest='filter', help='if set, only the lines beginning with FILTER will be plotted')
    parser.add_argument('--length', '-l', type=int, help='if set, only the lines with a length of serial packet')
    parser.add_argument('--fields', '-F', nargs='*', type=int, help='if set, only the specified fields will be plotted')
    parser.add_argument('--field-names', '-n', nargs='*', type=str, help='specify names for fields')
    parser.add_argument('--startup-cmd', '-S', type=str, help='command to send at startup')
    parser.add_argument('--output-file', '-o', type=argparse.FileType('w'), help='file to store data')
    parser.add_argument('--input-file', '-i', type=argparse.FileType('r'), help='file to plot. If not specified, port must be set')
    parser.add_argument('--graph-to', '-g', metavar='F:P', nargs='*', type=str, help='display data to different a different plot. Arguments are F:P were F is the field and P is the plot')

    args = parser.parse_args()

    kwargs = vars(args)

    for k in list(kwargs.keys()):
        if kwargs[k] is None:
            kwargs.pop(k)

    if 'port' in kwargs:
        kwargs['sample_length'] = 500
    elif 'input_file' in kwargs:
        pass
    else:
        raise parser.error('Either --port or --input-file must be specified')

    kwargs['packet_length'] = kwargs.pop('length', 3)

    kwargs['nb_axis'] = kwargs['packet_length']

    kwargs['filter_str'] = kwargs.pop('filter', None)
    if kwargs['filter_str']:
        kwargs['nb_axis'] -= 1
        if 'port' in kwargs:
            kwargs['filter_str'] = kwargs['filter_str'].encode()

    if 'fields' in kwargs:
        kwargs['nb_axis'] = len(kwargs['fields'])

    if 'graph_to' in kwargs:
        try:
            pt = dict()
            for gt in kwargs['graph_to']:
                field, graph = gt.split(':')
                pt[int(field)] = int(graph)

            kwargs['plot_to'] = pt
        except (IndexError, TypeError):
            parser.error('Misformated --graph-to option: %s' % kwargs['graph_to'])

        kwargs['ngraphs'] = max(pt.values()) + 1

    if 'port' in kwargs:
        print('Reading from serial port %s...' % kwargs['port'])
        plotter = SerialPlotter(**kwargs)
    else:
        print('Reading from file %s...' % kwargs['input_file'].name)
        plotter = DataPlotter(**kwargs)

    if 'startup_cmd' in kwargs:
        print('Sending startup command.')
        plotter.ser.write((kwargs['startup_cmd'] + '\r\n').encode())
        kwargs.pop('startup_cmd')

    print('Plotting data...')

    plotter.setup()
    plotter.show()      # Should block here

    plotter.close()

    print('Exiting.')


if __name__ == '__main__':
    main()
