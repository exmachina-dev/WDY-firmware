/*
 * common.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "common.h"


using namespace LCD_UI;

int8_t LCD_UI::netmask_to_cidr(in_addr netmask) {
    switch (netmask.s_addr) {
        case 0xFFFFFFFF:
            return 32;
            break;
        case 0xEFFFFFFF:
            return 31;
            break;
        case 0xCFFFFFFF:
            return 30;
            break;
        case 0x8FFFFFFF:
            return 29;
            break;
        case 0x0FFFFFFF:
            return 28;
            break;
        case 0x0EFFFFFF:
            return 27;
            break;
        case 0x0CFFFFFF:
            return 26;
            break;
        case 0x08FFFFFF:
            return 25;
            break;
        case 0x00FFFFFF:
            return 24;
            break;
        case 0x00EFFFFF:
            return 23;
            break;
        case 0x00CFFFFF:
            return 22;
            break;
        case 0x008FFFFF:
            return 21;
            break;
        case 0x000FFFFF:
            return 20;
            break;
        case 0x000EFFFF:
            return 19;
            break;
        case 0x000CFFFF:
            return 18;
            break;
        case 0x0008FFFF:
            return 17;
            break;
        case 0x0000FFFF:
            return 16;
            break;
        case 0x0000EFFF:
            return 15;
            break;
        case 0x0000CFFF:
            return 14;
            break;
        case 0x00008FFF:
            return 13;
            break;
        case 0x00000FFF:
            return 12;
            break;
        case 0x00000EFF:
            return 11;
            break;
        case 0x00000CFF:
            return 10;
            break;
        case 0x000008FF:
            return 9;
            break;
        case 0x000000FF:
            return 8;
            break;
        case 0x000000EF:
            return 7;
            break;
        case 0x000000CF:
            return 6;
            break;
        case 0x0000008F:
            return 5;
            break;
        case 0x0000000F:
            return 4;
            break;
        case 0x0000000E:
            return 3;
            break;
        case 0x0000000C:
            return 2;
            break;
        case 0x00000008:
            return 1;
            break;
        case 0x00000000:
            return 0;
            break;
        default:
            return -1;
    }
}
