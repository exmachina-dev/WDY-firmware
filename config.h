#ifndef CONFIG_H_
#define CONFIG_H_

// LightArtNET config
// =============

// Manufacturer and product constants
#define AN_OEM_LO   0x32    // Winch Dynamic by ExMachina
#define AN_OEM_HI   0x28    // Winch Dynamic by ExMachina

#define AN_ESTA_LO  0x00
#define AN_ESTA_HI  0x00

// #define ARTNET_PRINTF USBport->printf

// CANopen config
// ==============

#define MBED_CAN            (0)
#define CO_BUS_BITRATE      500
#define CO_NODEID           0x01
#define CO_DRV_NODEID       0x02

// Product config
// ==============

#define WDY_SHORT_NAME  "Winch Dynamic"
#define WDY_LONG_NAME   "Winch Dynamic by ExMachina"

#endif
