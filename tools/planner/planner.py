# -*- coding: utf8 -*-

"""
planner.py

Author: Benoit Rapidel

Algorithm taken from [Block.cpp on Smoothieware project](https://github.com/Smoothieware/Smoothieware/blob/edge/src/modules/robot/Block.cpp)

Plan a trapezoidal move and output the move profile.
"""

import time
import argparse

from math import sqrt, floor


class Move(object):
    """
    Store and compute a move.
    """

    KEYS = ('position', 'current_position', 'speed', 'current_velocity', 'acceleration', 'deceleration')
    FORWARD = 1
    BACKWARD = -1
    FREQUENCY = 100     #Hz

    def __init__(self, **kwargs):
        try:
            for key in self.KEYS:
                self.__dict__[key] = kwargs[key]
        except KeyError as e:
            print('Missing required argument: {}'.format(e))

        self.vel0_time = 0
        self.acceleration_time = 0
        self.deceleration_time = 0
        self.plateau_time = 0
        self.total_move_time = 0
        self.vel0_until = 0
        self.accelerate_until = 0
        self.decelerate_after = 0

        self.maximum_speed = 0

        self.plan()

    def plan(self):
        initial_speed = abs(self.current_velocity)
        initial_velocity = self.current_velocity * self.direction

        if self.distance == 0:
            return

        # How many steps ( can be fractions of steps, we need very precise values ) to accelerate and decelerate
        # This is a simplification to get rid of rate_delta and get the steps/s² accel directly from the mm/s² accel
        # float acceleration_per_second = (this->acceleration * this->steps_event_count) / this->millimeters
        # self.acceleration = (self.acceleration * self.position) / self.distance
        # self.deceleration = (self.deceleration * self.position) / self.distance

        decel_dist_to_decel0 = 0
        time_to_decel0 = 0
        if initial_velocity < 0 or initial_velocity > self.speed:
            time_to_decel0 = -initial_speed / -self.deceleration
            decel_dist_to_decel0 = (initial_speed * time_to_decel0) + \
                    ((self.deceleration * pow(time_to_decel0, 2)) / 2)

        distance_from_decel0 = self.distance - decel_dist_to_decel0

        if distance_from_decel0 < 0:
            print('Impossible movement')


        maximum_possible_speed = sqrt(
                (self.deceleration * pow(initial_velocity, 2) +
                    2 * self.acceleration * self.distance * self.deceleration) /
                (self.acceleration + self.deceleration))

        # Now this is the maximum rate we'll achieve this move, either because
        # it's the higher we can achieve, or because it's the higher we are
        # allowed to achieve
        self.maximum_speed = min(maximum_possible_speed, self.speed)

        # Now figure out how long it takes to accelerate in seconds
        time_to_accelerate = self.maximum_speed / self.acceleration

        # Now figure out how long it takes to decelerate
        time_to_decelerate = -self.maximum_speed / -self.deceleration

        # Now we know how long it takes to accelerate and decelerate, but we must
        # also know how long the entire move takes so we can figure out how long
        # is the plateau if there is one

        # Only if there is actually a plateau ( we are limited by nominal_rate )
        if(maximum_possible_speed > self.speed):
            # Figure out the acceleration and deceleration distances ( in mm )
            acceleration_distance = ((initial_velocity + self.maximum_speed) / 2.0) * time_to_accelerate
            deceleration_distance = ((self.maximum_speed + 0) / 2.0) * time_to_decelerate

            # Figure out the plateau steps
            plateau_distance = distance_from_decel0 - acceleration_distance - deceleration_distance

            # Figure out the plateau time in seconds
            self.plateau_time = plateau_distance / self.maximum_speed

        # Figure out how long the move takes total ( in seconds )
        self.total_move_time = time_to_decel0 + time_to_accelerate + time_to_decelerate + self.plateau_time

        # Round the times into intervals
        decel0_intervals = floor(time_to_decel0 * self.FREQUENCY)
        acceleration_intervals = floor(time_to_accelerate * self.FREQUENCY)
        deceleration_intervals = floor(time_to_decelerate * self.FREQUENCY)
        self.total_move_intervals = floor(self.total_move_time * self.FREQUENCY)
        print("{:.0f} {:.0f} {:.0f} {:.0f}".format(
            decel0_intervals, acceleration_intervals,
            deceleration_intervals, self.total_move_intervals))

        self.decel0_time = decel0_intervals / self.FREQUENCY
        self.acceleration_time = acceleration_intervals / self.FREQUENCY
        self.deceleration_time = deceleration_intervals / self.FREQUENCY
        self.total_move_time = self.total_move_intervals / self.FREQUENCY
        print("{:.1f}s {:.1f}s {:.1f}s {:.1f}s".format(
            self.decel0_time, self.acceleration_time,
            self.deceleration_time, self.total_move_time))

        # Now figure out the two acceleration ramp change events in intervals
        self.decel0_until = decel0_intervals
        self.accelerate_until = self.decel0_until + acceleration_intervals
        self.decelerate_after = self.total_move_intervals - deceleration_intervals
        print("{:.0f} {:.0f} {:.0f} {:.0f}".format(
            self.decel0_until, self.accelerate_until - self.decel0_until,
            self.decelerate_after - self.total_move_intervals, self.total_move_intervals))

    @property
    def distance(self):
        return abs(self.current_position - self.position)

    @property
    def direction(self):
        if self.current_position < self.position:
            return self.FORWARD
        elif self.current_position > self.position:
            return self.BACKWARD
        else:
            return

    @property
    def velocity(self):
        return self.speed * self.direction

    def __repr__(self):
        return 'Move:\
 pos={0.position:.0f} cpos={0.current_position:.0f} d={0.distance:.0f}\
 spd={0.speed:.0f} cvel={0.current_velocity:.0f}\
 mspd={0.maximum_speed:.0f} d={0.distance:.0f}\
 acc={0.acceleration:.0f} dec={0.deceleration:.0f}\
 dir={0.direction:d}\
 v0i={0.vel0_until:d} ai={0.accelerate_until:d} di={0.decelerate_after:d}\
 v0t={0.vel0_time:.1f}s at={0.acceleration_time:.1f}s dt={0.deceleration_time:.1f}s\
 pt={0.plateau_time:.1f}s tt={0.total_move_time:.1f}s'.format(self)



class Planner(object):
    max_speed = 1800            # mm/s
    max_position = 15000        # mm
    max_acceleration = 1800     # mm/s^2
    max_deceleration = max_acceleration

    def __init__(self, **kwargs):
        self.speed = 0.0
        self.position = 0.0

        self._acceleration = self.max_acceleration / 2
        self._deceleration = self.max_acceleration / 2

        self._current_speed = 0.0
        self._current_velocity = 0.0
        self._current_position = 0.0
        self._current_distance = 0.0
        self._current_iteration = 0

        self._frequency = 100     # Hz
        self._interval = 1 / self._frequency

        Move.FREQUENCY = self._frequency

        self.move = None

        if kwargs.get('output_file', None) is not None:
            self.out = kwargs['output_file']
        else:
            self.out = open('./data.dat', 'w')

        if kwargs.get('separator', None) is not None:
            if kwargs['separator'] == 'tab':
                self._separator = '\t'
            elif kwargs['separator'] == 'csv':
                self._separator = ','
            else:
                self._separator = ' '
        else:
            self._separator = ' '

    def plan_move(self, position, speed):
        self.position = position
        self.speed = floor(speed * self._frequency) / self._frequency
        self.move = Move(
                position=position,
                current_position=self._current_position,
                speed=self.speed,
                current_velocity=self._current_velocity,
                acceleration=self.acceleration, deceleration=self.deceleration)

        self._current_iteration = 0

        print(repr(self.move))

    def iter(self):
        self._current_distance = self.position - self._current_position

        if self.distance == 0:
            self.out.write(self.state)
            self._current_iteration += 1
            return

        self.phase = ''
        if self._current_iteration > self.move.total_move_intervals:
            self._current_velocity = 0
            self.phase = ' STOP'
        elif self._current_iteration < self.move.vel0_until:
            self._current_velocity += (self.move.deceleration * self.move.direction) / self._frequency
            self.phase = 'VEL0'
        elif self._current_iteration > self.move.decelerate_after:
            self._current_velocity -= (self.move.deceleration * self.move.direction) / self._frequency
            self.phase = 'DECEL'
        elif self._current_iteration < self.move.accelerate_until:
            self._current_velocity += (self.move.acceleration * self.move.direction) / self._frequency
            self.phase = 'ACCEL'
        else:
            self.phase = 'PLATE'

        if abs(self._current_velocity) > self.move.maximum_speed:
            self._current_velocity = self.move.maximum_speed * self.move.direction

        self._current_position += self._current_velocity / self._frequency

        s = self.state
        if self._separator != ' ':
            s = s.split()
            s = self._separator.join(s)
        self.out.write(s + '\n')

        self._current_iteration += 1

    def iter_move(self, nb_steps=False, pause=False):
        if nb_steps is False:
            nb_steps = self.move.total_move_intervals+1

        for i in range(int(nb_steps)):
            p.iter()
            if pause:
                time.sleep(self._interval)

        if nb_steps >= self.move.total_move_intervals:
            print("Distance is {0.distance:.1f}mm".format(self))
        print()

    @property
    def current_position(self):
        return self._current_position

    @property
    def distance(self):
        return self.position - self._current_position

    @property
    def acceleration(self):
        return self._acceleration

    @acceleration.setter
    def acceleration(self, accel):
        if accel <= 0:
            self._acceleration = 0.1
            return
        elif accel > self.max_acceleration:
            self._acceleration = self.max_acceleration
            return

        self._acceleration = floor(accel / self._frequency) * self._frequency

    @property
    def deceleration(self):
        return self._deceleration

    @deceleration.setter
    def deceleration(self, decel):
        if decel <= 0:
            self._deceleration = 0.1
            return
        elif decel > self.max_deceleration:
            self._deceleration = self.max_deceleration
            return

        self._deceleration = floor(decel / self._frequency) * self._frequency

    def close(self):
        if self.out:
            self.out.close()

    @property
    def state(self):
        return '{0._current_iteration:5d} {0.phase} {0._current_position:10.2f} {0._current_velocity:10.2f} {0.distance:10.2f}' .format(self)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Graph a move")

    parser.add_argument('--output-file', '-o', type=argparse.FileType('w'), help='file to store data')
    parser.add_argument('--separator', '-s', choices=('tab', 'csv', 'space'), help='format of separator')

    args = parser.parse_args()

    kwargs = vars(args)
    p = Planner(**kwargs)

    p.plan_move(1000, 100)
    for i in range(int(p.move.total_move_time * p._frequency + 0.25 * p._frequency)):
        p.iter()

    p.plan_move(1500, 50)
    for i in range(int(p.move.total_move_time * p._frequency + 0.25 * p._frequency)):
        p.iter()

    p.plan_move(500, 100)
    for i in range(int(p.move.total_move_time * p._frequency + 0.25 * p._frequency)):
        p.iter()

    p.plan_move(0, 1000)
    for i in range(int(p.move.total_move_time * p._frequency + 0.25 * p._frequency)):
        p.iter()

    p.plan_move(500, 100)
    for i in range(int(p.move.total_move_time / 2 * p._frequency)): # Only iter for half the range
        p.iter()

    p.plan_move(0, 100)
    for i in range(int(p.move.total_move_time * p._frequency + 0.25 * p._frequency)):
        p.iter()

    p.plan_move(500, 50)
    for i in range(int(p.move.total_move_time / 2 * p._frequency)): # Only iter for half the range
        p.iter()

    p.plan_move(0, 200)
    for i in range(int(p.move.total_move_time * p._frequency + 0.25 * p._frequency)):
        p.iter()

    p.plan_move(500, 100)
    for i in range(int(p.move.total_move_time / 2 * p._frequency)): # Only iter for half the range
        p.iter()

    p.plan_move(0, 200)
    for i in range(int(p.move.total_move_time * p._frequency + 0.25 * p._frequency)):
        p.iter()
