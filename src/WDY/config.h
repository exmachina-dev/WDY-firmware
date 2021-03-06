#ifndef CONFIG_H_
#define CONFIG_H_

// #define MBED_CONF_APP_MEMTRACE

// Global behaviour settings
// -------------------------
#define WDY_DEBUG           (1)

// Math constants
// --------------
#define WDY_PI                  3.1415926535

// LightArtNET config
// =============

// Manufacturer and product constants
#define LAN_OEM_CODE_LO   0x32    // Winch Dynamic by ExMachina
#define LAN_OEM_CODE_HI   0x28    // Winch Dynamic by ExMachina

#define LAN_ESTA_CODE_LO  0x78
#define LAN_ESTA_CODE_HI  0x45

#define WDY_DMX_FOOTPRINT   7

// #define ARTNET_PRINTF USBport->printf

// CANopen config
// ==============

#define MBED_CAN            (0)
#define CO_BUS_BITRATE      1000
#define CO_NODEID           0x01
#define CO_DRV_NODEID       0x02

// Product config
// ==============

#define WDY_SHORT_NAME  "WDY3000"
#define WDY_LONG_NAME   "Winch Dynamic by ExMachina"

#define WDY_LOOP_INTERVAL       10      // ms

#define WDY_COMMAND_SLOTS       16      // divide 255 by 16
#define WDY_INVERT_POS_COMMAND  true    // invert position

// Physic constants
// ----------------
#define WDY_STRAP_LENGTH        30000   // mm
#define WDY_STRAP_THICKNESS     1.1     // mm

#define WDY_DRUM_OFFSET_TURNS   1.97     // turns - This is the minimum of turns
                                        // rolled up onto the drum
#define WDY_DRUM_CORE_DIA       120 + (WDY_DRUM_OFFSET_TURNS * 2 * WDY_STRAP_THICKNESS)      // mm

#define WDY_STRAP_DEAD_OFFSET   765     // mm - This is the length always rolled up onto the drum
#define WDY_STRAP_FIXED_OFFSET  428     // mm - This is the length of the strap path

#define WDY_GEARBOX_RATIO       6       // The gearbox ratio between the motor and the drum

#define WDY_DEFAULT_ACCEL       500     // mm.s-1
#define WDY_DEFAULT_DECEL       500     // mm.s-1

// Limits
// ------
#define WDY_MAX_SPEED           3000    // mm.s
#define WDY_MAX_POSITION        WDY_STRAP_LENGTH
#define WDY_MIN_POSITION        0

#define WDY_MAX_ACCEL           3000    // mm.s-1
#define WDY_MAX_DECEL           3000    // mm.s-1

#define WDY_MAX_HOMING_TIME     35000   // ms


// Encoder
// -------
#define WDY_ENCODER_VFQ         (30)    // Lower value means the speed is
                                        // calculated over a longer time interval
                                        // This may help reduce noise

#define WDY_ENCODER_PPR         1000    // 500ppr * 4 edges
#define WDY_ENCODER_WHEEL_DIA   16.6    // mm

// This the factor in order to convert rps to mm/s
#define WDY_ENCODER_FACTOR      (WDY_STRAP_THICKNESS + WDY_ENCODER_WHEEL_DIA) * WDY_PI

#define WDY_ENCODER_HOME_OFFSET 0.0

#define WDY_INVERT_POSITION     1

#endif
