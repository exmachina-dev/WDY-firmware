#ifndef FANS_H_
#define FANS_H_

#include "mbed.h"
#include "pins.h"

PwmOut      fan_top(FAN_P1);
PwmOut      fan_bot(FAN_P2);

#endif // FANS_H

