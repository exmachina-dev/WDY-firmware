#ifndef LED_H_
#define LED_H_

#include "mbed.h"

DigitalOut  can_led(LED1);
DigitalOut  err_led(LED2);

PwmOut      led1(LED_HMI1);
PwmOut      led2(LED_HMI2);
PwmOut      led3(LED_HMI3);
PwmOut      led4(LED_HMI4);

#endif // LED_H
