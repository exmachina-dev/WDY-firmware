#ifndef MAIN_H_
#define MAIN_H_

#include "config.h"

/* Mbed includes */
#include "mbed.h"
#include "EthernetInterface.h"

/* Specific includes */

#include "leds.h"
#include "fans.h"
#include "watchdog.h"

/* Artnet includes */

#define HAVE_CONFIG_H // Tell libartnet we have a custom config file

#include "LAN.h"
#include "LAN_common.h"
#include "LAN_packets.h"

/* CANopen includes */

#define CO_HAVE_CONFIG // Tell CANopenNode we have a custom config file

extern "C" {
#include "CO_CAN.h"
#include "CO_driver.h"
}

#include "CO_helpers.h"
#include "CO_units.h"

#include "MFE_helpers.h"

#define TMR_TASK_INTERVAL   (1000)  // Interval of tmrTask thread in microseconds
#define INCREMENT_1MS(var)  (var++) // Increment 1ms variable in tmrTask

struct dmx_device_parameter_s {
    uint8_t command;
    uint16_t position;
    uint16_t speed;
} __attribute__((packed));

typedef struct dmx_device_parameter_s dmx_device_parameter_t;

typedef union {
    uint8_t data[5];
    dmx_device_parameter_t parameter;
} dmx_device_union_t;

int main(void);

static void CO_app_task(void);

static void CO_sync_task(void);

static void CO_timer1ms_task(void);

static void CO_leds_task(void);

void CO_CANInterruptHandler(void);

static void LAN_app_task(void);
static void _dmx_cb(uint16_t port, uint8_t *data);

#endif
