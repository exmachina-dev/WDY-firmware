/*
 * eeprom.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef WDY_EEPROM_H
#define WDY_EEPROM_H

#include "AT24Cxx_I2C.h"

I2C i2c0(I2C0_SDA, I2C0_SCL);
AT24CXX_I2C eeprom(&i2c0, 0x50);
wdy_eeprom_data_t eeprom_data;

#endif /* !WDY_EEPROM_H */
