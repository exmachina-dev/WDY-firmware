/*
 * actions_cb.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef ACTIONS_CB_H
#define ACTIONS_CB_H

#include "common.h"


LCD_UI::SetAction<bool> set_action_dhcp(&lcd, "Enable DHCP", &WDY_STATE.config.network.dhcp, LCD_UI::TYPE_BOOL);
LCD_UI::SetAction<bool> set_action_static(&lcd, "Use static address", &WDY_STATE.config.network.static_addr, LCD_UI::TYPE_BOOL);

LCD_UI::SetAction<uint8_t> set_action_net(&lcd, "ArtNET net", &WDY_STATE.config.artnet.net, LCD_UI::TYPE_UINT);
LCD_UI::SetAction<uint8_t> set_action_subnet(&lcd, "ArtNET subnet", &WDY_STATE.config.artnet.subnet, LCD_UI::TYPE_UINT);
LCD_UI::SetAction<uint8_t> set_action_universe(&lcd, "ArtRET universe", &WDY_STATE.config.artnet.universe, LCD_UI::TYPE_UINT);
LCD_UI::SetAction<uint8_t> set_action_dmx_addr(&lcd, "DMX address", &WDY_STATE.config.artnet.dmx_addr, LCD_UI::TYPE_UINT);

LCD_UI::SetAction<uint8_t> set_action_contrast(&lcd, "Contrast", &WDY_STATE.config.screen.contrast, LCD_UI::TYPE_UINT, true);
LCD_UI::SetAction<uint8_t> set_action_backlight(&lcd, "Backlight", &WDY_STATE.config.screen.backlight, LCD_UI::TYPE_UINT, true);

LCD_UI::SetAction<bool> set_action_erase(&lcd, "Reset to factory", NULL, LCD_UI::TYPE_BOOL);

LCD_UI::InfoAction info_action(&lcd, &WDY_STATE);
LCD_UI::AboutAction about_action(&lcd);

#endif /* !ACTIONS_CB_H */
