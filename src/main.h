/*
 * main.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "WDY/config.h"

/* Mbed includes */
#include "mbed.h"
#include "EthernetInterface.h"

/* Specific includes */

#include "common.h"
#include "watchdog.h"

#include "WDY/fans.h"
#include "WDY/inputs.h"
#include "WDY/adc.h"
#include "WDY/loadpin.h"
#include "WDY/lcd.h"
#include "WDY/eeprom.h"

/* CANopen includes */

#define CO_HAVE_CONFIG // Tell CANopenNode we have a custom config file

extern "C" {
#include "CO_CAN.h"
#include "CO_driver.h"
}

#include "CO_helpers.h"
#include "CO_units.h"

#include "MFE_helpers.h"

#include "WDY/helpers.h"

#define TMR_TASK_INTERVAL   (1000)  // Interval of tmrTask thread in microseconds
#define INCREMENT_1MS(var)  (var++) // Increment 1ms variable in tmrTask

extern DigitalOut  can_led;
extern DigitalOut  err_led;

/* libQEI includes */

#include "QEI.h"



// Encoder
QEI encoder(QEI_INVERT);

// Serial debug port
Serial USBport(USBTX, USBRX);


int main(void);

static void CO_app_task(void);

static void CO_sync_task(void);

static void CO_leds_task(void);

void CO_CANInterruptHandler(void);

/* Tasks */
#include "tasks/artnet.h"
#include "tasks/lcd_ui.h"

#endif /* !MAIN_H_ */
