/*
 * lcd_ui.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "lcd_ui.h"

#include <cstdarg>
#include <cstdio>


#include "common.h"
#include "actions_cb.h"

#include "WDY/leds.h"
#include "WDY/buttons.h"

volatile bool WDY_UI_MENU_FLAG;
volatile bool WDY_UI_DOWN_FLAG;
volatile bool WDY_UI_UP_FLAG;
volatile bool WDY_UI_ENTER_FLAG;

// static buffer for the error strings
char wdy_init_text[18];

void menu_interrupt(void) {
    WDY_UI_MENU_FLAG = true;
}

void down_interrupt(void) {
    WDY_UI_DOWN_FLAG = true;
}

void up_interrupt(void) {
    WDY_UI_UP_FLAG = true;
}

void enter_interrupt(void) {
    WDY_UI_ENTER_FLAG = true;
}

void save_func_cb(void) {
    WDY_eeprom_write_config(&eeprom, &WDY_STATE.config);
}

void wdy_init_msg(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(wdy_init_text, sizeof(wdy_init_text), fmt, ap);
    va_end(ap);
}

void UI_app_task(void) {
    bool _init = true;
    uint16_t period_counter = 0;
    uint8_t lcd_contrast = 0;

    // Status led setup
    led1.period(0.02);
    led2.period(0.02);
    led3.period(0.02);
    led4.period(0.02);

    button2.mode(PullUp);
    button3.mode(PullUp);
    button4.mode(PullUp);
    button5.mode(PullUp);

    button2.fall(&menu_interrupt);
    button3.fall(&up_interrupt);
    button4.fall(&down_interrupt);
    button5.fall(&enter_interrupt);

    // I2C bus speed: 400 kHz
    i2c2.frequency(400 * 1000);

    lcd.setBacklight(true);
    lcd.setContrast(WDY_STATE.config.screen.contrast);

    lcd.setUDC(0, UDC_bar_left_empty);
    lcd.setUDC(1, UDC_bar_left_full);
    lcd.setUDC(2, UDC_bar_middle_empty);
    lcd.setUDC(3, UDC_bar_middle_full);
    lcd.setUDC(4, UDC_bar_right_empty);
    lcd.setUDC(5, UDC_bar_right_full);

    lcd.locate(0, 3);
    lcd.printf("Winch  Dynamic");
    lcd.locate(1, 5);
    lcd.printf("by ExMachina");

    using namespace LCD_UI;

    Menu _root_menu("Main menu");

    Menu _config_menu("Config");

    Menu _network_menu("Network");
    _network_menu.addItem(&set_action_dhcp, NULL, "DHCP");
    _network_menu.addItem(NULL, NULL, "IP address");
    _network_menu.addItem(NULL, NULL, "Netmask");
    _network_menu.addItem(NULL, NULL, "Gateway");
    _network_menu.addItem(NULL, &_config_menu, "--Back--");
    _config_menu.addItem(NULL, &_network_menu, "Network");

    Menu _artnet_menu("ArtNET");
    _artnet_menu.addItem(&set_action_net, NULL, "Net");
    _artnet_menu.addItem(&set_action_subnet, NULL, "Subnet");
    _artnet_menu.addItem(&set_action_universe, NULL, "Universe");
    _artnet_menu.addItem(&set_action_dmx_addr, NULL, "DMX address");
    _artnet_menu.addItem(NULL, &_config_menu, "--Back--");
    _config_menu.addItem(NULL, &_artnet_menu, "ArtNET");

    Menu _screen_menu("Screen");
    _screen_menu.addItem(&set_action_contrast, NULL, "Contrast");
    _screen_menu.addItem(&set_action_backlight, NULL, "Backlight");
    _screen_menu.addItem(NULL, &_config_menu, "--Back--");
    _config_menu.addItem(NULL, &_screen_menu, "Screen");

    _config_menu.addItem(NULL, &_root_menu, "--Back--");

    _root_menu.addItem(&info_action, NULL, "Infos");
    _root_menu.addItem(NULL, &_config_menu, "Config");
    _root_menu.addItem(&about_action, NULL, "About");

    UI ui(&lcd, &_root_menu);

    ui.setDefaultAction(&info_action);

    set_action_backlight.save_func_ptr = &save_func_cb;
    set_action_contrast.save_func_ptr = &save_func_cb;

    set_action_contrast.minimum = 1;
    set_action_contrast.maximum = 60;
    set_action_backlight.step = 5;
    set_action_backlight.minimum = 5;
    set_action_backlight.maximum = 60;

    while (true) {
        if (_init) {
            led1 = 1;
            led2 = 1;
            led3 = 1;
            led4 = 1;


            if (WDY_STATE.init_state < 0) {
                ui.splashscreen(WDY_STATE.init_state, wdy_init_text);
            } else if (WDY_STATE.init_state >= 100) {
                _init = false;

                ui.splashscreen(WDY_STATE.init_state, wdy_init_text);
                Thread::wait(500);

                led1 = 0;
                led2 = 0;
                led3 = 0;
                led4 = 0;
            } else {
                ui.splashscreen(WDY_STATE.init_state, wdy_init_text);
            }

            Thread::wait(50);
        }

        if (WDY_UI_MENU_FLAG || WDY_UI_UP_FLAG || WDY_UI_DOWN_FLAG || WDY_UI_MENU_FLAG)
            period_counter = 0;

        // Update keys base on flags
        if (WDY_UI_MENU_FLAG) {
            ui.setKey(KEY_MENU);
            WDY_UI_MENU_FLAG = false;
        } else if (WDY_UI_UP_FLAG) {
            ui.setKey(KEY_UP);
            WDY_UI_UP_FLAG = false;
        } else if (WDY_UI_DOWN_FLAG) {
            ui.setKey(KEY_DOWN);
            WDY_UI_DOWN_FLAG = false;
        } else if (WDY_UI_ENTER_FLAG) {
            ui.setKey(KEY_ENTER);
            WDY_UI_ENTER_FLAG = false;
        } else {
            ui.setKey(KEY_NONE);
        }

        ui.update();

        if (lcd_contrast != WDY_STATE.config.screen.contrast) {
            lcd_contrast = WDY_STATE.config.screen.contrast;
            lcd.setContrast(lcd_contrast);
        }

        // Turn off backlight after backlight period
        if (period_counter > WDY_STATE.config.screen.backlight * (1000 / 50))
            lcd.setBacklight(false);
        else
            lcd.setBacklight(true);

        // Update leds
        if (WDY_STATE.status != 0) {
            ui.blink_code(&led4, WDY_STATE.status);
        } else {
            led4 = 0;
        }

        led2 = ((float) (period_counter % 16)) / 16;
        led3 = ((float) (period_counter % 8)) / 16;

        period_counter++;

        Thread::wait(50);
    }
}
