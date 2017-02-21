"""
ldr.py
Display analog data from Arduino using Python (matplotlib)
Author: Mahesh Venkitachalam
Website: electronut.in
"""

import argparse
import numpy as np
import math

import matplotlib.pyplot as plt
import matplotlib.animation as animation


_S_THK = 1.1
_D_DIA = 120
WDY_MAX_POSITION = 30000
WDY_STRAP_DEAD_OFFSET = 0
WDY_DRUM_OFFSET_TURNS = 0
_PI = 3.1415


def spiral_length_to_turns(length):
    Nturns = (_S_THK - _D_DIA + math.sqrt(
        pow(_D_DIA - _S_THK, 2) + ((4 * _S_THK * (
            (WDY_MAX_POSITION - length + WDY_STRAP_DEAD_OFFSET))) / _PI))) / \
        (2 * _S_THK)

    return Nturns


def spiral_length_to_diameter(length):
    Ddiameter = 2 * (spiral_length_to_turns(length) + WDY_DRUM_OFFSET_TURNS) \
        * _S_THK + _D_DIA

    return Ddiameter


# main() function
def main():
    # create parser
    parser = argparse.ArgumentParser(description="Graph from serial data")

    parser.add_argument('--max', '-M', type=int, default=30000)
    parser.add_argument('--min', '-m', type=int, default=0)
    parser.add_argument('--step', '-s', type=int, default=1)

    # parse args
    args = parser.parse_args()

    # plot parameters
    data1 = []
    data2 = []
    for i in range(args.min, args.max, args.step):
        data1.append(spiral_length_to_turns(i))
        data2.append(spiral_length_to_diameter(i))


    print('Plotting data...')

    # set up animation
    ax = plt.axes(xlim=(0, args.max), ylim=(0, data2[0] + 10))
    ax.plot(data1, label='Length to turns')
    ax.plot(data2, label='Length to diameter')

    # show plot
    ax.legend()
    plt.show()

    print('Exiting.')


# call main
if __name__ == '__main__':
    main()
