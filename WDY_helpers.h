#ifndef WDY_HELPERS_H_
#define WDY_HELPERS_H_


#include <stdint.h>

#include "CO_units.h"

#define DMX_MAX_VALUE   0xffff  // Value coded on two DMX channels

typedef enum {
    WDY_CMD_NONE =              0x00,
    WDY_CMD_ENABLE =            0x01,
    WDY_CMD_HOME =              0x02,
} wdy_command_t;

typedef enum {
    WDY_STS_NONE =              0x000,
    WDY_STS_ENABLED =           0x001,
    WDY_STS_HOMED =             0x002,
    WDY_STS_HOME_IN_PROGRESS =  0x004,
    WDY_STS_SOFT_LIMIT_FW =     0x008,
    WDY_STS_SOFT_LIMIT_RW =     0x010,
    WDY_STS_HARD_LIMIT_FW =     0x020,
    WDY_STS_HARD_LIMIT_RW =     0x040,
    WDY_STS_UNPOWERED =         0x080,
    WDY_STS_COMM_FAULT =        0x100
} wdy_status_t;

uint8_t map_DMX_to_world(uint16_t DMXvalue, float *mvalue, int maxValue);

uint8_t map_DMXcommand_to_command(uint8_t DMXcommand, wdy_command_t *mcommand);

float length_to_drum_turns(float length);

float length_to_drum_diameter(float length);

float linear_speed_to_rps(float lspeed, float position);

#endif // WDY_HELPERS_H_
