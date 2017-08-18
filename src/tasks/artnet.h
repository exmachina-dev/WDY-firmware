/*
 * artnet.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TASK_ARTNET_H
#define TASK_ARTNET_H

 

#define HAVE_CONFIG_H // Tell libartnet we have a custom config file

#include "WDY/config.h"


#include "LAN.h"
#include "LAN_common.h"
#include "LAN_packets.h"


struct dmx_device_parameter_s {
    uint8_t command;
    uint16_t position;
    uint16_t speed;
    uint8_t accel;
    uint8_t decel;
} __attribute__((packed));

typedef struct dmx_device_parameter_s dmx_device_parameter_t;

typedef union {
    uint8_t data[WDY_DMX_FOOTPRINT];
    dmx_device_parameter_t parameter;
} dmx_device_union_t;

// DMX device
extern dmx_device_union_t DMXdevice;
extern volatile bool new_dmx_sig;

void LAN_app_task(void);

#endif /* !TASK_ARTNET_H */
