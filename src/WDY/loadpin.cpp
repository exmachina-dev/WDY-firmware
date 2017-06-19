/*
 * loadpin.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "loadpin.h"


// Loadpin ADC
AnalogIn loadpin_adc(ADC_LOADPIN);

static inline float adc_to_volts(float adc) {
    return adc * WDY_ADC_TO_VOLTS_FACTOR;
}

float read_loadpin() {
    return adc_to_volts(loadpin_adc.read());
}

