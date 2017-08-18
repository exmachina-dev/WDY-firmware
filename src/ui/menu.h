/*
 * menu.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef UI_MENU_H
#define UI_MENU_H

#include "mbed.h"
#include <vector>

#include "ui/actions.h"

namespace LCD_UI {
    class Menu;

    class Item {
        public:
            Item(Action *_action_ptr, uint8_t _position, Menu *_child_menu, const char *_text); 

            Action *action_ptr;
            const char* text;
            uint8_t position;
            Menu *child_menu; 
    };

    class Menu {
        public:
            Menu(const char*);

            Menu* parent() { return _parent; }
            void parent(Menu *_menu) { _parent = _menu; }

            vector<Item> items;
            const char* menuID;

            void addItem(Action *action, Menu *child_menu, const char* text);

        private:
            uint8_t _position;
            Menu *_parent;
    };
}

#endif /* !UI_MENU_H */
