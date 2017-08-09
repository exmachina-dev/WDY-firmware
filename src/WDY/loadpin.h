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

#define WDY_ADC_TO_MILLIVOLTS_RANGE         3300
#define WDY_MV_TO_LOADPIN_V_FACTOR          662.269
#define WDY_LOADPIN_V_T_NEWTONS_FACTOR      100
#define WDY_NEWTONS_TO_LOAD_FACTOR          0.9

float read_loadpin(AnalogIn *adc);

#endif /* !LOADPIN_H */
