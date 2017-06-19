/*
 * loadpin.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LOADPIN_H
#define LOADPIN_H

#include "mbed.h"
#include "pins.h"

#define WDY_ADC_TO_VOLTS_FACTOR         (1)

float read_loadpin(void);

#endif /* !LOADPIN_H */
