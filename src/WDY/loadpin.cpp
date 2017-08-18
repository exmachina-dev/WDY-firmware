/*
 * loadpin.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "loadpin.h"


static inline float adc_to_load(float adc) {
    return adc * WDY_ADC_TO_MILLIVOLTS_RANGE / 1000
    * WDY_MV_TO_LOADPIN_V_FACTOR
    * WDY_LOADPIN_V_T_NEWTONS_FACTOR
    * WDY_NEWTONS_TO_LOAD_FACTOR / 10;
}

float read_loadpin(AnalogIn *adc) {
    return adc_to_load(adc->read());
}

