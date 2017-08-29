/*
 * planner.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "planner.h"
#include "config.h"


using namespace WDY_motion;

Planner::Planner() {
    speed_tgt = 0.0;
    speed_cur = 0.0;

    position_tgt = 0.0;
    position_cur = 0.0;

    accel = WDY_MIN_ACCEL;
    decel = WDY_MIN_DECEL;

    velocity_cur = 0.0;

    iter_cur = 0;
    interval = WDY_LOOP_INTERVAL_S;

    move_cur = NULL;
    move_last = NULL;
}

Move* Planner::plan(float _position, float _speed) {
    _position = (_position > WDY_MAX_POSITION) ? WDY_MAX_POSITION : _position;
    _position = (_position < WDY_MIN_POSITION) ? WDY_MIN_POSITION : _position;
    _speed = (_speed > WDY_MAX_SPEED) ? WDY_MAX_SPEED : _speed;
    _speed = (_speed < WDY_MIN_SPEED) ? WDY_MIN_SPEED : _speed;
    this->accel = (this->accel > WDY_MAX_ACCEL) ? WDY_MAX_ACCEL : this->accel;
    this->decel = (this->decel < WDY_MIN_DECEL) ? WDY_MIN_DECEL : this->decel;

    _plan(_position, _speed, this->accel, this->decel);
    return this->move_cur;
}

void Planner::get_next_interval(move_cmd_t * move) {
    move->accel = this->accel;
    move->decel = this->decel;

    float distance_cur = this->get_distance();

    if (distance_cur == 0 || this->accel <= 0.0 || this->decel <= 0.0) {
        iter_cur++;
        move->position = this->position_cur;
        move->velocity = 0.0;
        move->phase = PHASE_IDLE;
        return;
    }

    if (this->move_cur->total_move_intervals < this->iter_cur) // STOP
        this->velocity_cur = 0.0;
        move->phase = PHASE_IDLE;
    else if (this->move_cur->accelerate_until > this->iter_cur) // ACCEL
        this->velocity_cur += this->move_cur->accel * this->interval * (float) this->move_cur->get_direction();
        move->phase = PHASE_ACCEL;
    else if (this->move_cur->decelerate_after < this->iter_cur) // DECEL
        this->velocity_cur -= this->move_cur->decel * this->interval * (float) this->move_cur->get_direction();
        move->phase = PHASE_DECEL;
    } else
        move->phase = PHASE_PLATE;

    this->position_cur += this->velocity_cur * this->interval;

    move->velocity = this->velocity_cur;
    move->position = this->position_cur;

    iter_cur++;
}

float Planner::get_distance() {
    return this->position_tgt - this->position_cur;
}

void Planner::_plan(float _position_new, float _speed_nom,
        float _accel, float _decel) {
    if (this->move_cur != NULL)
        free(this->move_cur);

    this->move_cur = new Move(this->position_cur, _position_new,
            this->speed_cur, _speed_nom, _accel, _decel);

    this->position_tgt = _position_new;
    this->speed_tgt = _speed_nom;
    this->iter_cur = 0;
    this->iter_mov = this->move_cur->total_move_intervals;
}
