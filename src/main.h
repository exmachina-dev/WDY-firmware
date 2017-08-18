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

/* Artnet includes */

#define HAVE_CONFIG_H // Tell libartnet we have a custom config file

#include "LAN.h"
#include "LAN_common.h"
#include "LAN_packets.h"

#define DMX_FOOTPRINT   7

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

struct dmx_device_parameter_s {
    uint8_t command;
    uint16_t position;
    uint16_t speed;
    uint8_t accel;
    uint8_t decel;
} __attribute__((packed));

typedef struct dmx_device_parameter_s dmx_device_parameter_t;

typedef union {
    uint8_t data[DMX_FOOTPRINT];
    dmx_device_parameter_t parameter;
} dmx_device_union_t;

int main(void);

static void CO_app_task(void);

static void CO_sync_task(void);

static void CO_leds_task(void);

void CO_CANInterruptHandler(void);

static void LAN_app_task(void);
static void _dmx_cb(uint16_t port, uint8_t *data);
/* Tasks */
#include "tasks/lcd_ui.h"

#endif /* !MAIN_H_ */
