/*
 * common.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <stdint.h>

#include <inet.h>

namespace LCD_UI {
    enum key_e {
        KEY_UNKNOWN = -1,
        KEY_NONE = 0,
        KEY_MENU,
        KEY_UP,
        KEY_DOWN,
        KEY_ENTER,
        KEY_PLUS,
        KEY_MINUS,
    };

    enum mode_e {
        MODE_NAV = 0,
        MODE_EDIT,
        MODE_CONFIRM,
    };

    int8_t netmask_to_cidr(in_addr netmask);
}

#endif /* !COMMON_H */
