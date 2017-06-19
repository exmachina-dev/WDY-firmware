/*
 * actions.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "actions.h"


using namespace LCD_UI;

Action::Action(AC780 *_lcd) {
    lcd = _lcd;
    last_key = KEY_NONE;
    current_key = KEY_UNKNOWN;
    _button_mode = MODE_EDIT;

    _running = false;
    _done = false;
}

void Action::setKey(key_e _key) { 
    switch (_key) {
        case KEY_DOWN:
            current_key = KEY_PLUS;
            break;
        case KEY_UP:
            current_key = KEY_MINUS;
            break;
        default:
            current_key = _key;
            break;
    }
}

void InfoAction::update() {
    switch (current_key) {
        case KEY_UNKNOWN:
            last_key = KEY_NONE;
            break;
        case KEY_NONE:
            break;
        case KEY_PLUS:
            pageNext();
            break;
        case KEY_MINUS:
            pagePrev();
            break;
        case KEY_MENU:
            end();
            return;
            break;
        case KEY_ENTER:
            end();
            return;
            break;
        default:
            current_key = KEY_UNKNOWN;
    }

    switch (page) {
        case 0:
            lcd->locate(0, 0);
            lcd->printf("IP: %s/%d", inet_ntoa(state->config.network.ip_addr),
                    netmask_to_cidr(state->config.network.nm_addr));
            lcd->locate(1, 0);
            lcd->printf("Net: % 3d Subnet: % 2d", state->config.artnet.net, state->config.artnet.subnet);
            lcd->locate(2, 0);
            lcd->printf("Uni.: % 2d   DMX: % 3d", state->config.artnet.universe, state->config.artnet.dmx_addr);
            break;
        case 1:
            lcd->locate(0, 0);
            lcd->printf("Err: % 3d         ", state->status);
            lcd->locate(1, 0);
            lcd->printf("Pos: % 5.1f      ", state->position);
            lcd->locate(2, 0);
            lcd->printf("Spd: % 5.1f      ", state->speed);
            break;
    }

    last_key = current_key;
}

void AboutAction::update() {
    switch (current_key) {
        case KEY_UNKNOWN:
            last_key = KEY_NONE;
            break;
        case KEY_MENU:
            end();
            return;
            break;
        default:
            current_key = KEY_UNKNOWN;
    }

    if (last_key == current_key)
        return;

    lcd->locate(0, 0);
    lcd->printf("Winch Dynamic");
    lcd->locate(1, 0);
    lcd->printf("by ExMachina");

    last_key = current_key;
}
