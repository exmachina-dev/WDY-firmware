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

#include "WDY_helpers.h"
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

            void start() {
                _running = true;
                _done = false;
                current_key = KEY_UNKNOWN;
                lcd->clear();
            }
            void virtual update() = 0;
            void end() {
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
            SetAction(AC780 *_lcd, const char *_var_name, T *_var, type_e _type) : Action(_lcd) {
                var_name = _var_name;
                var = *_var;
                var_ptr = _var;
                type = _type;
                _button_mode = MODE_EDIT;
            }

            void update();

        private:
            T var;
            T *var_ptr;
            type_e type;
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
                var++;
                break;
            case KEY_MINUS:
                var--;
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
