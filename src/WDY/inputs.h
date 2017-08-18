/*
 * inputs.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef WDY_INPUTS_H
#define WDY_INPUTS_H

#include "mbed.h"
#include "pins.h"

// System inputs
DigitalIn drive_enable(DRV_ENABLE);
DigitalIn drive_error(DRV_ERROR);
DigitalIn brake_sensor1(BRK_SENSOR1);
DigitalIn brake_sensor2(BRK_SENSOR2);

#endif /* !WDY_INPUTS_H */
