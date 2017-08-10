/*
 * actions.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef ACTIONS_H
#define ACTIONS_H

#include "mbed.h"
#include "libAC780.h"

#include "WDY/helpers.h"
#include "common.h"

namespace LCD_UI {
    enum type_e {
        TYPE_UNKNOW = 0,
        TYPE_UINT,
        TYPE_BOOL,
        TYPE_IP
    };

    class Action {
        public:
            Action(AC780 *_lcd);

            bool is_running() { return _running; }
            bool is_done() { return _done; }

            void setKey(key_e _key);
            mode_e button_mode() { return _button_mode; }

            void _start() {};
            void start() {
                _running = true;
                _done = false;
                current_key = KEY_UNKNOWN;
                lcd->clear();
                _start();
            }
            void virtual update() = 0;
            void _end() {};
            void end() {
                _end();
                _running = false;
                _done = true;
            }
            const char *var_name;

        protected:
            AC780 *lcd;

            key_e last_key, current_key;
            mode_e _button_mode;
            bool _running;
            bool _done;
    };

    template <class T = uint8_t>
    class SetAction : public Action {
        public:
            SetAction(AC780 *_lcd, const char *_var_name, T *_var, type_e _type, bool _instant_update=false) : Action(_lcd) {
                var_name = _var_name;
                if (_var != NULL) {
                    var = *_var;
                    var_ptr = _var;
                } else {
                    var = 0;
                    var_ptr = NULL;
                }
                type = _type;
                _button_mode = MODE_EDIT;
                instant_update = _instant_update;
                save_func_ptr = NULL;

                step = 1;
                minimum = 0;
                maximum = 255;
            }

            void _start() {
                if (var_ptr != NULL) {
                    var = *var_ptr;
                    old_var = var;
                }
            }
            void update();

            void (*save_func_ptr)();

            T maximum;
            T minimum;
            T step;

        private:
            T var;
            T old_var;
            T *var_ptr;
            type_e type;
            bool instant_update;
    };

    template <class T>
    void SetAction<T>::update() {
        switch (current_key) {
            case KEY_UNKNOWN:
                last_key = KEY_NONE;
                break;
            case KEY_NONE:
                break;
            case KEY_PLUS:
                if ((var + step) > maximum)
                    var = maximum;
                else
                    var += step;

                if (instant_update && var_ptr != NULL)
                    *var_ptr = var;
                break;
            case KEY_MINUS:
                if ((var - step) < minimum)
                    var = minimum;
                else
                    var -= step;

                if (instant_update && var_ptr != NULL)
                    *var_ptr = var;
                break;
            case KEY_MENU:
                if (var_ptr != NULL) {
                    if (instant_update)
                        *var_ptr = old_var;
                    var = *var_ptr;
                }
                end();
                return;
                break;
            case KEY_ENTER:
                if (var_ptr != NULL)
                    *var_ptr = var;
                if (save_func_ptr != NULL) {
                    save_func_ptr();
                }
                end();
                return;
                break;
            default:
             current_key = KEY_UNKNOWN;
        }

        if (last_key == current_key)
            return;

        lcd->locate(0, 0);
        lcd->printf("%s", var_name);
        lcd->locate(1, 0);
        lcd->printf("%d   ", var);

        last_key = current_key;
    }

    template<>
    inline void SetAction<bool>::update() {
        switch (current_key) {
            case KEY_UNKNOWN:
                last_key = KEY_NONE;
                break;
            case KEY_NONE:
                break;
            case KEY_PLUS:
                var = !var;
                break;
            case KEY_MINUS:
                var = !var;
                break;
            case KEY_MENU:
                var = *var_ptr;
                end();
                return;
                break;
            case KEY_ENTER:
                *var_ptr = var;
                end();
                return;
                break;
            default:
             current_key = KEY_UNKNOWN;
        }

        if (last_key == current_key)
            return;

        lcd->locate(0, 0);
        lcd->printf("%s", var_name);
        lcd->locate(1, 0);
        lcd->printf("%s   ", (var) ? "yes" : "no");

        last_key = current_key;
    }

    class InfoAction : public Action {
        public:
            InfoAction(AC780 *_lcd, wdy_state_t *_state) : Action(_lcd) {
                state = _state;
                page = 0;
                _button_mode = MODE_EDIT;
            }

            void update();
            void pageNext() { page++; page %= 2; }
            void pagePrev() { page--; page %= 2; }

        private:
            wdy_state_t *state;
            uint8_t page;
    };

    class AboutAction : public Action {
        public:
            AboutAction(AC780 *_lcd) : Action(_lcd) {
                _button_mode = MODE_NAV;
            }

            void update();
    };
}

#endif /* !ACTIONS_H */
