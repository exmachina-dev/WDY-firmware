# -*- coding: utf8 -*-

"""
planner.py

Author: Benoit Rapidel

Algorithm taken from [Block.cpp on Smoothieware project](https://github.com/Smoothieware/Smoothieware/blob/edge/src/modules/robot/Block.cpp)

Plan a trapezoidal move and output the move profile.
"""

import time

from math import sqrt


class Move(object):
    """
    Store and compute a move.
    """

    KEYS = ('distance', 'speed', 'current_speed', 'acceleration', 'deceleration')

    def __init__(self, **kwargs):
        try:
            for key in self.KEYS:
                self.__dict__[key] = kwargs[key]
        except KeyError as e:
            print('Missing required argument: {}'.format(e))

        self.plateau_time = 0
        self.total_move_time = 0
        self.accelerate_until = 0
        self.decelerate_after = 0

        self.plan()

    def plan(self):
        initial_speed = self.current_speed
        # float final_rate = this->nominal_rate * (exitspeed / this->nominal_speed) # Always 0

        # How many steps ( can be fractions of steps, we need very precise values ) to accelerate and decelerate
        # This is a simplification to get rid of rate_delta and get the steps/s² accel directly from the mm/s² accel
        # float acceleration_per_second = (this->acceleration * this->steps_event_count) / this->millimeters

        maximum_possible_speed = sqrt((self.distance * self.acceleration) +
                ((pow(initial_speed, 2) + 0) / 2.0))  # final_speed is always 0 in our case

        # Now this is the maximum rate we'll achieve this move, either because
        # it's the higher we can achieve, or because it's the higher we are
        # allowed to achieve
        maximum_speed = min(maximum_possible_speed, self.speed)

        # Now figure out how long it takes to accelerate in seconds
        time_to_accelerate = (maximum_speed - initial_speed) / self.acceleration

        # Now figure out how long it takes to decelerate
        time_to_decelerate = (0 - maximum_speed) / -self.deceleration

        # Now we know how long it takes to accelerate and decelerate, but we must
        # also know how long the entire move takes so we can figure out how long
        # is the plateau if there is one

        # Only if there is actually a plateau ( we are limited by nominal_rate )
        if(maximum_possible_speed > self.speed):
            # Figure out the acceleration and deceleration distances ( in mm )
            acceleration_distance = ((initial_speed + maximum_speed) / 2.0) * time_to_accelerate
            deceleration_distance = ((maximum_speed + 0) / 2.0) * time_to_decelerate

            # Figure out the plateau steps
            plateau_distance = self.distance - acceleration_distance - deceleration_distance

            # Figure out the plateau time in seconds
            self.plateau_time = plateau_distance / maximum_speed

        # Figure out how long the move takes total ( in seconds )
        self.total_move_time = time_to_accelerate + time_to_decelerate + self.plateau_time

        # Now figure out the two acceleration ramp change events in ticks
        self.accelerate_until = time_to_accelerate
        self.decelerate_after = self.total_move_time - time_to_decelerate


class Planner(object):
    max_speed = 1800            # mm/s
    max_position = 15000        # mm
    max_acceleration = 1800     # mm/s^2
    max_deceleration = max_acceleration

    def __init__(self):
        self.speed = 0.0
        self.position = 0.0

        self._acceleration = 50.0
        self._deceleration = 50.0

        self._current_speed = 0.0
        self._current_position = 0.0
        self._current_iteration = 0

        self._frequency = 10     # Hz
        self._interval = 1 / self._frequency

        self.move = None

        self.out = open('./data.dat', 'w')

    def plan_move(self, position, speed):
        self.position = position
        self.speed = speed
        self.move = Move(distance=abs(self._current_position - position),
                speed=speed,
                current_speed=self._current_speed,
                acceleration=self.acceleration, deceleration=self.deceleration)

        self._current_iteration = 0

        print(vars(self.move))

    def iter(self):
        _distance = self.position - self._current_position

        phase = ''
        if (self.move.accelerate_until * self._frequency) > self._current_iteration:
            self._current_speed += self.move.acceleration * self._interval
            phase = 'ACCEL'
        elif (self.move.total_move_time * self._frequency) <= self._current_iteration:
            self._current_speed = 0
            phase = ' STOP'
        elif (self.move.decelerate_after * self._frequency) <= self._current_iteration:
            self._current_speed -= self.move.deceleration * self._interval
            phase = 'DECEL'
        else:
            phase = 'PLATE'

        if _distance < 0:
            self._current_position -= self._current_speed * self._interval
        else:
            self._current_position += self._current_speed * self._interval

        print('{:3d} {}: {:10.2f} {:10.2f} {:10.2f}' .format(self._current_iteration, phase, self._current_position, self._current_speed, _distance))
        self.out.write('{:3d} {} {:10.2f} {:10.2f} {:10.2f}\n' .format(self._current_iteration, phase, self._current_position, self._current_speed, _distance))

        self._current_iteration += 1

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

        self._acceleration = float(accel)

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

        self._deceleration = float(decel)

if __name__ == '__main__':
    p = Planner()

    p.plan_move(1000, 100)
    for i in range(int(p.move.total_move_time * p._frequency) + 1):
        p.iter()

    p.plan_move(1500, 50)
    for i in range(int(p.move.total_move_time * p._frequency) + 1):
        p.iter()

    p.plan_move(500, 100)
    for i in range(int(p.move.total_move_time * p._frequency) + 1):
        p.iter()

    p.plan_move(0, 1000)
    for i in range(int(p.move.total_move_time * p._frequency) + 1):
        p.iter()
