/*
 * inputs.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef INPUTS_H
#define INPUTS_H

#include "mbed.h"
#include "pins.h"

// Inputs
DigitalIn drive_status(DRV_STS1);
DigitalIn slowfeed_input(DRV_STS2);
DigitalIn drum_limit(LIMIT_SW1);
DigitalIn emergency_stop(EMERGENCY_STOP);


#endif /* !INPUTS_H */
