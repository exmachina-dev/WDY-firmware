/*
 * motion.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef MOTION_H
#define MOTION_H

#include "common.h"
#include "WDY/config.h"

namespace WDY_motion {
    enum direction_e {
        DIR_BACKWARD    = -1,
        DIR_NONE        = 0,
        DIR_FORWARD     = 1
    };

    class Move {
        public:
            Move(
                    float _position_cur, float _position_new,
                    float _speed_cur, float _speed_nom,
                    float _accel, float _decel);

            float get_distance();
            direction_e get_direction();
        private:
            void _compute();

        public:
            float position_cur;
            float position_new;
            float speed_cur;
            float speed_nom;
            float accel;
            float decel;

            uint32_t accelerate_until;
            uint32_t decelerate_after;
            uint32_t total_move_intervals;

            float accel_time;
            float decel_time;
            float total_move_time;
    };
}

#endif /* !MOTION_H */
