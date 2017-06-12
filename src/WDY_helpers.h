#ifndef WDY_HELPERS_H_
#define WDY_HELPERS_H_


#include <inet.h>
#include <sys/types.h>
#include <stdint.h>

#include "CO_units.h"

#define DMX_MAX_VALUE16     0xffff  // Value coded on two DMX channels
#define DMX_MAX_VALUE8      0xff    // Value coded on two DMX channels

typedef enum {
    WDY_CMD_NONE =              0x00,
    WDY_CMD_ENABLE =            0x01,
    WDY_CMD_HOME =              0x02,
    WDY_CMD_HOME_ENCODER =      0x04,
    WDY_CMD_CLEAR_ERRORS =      0x08,
    WDY_CMD_RESET =             0x80,
} wdy_command_t;

typedef enum {
    WDY_STS_NONE =              0x000,
    WDY_STS_ENABLED =           0x001,
    WDY_STS_HOMED =             0x002,
    WDY_STS_HOME_IN_PROGRESS =  0x004,
    WDY_STS_HOME_TIMEOUT =      0x008,
    WDY_STS_BRAKE_ACTIVE =      0x010,
    WDY_STS_SOFT_LIMIT_FW =     0x020,
    WDY_STS_SOFT_LIMIT_RW =     0x040,
    WDY_STS_HARD_LIMIT_FW =     0x080,
    WDY_STS_HARD_LIMIT_RW =     0x100,
    WDY_STS_UNPOWERED =         0x200,
    WDY_STS_COMM_FAULT =        0x400,
    WDY_STS_FLIP_BIT =          0x800,
} wdy_status_t;

struct wdy_network_config_s {
    bool dhcp;
    in_addr ip_addr;
    in_addr nm_addr;
    in_addr gw_addr;
};

typedef wdy_network_config_s wdy_network_config_t;

struct wdy_artnet_config_s {
    uint8_t net;
    uint8_t subnet;
    uint8_t universe;
    uint8_t dmx_addr;
};

typedef wdy_artnet_config_s wdy_artnet_config_t;

struct wdy_screen_config_s {
    uint8_t contrast;
    uint8_t backlight;
};

typedef wdy_screen_config_s wdy_screen_config_t;

struct wdy_config_s {
    wdy_network_config_t network;
    wdy_artnet_config_t artnet;
    wdy_screen_config_t screen;
};

typedef struct wdy_config_s wdy_config_t;

struct wdy_state_s {
    int8_t init_state;
    uint16_t status;
    wdy_config_t config;
};

typedef struct wdy_state_s wdy_state_t;

uint8_t map_DMX16_to_world(uint16_t DMXvalue, float *mvalue, int maxValue);

uint8_t map_DMX8_to_world(uint8_t DMXvalue, float *mvalue, int maxValue);

uint8_t map_DMXcommand_to_command(uint8_t DMXcommand, wdy_command_t *mcommand);

float length_to_drum_turns(float length);

float length_to_drum_diameter(float length);

float linear_to_rot(float lvalue, float diameter);

#endif // WDY_HELPERS_H_
