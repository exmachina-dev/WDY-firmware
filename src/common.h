/*
 * common.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef COMMON_H
#define COMMON_H

#include "mbed.h"

/* LCD */
#include "libAC780.h"
#include "ui.h"

extern I2C i2c2;
extern AC780 lcd;

/* EEPROM */
#include "AT24Cxx_I2C.h"

extern I2C i2c0;
extern AT24CXX_I2C eeprom;
extern wdy_eeprom_data_t eeprom_data;

/* WDY */
extern wdy_state_t WDY_STATE;

/* Debug */
#if defined(WDY_DEBUG) && (WDY_DEBUG != 0)
#define DEBUG_PRINTF(...) (printf(__VA_ARGS__))
#else
#define DEBUG_PRINTF(...) (0)
#endif

#endif /* !COMMON_H */
