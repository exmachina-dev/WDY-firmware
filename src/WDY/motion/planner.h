/*
 * planner.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef PLANNER_H
#define PLANNER_H

#include "move.h"

#define WDY_LOOP_INTERVAL_S     (1000.0 / WDY_LOOP_INTERVAL)

namespace WDY_motion {
    struct move_cmd_s {
        float velocity;
        float accel;
        float decel;
    };

    typedef move_cmd_s move_cmd_t;

    class Planner {
        public:
            Planner();

            Move* plan(float position, float speed);
            move_cmd_t get_next_interval();
            float get_distance();

            void set_accel(float _accel) {
                this->accel = (_accel > WDY_MIN_ACCEL) ? _accel : WDY_MIN_ACCEL;
                this->accel = (_accel < WDY_MAX_ACCEL) ? _accel : WDY_MAX_ACCEL;
            }
            void set_decel(float _decel) {
                this->decel = (_decel > WDY_MIN_DECEL) ? _decel : WDY_MIN_DECEL;
                this->decel = (_decel < WDY_MAX_DECEL) ? _decel : WDY_MAX_DECEL;
            }
        private:
            float speed_tgt;
            float speed_cur;

            float position_tgt;
            float position_cur;

            float accel;
            float decel;

            float velocity_cur;

            volatile uint32_t iter_cur;
            volatile uint32_t iter_mov;
            float interval;

            Move *move_cur;
            Move *move_last;

            void _plan(float _position_new, float _speed_nom,
                    float _accel, float _decel);
    };
}

#endif /* !PLANNER_H */
