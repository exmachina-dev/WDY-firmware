/*
 * motion.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include <cmath>

#include "move.h"


using namespace WDY_motion;

Move::Move(
        float _position_cur, float _position_new,
        float _speed_cur, float _speed_nom,
        float _accel, float _decel) {
    position_cur = _position_cur;
    position_new = _position_new;

    speed_nom = _speed_nom;
    speed_cur = _speed_cur;

    accel = _accel;
    decel = _decel;

    _compute();
}

void Move::_compute() {
    // Speeds must be positive
    if (this->speed_nom < 0)
        this->speed_nom = abs(this->speed_nom);

    if (this->speed_cur < 0)
        this->speed_cur = abs(this->speed_cur);

    float velocity_cur = this->speed_cur * (float) get_direction();

    float speed_max_possible = sqrt((get_distance() * this->accel) +
            ((pow(this->speed_cur, 2) + 0) / 2.0));  // final_speed is always 0 in our case

    // Now this is the maximum rate we'll achieve this move, either because
    // it's the higher we can achieve, or because it's the higher we are
    // allowed to achieve
    float speed_max = fminf(speed_max_possible, this->speed_nom);

    // Now figure out how long it takes to accelerate in seconds
    float time_to_accelerate = abs(speed_max - velocity_cur) / this->accel;

    // Now figure out how long it takes to decelerate
    float time_to_decelerate = (0 - speed_max) / -this->decel;

    // Now we know how long it takes to accelerate and decelerate, but we must
    // also know how long the entire move takes so we can figure out how long
    // is the plateau if there is one

    // Only if there is actually a plateau ( we are limited by nominal_rate )
    float plateau_time = 0.0;
    if (speed_max_possible > this->speed_nom) {
        // Figure out the acceleration and deceleration distances ( in mm )
        float accel_distance = ((velocity_cur + speed_max) / 2.0) * time_to_accelerate;
        float decel_distance = ((speed_max + 0) / 2.0) * time_to_decelerate;

        // Figure out the plateau steps
        float plateau_distance = get_distance() - accel_distance - decel_distance;

        // Figure out the plateau time in seconds
        plateau_time = plateau_distance / speed_max;
    }

    // Figure out how long the move takes total ( in seconds )
    float total_move_time = time_to_accelerate + time_to_decelerate + plateau_time;

    // Round the times into intervals
    uint32_t accel_intervals = floorf(time_to_accelerate * WDY_LOOP_FREQUENCY);
    uint32_t decel_intervals = floorf(time_to_decelerate * WDY_LOOP_FREQUENCY);
    this->total_move_intervals = floorf(total_move_time * WDY_LOOP_FREQUENCY);

    this->accel_time = (float) accel_intervals * WDY_LOOP_INTERVAL_S;
    this->decel_time = (float) decel_intervals * WDY_LOOP_INTERVAL_S;
    this->total_move_time = (float) this->total_move_intervals * WDY_LOOP_INTERVAL_S;

    // Now figure out the two acceleration ramp change events in intervals
    this->accelerate_until = accel_intervals;
    this->decelerate_after = this->total_move_intervals - decel_intervals;
}

float Move::get_distance() {
    return abs(this->position_cur - this->position_new);
}

direction_e Move::get_direction() {
    if (this->position_cur < this->position_new)
        return DIR_FORWARD;
    else if (this->position_cur > this->position_new)
        return DIR_BACKWARD;
    else
        return DIR_NONE;
}
