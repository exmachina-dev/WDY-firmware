/*
 * lcd.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef WDY_LCD_H
#define WDY_LCD_H


#include "mbed.h"
#include "WDY/pins.h"
#include "libAC780.h"
#include "ui.h"

I2C i2c2(I2C2_SDA, I2C2_SCL);
AC780 lcd(&i2c2, 0x78, LCD_BACKLIGHT, 0x5c, true);

#endif /* !WDY_LCD_H */
