#ifndef LED_H_
#define LED_H_

#include "mbed.h"

DigitalOut  can_led(P1_21);
DigitalOut  err_led(P1_18);

PwmOut      led1(P2_2);
PwmOut      led2(P2_3);
PwmOut      led3(P2_4);
PwmOut      led4(P2_5);

#endif // LED_H
