/*
 * adc.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef ADC_H
#define ADC_H

AnalogIn adc_temp0(ADC_TEMP0);
AnalogIn adc_temp1(ADC_TEMP1);

// Loadpin ADC
AnalogIn adc_loadpin(ADC_LOADPIN);

#endif /* !ADC_H */
