/*
 * menu.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "menu.h"

namespace LCD_UI {
    Item::Item(Action* _action, uint8_t _position, Menu *_child_menu, const char *_text) {
        action_ptr = _action;
        text = _text;
        position = _position;
        child_menu = _child_menu;
    }

    Menu::Menu(const char *id) : menuID(id) {
        items.clear();
        _position = 0;
    }

    void Menu::addItem(Action* action, Menu *child_menu, const char* text) {
        Item item(action, (uint8_t) _position, child_menu, text);
        items.push_back(item);
        _position++;
    }
}
