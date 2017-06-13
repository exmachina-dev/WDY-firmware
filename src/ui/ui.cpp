/*
 * ui.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "ui.h"

namespace LCD_UI {

    UI::UI(AC780 *_lcd, Menu *_root_menu) {
        lcd = _lcd;
        root_menu = _root_menu;
        active_menu = root_menu;
        menu_size = active_menu->items.size();
        cursor_pos = 0;
        cursor_line = 0;

        active_action = NULL;
        default_action = NULL;

        last_key = KEY_NONE;
        current_key = KEY_UNKNOWN;

        _display_menu_flag = false;
        _update_menu_flag = true;
        _update_buttons_flag = true;
        _cycles = 0;
    }

    void UI::update() {
        if(!_display_menu_flag) { // Don't do anything in the menu
            switch (current_key) {
                case KEY_UNKNOWN:
                    last_key = KEY_NONE; // Force update
                    break;
                default:
                    current_key = KEY_UNKNOWN;
            }
        } else {
            switch (current_key) {
                case KEY_UNKNOWN:
                    last_key = KEY_NONE; // Force update
                    break;
                case KEY_NONE:
                case KEY_PLUS:
                case KEY_MINUS:
                    break;
                case KEY_MENU:
                    if (last_key != current_key) {
                        if (active_menu == root_menu)
                            _display_menu_flag = !_display_menu_flag;
                        else
                            active_menu = active_menu->parent();
                            menu_size = active_menu->items.size();
                            cursor_pos = 0;
                    }
                    break;
                case KEY_UP:
                    if (last_key != current_key)
                        moveUp();
                    break;
                case KEY_DOWN:
                    if (last_key != current_key)
                        moveDown();
                    break;
                case KEY_ENTER:
                    if (last_key != current_key) {
                        if (active_menu->items[cursor_pos].action_ptr != NULL)
                            active_action = active_menu->items[cursor_pos].action_ptr;

                        if (active_action != NULL && !active_action->is_running()) {  // Action not running
                            active_action->start();
                            _display_menu_flag = false;
                        }

                        if (active_menu->items[cursor_pos].child_menu != NULL) {
                            active_menu = active_menu->items[cursor_pos].child_menu;
                            menu_size = active_menu->items.size();
                            cursor_pos = 0;
                        }
                    }
                    break;
                default:
                    current_key = KEY_UNKNOWN;
            }
        }

        if (active_action != NULL && active_action->is_done()) {
            active_action = NULL;
            _display_menu_flag = true;
        }

        if (_display_menu_flag) {
            displayMenu();
            if (last_key != current_key)
                displayButtonsBar(MODE_NAV);
        } else if (active_action != NULL && active_action->is_running()) {
            active_action->update();
            if (last_key != current_key)
                displayButtonsBar(default_action->button_mode());
        } else {
            _display_menu_flag = true;
        }


        last_key = current_key;

        _cycles++;
    }

    void UI::moveUp(void) {
        cursor_pos--;
        cursor_pos %= menu_size;
    }

    void UI::moveDown(void) {
        cursor_pos++;
        cursor_pos %= menu_size;
    }

    void UI::displayMenu(void) {
        if (last_key == current_key)
            return;     // Nothing changed, nothing to update

        lcd->clear();
        lcd->locate(0, 0);
        if (active_menu != root_menu)
            lcd->printf("               %s", active_menu->menuID);

        lcd->locate(0, 0);
        if (cursor_pos == 0) {
            lcd->printf(" %s\n", active_menu->items[cursor_pos].text);

            if (menu_size > 1)
                lcd->printf(" %s\n", active_menu->items[cursor_pos+1].text);
            if (menu_size > 2)
                lcd->printf(" %s\n", active_menu->items[cursor_pos+2].text);

            cursor_line = 0;
        } else if (cursor_pos >= (menu_size - 1)) {
            if (menu_size > 2)
                lcd->printf(" %s\n", active_menu->items[cursor_pos-2].text);

            if (menu_size > 1)
                lcd->printf(" %s\n", active_menu->items[cursor_pos-1].text);

            cursor_line = (menu_size > 2) ? 2 : 1;

            lcd->printf(" %s\n", active_menu->items[cursor_pos].text);

        } else {
            lcd->printf(" %s\n", active_menu->items[cursor_pos-1].text);
            lcd->printf(" %s\n", active_menu->items[cursor_pos].text);
            if (cursor_pos < menu_size-1)
                lcd->printf(" %s\n", active_menu->items[cursor_pos+1].text);

            cursor_line = 1;
        }

        lcd->locate(cursor_line, 0);
        lcd->putc(1);
    }

    void UI::displayButtonsBar(mode_e mode=MODE_NAV) {
        char icons[] = {4, 2, 3, 5};

        switch (mode) {
            case MODE_NAV:
                icons[0] = 4;
                icons[1] = 2;
                icons[2] = 3;
                icons[3] = 5;
                break;
            case MODE_EDIT:
                icons[0] = 4;
                icons[1] = 45;
                icons[2] = 43;
                icons[3] = 5;
                break;
            case MODE_CONFIRM:
                icons[0] = 4;
                icons[1] = 32;
                icons[2] = 6;
                icons[3] = 5;
                break;
        }

        for (int b=0; b<4; b++) {
            lcd->locate(3, 2 + b * 5);
            lcd->putc(icons[b]);
        }
    }

    void UI::setKey(key_e _key) {
        current_key = _key;

        if(active_action != NULL)
            active_action->setKey(current_key);
    }

    void UI::setDefaultAction(Action *_action) {
        default_action = _action;
        active_action = default_action;
        active_action->start();
    }

    bool UI::display_menu(void) {
        return _display_menu_flag;
    }

    void UI::blink_code(PwmOut *led, uint16_t code) {
        uint16_t n_cycle = _cycles % (4 * (code + 4));
        led->write((n_cycle > 7) ? (((n_cycle-8) / 2) % 2) : 0);
    }
}
