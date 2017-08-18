/*
 * ui.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef UI_H
#define UI_H

#include "ui/common.h"
#include "ui/menu.h"
#include "ui/actions.h"
#include "libAC780.h"

#define LCD_LINES       (AC780_CONF_ROWS)
#define CYCLE_PERIOD    (50)

namespace LCD_UI {
    class UI {
        public:
            UI(AC780 *lcd, Menu *root_menu);
            Menu *active_menu, *root_menu;
            Action *default_action;
            AC780 *lcd;

            uint8_t menu_size;
            uint8_t cursor_pos;
            uint8_t cursor_line;

            void update(void);
            void moveUp(void);
            void moveDown(void);
            void displayMenu(void);
            void displayButtonsBar(mode_e mode);
            void setKey(key_e _key);
            void setDefaultAction(Action *_action);

            bool display_menu(void);
            void splashscreen(int8_t percent, const char *text);

            void blink_code(PwmOut *led, uint16_t code);

        private:
            Action *active_action;
            key_e last_key, current_key;
            bool _display_splashscreen_flag;
            bool _display_menu_flag;

            bool _update_buttons_flag;
            bool _update_menu_flag;

            uint16_t _cycles;

            void printbar(int length, int perc);
    };
} // namespace UI

#endif /* !UI_H */
