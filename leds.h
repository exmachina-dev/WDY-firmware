#ifndef LED_H_
#define LED_H_

#include "mbed.h"

DigitalOut  can_led(LED1);
//DigitalOut  err_led(LED2);
PwmOut      spd_led(LED3);
//PwmOut      pos_led(LED4);

#endif // LED_H
