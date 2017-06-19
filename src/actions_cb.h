/*
 * actions_cb.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef ACTIONS_CB_H
#define ACTIONS_CB_H


using namespace LCD_UI;

SetAction<bool> set_action_dhcp(&lcd, "Enable DHCP", &WDY_STATE.config.network.dhcp, TYPE_BOOL);
SetAction<bool> set_action_static(&lcd, "Use static address", &WDY_STATE.config.network.static_addr, TYPE_BOOL);

SetAction<uint8_t> set_action_net(&lcd, "ArtNET net", &WDY_STATE.config.artnet.net, TYPE_UINT);
SetAction<uint8_t> set_action_subnet(&lcd, "ArtNET subnet", &WDY_STATE.config.artnet.subnet, TYPE_UINT);
SetAction<uint8_t> set_action_universe(&lcd, "ArtRET universe", &WDY_STATE.config.artnet.universe, TYPE_UINT);
SetAction<uint8_t> set_action_dmx_addr(&lcd, "DMX address", &WDY_STATE.config.artnet.dmx_addr, TYPE_UINT);

SetAction<uint8_t> set_action_contrast(&lcd, "Contrast", &WDY_STATE.config.screen.contrast, TYPE_UINT, true);
SetAction<uint8_t> set_action_backlight(&lcd, "Backlight", &WDY_STATE.config.screen.backlight, TYPE_UINT, true);

SetAction<bool> set_action_erase(&lcd, "Reset to factory", NULL, TYPE_BOOL);

InfoAction info_action(&lcd, &WDY_STATE);
AboutAction about_action(&lcd);

#endif /* !ACTIONS_CB_H */
