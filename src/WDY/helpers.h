#ifndef WDY_HELPERS_H_
#define WDY_HELPERS_H_


#include <inet.h>
#include <sys/types.h>
#include <stdint.h>

#include "CO_units.h"
#include "AT24Cxx_I2C.h"

#define WDY_EEPROM_DATA_SIZE        (22)
#define WDY_EEPROM_CONF_SIZE        (20)
#define WDY_EEPROM_START_ADDRESS    (1)

#define WDY_EEPROM_VERSION          (1)
#define WDY_EEPROM_STATE_BLANK      (0)
#define WDY_EEPROM_STATE_PRESENT    (1)

#define DMX_MAX_VALUE16     0xffff  // Value coded on two DMX channels
#define DMX_MAX_VALUE8      0xff    // Value coded on two DMX channels

#define CHECK_FLAG(value, flag) ((value & flag) == flag)
#define ADD_FLAG(value, flag) (value | flag)
#define REMOVE_FLAG(value, flag) (value & ~flag)
#define SWITCH_FLAG(value, flag, test) (test) ? ADD_FLAG(value, flag) : REMOVE_FLAG(value, flag)

typedef enum {
    WDY_CMD_NONE =              0x00,
    WDY_CMD_ENABLE =            0x01,
    WDY_CMD_HOME =              0x02,
    WDY_CMD_HOME_ENCODER =      0x04,
    WDY_CMD_CLEAR_ERRORS =      0x08,
    WDY_CMD_RESET =             0x80,
} wdy_command_t;

typedef enum {
    WDY_STS_NONE =              0x00000,
    WDY_STS_ENABLED =           0x00001,
    WDY_STS_HOMED =             0x00002,
    WDY_STS_HOME_IN_PROGRESS =  0x00004,
    WDY_STS_HOME_TIMEOUT =      0x00008,
    WDY_STS_BRAKE_ACTIVE =      0x00010,
    WDY_STS_SOFT_LIMIT_FW =     0x00020,
    WDY_STS_SOFT_LIMIT_RW =     0x00040,
    WDY_STS_HARD_LIMIT_FW =     0x00080,
    WDY_STS_HARD_LIMIT_RW =     0x00100,
    WDY_STS_IS_IDLE =           0x00200,
    WDY_STS_IS_READY =          0x00400,
    WDY_STS_EMERGENCY_STOP =    0x00800,
    WDY_STS_UNDERLOADED =       0x01000,
    WDY_STS_OVERLOADED =        0x02000,
    WDY_STS_UNPOWERED =         0x04000,
    WDY_STS_COMM_FAULT =        0x08000,
    WDY_STS_FLIP_BIT =          0x10000,
} wdy_status_t;

struct wdy_network_config_s {
    bool dhcp;          // 1
    bool static_addr;           // 1
    in_addr ip_addr;            // 4
    in_addr nm_addr;            // 4
    in_addr gw_addr;            // 4
} __attribute__((packed));

typedef wdy_network_config_s wdy_network_config_t;

struct wdy_artnet_config_s {
    uint8_t net;                // 1
    uint8_t subnet;                 // 1
    uint8_t universe;               // 1
    uint8_t dmx_addr;               // 1
} __attribute__((packed));

typedef wdy_artnet_config_s wdy_artnet_config_t;

struct wdy_screen_config_s {
    uint8_t contrast;               // 1
    uint8_t backlight;              // 1
} __attribute__((packed));

typedef wdy_screen_config_s wdy_screen_config_t;

struct wdy_config_s {
    wdy_network_config_t network;
    wdy_artnet_config_t artnet;
    wdy_screen_config_t screen;
} __attribute__((packed));

typedef struct wdy_config_s wdy_config_t;

struct wdy_state_s {
    int8_t init_state;
    uint16_t status;
    float speed;
    float position;
    wdy_config_t config;
} __attribute__((packed));

struct wdy_eeprom_data_s {
    unsigned char eeprom_version;               // 1
    unsigned char eeprom_state;                 // 1
    wdy_config_t config;
} __attribute__((packed));

typedef wdy_eeprom_data_s wdy_eeprom_data_t;

typedef union {
    unsigned char raw[WDY_EEPROM_DATA_SIZE];
    wdy_eeprom_data_t data;
} wdy_eeprom_t;

typedef struct wdy_state_s wdy_state_t;

uint8_t map_DMX16_to_world(uint16_t DMXvalue, float *mvalue, float maxValue);
uint8_t map_DMX16_to_world(uint16_t DMXvalue, float *mvalue, float minValue, float maxValue);


uint8_t map_DMX8_to_world(uint8_t DMXvalue, float *mvalue, float maxValue);
uint8_t map_DMX8_to_world(uint8_t DMXvalue, float *mvalue, float minValue, float maxValue);

uint8_t map_DMXcommand_to_command(uint8_t DMXcommand, wdy_command_t *mcommand);

float map(int from_val1, int from_val2, int to_val1, int to_val2, float value);

float length_to_drum_turns(float length);

float length_to_drum_diameter(float length);

float linear_to_rot(float lvalue, float diameter);

bool WDY_eeprom_read_config(AT24CXX_I2C *eeprom, wdy_eeprom_data_t *config);
bool WDY_eeprom_write_config(AT24CXX_I2C *eeprom, wdy_config_t *config);
bool WDY_eeprom_erase_config(AT24CXX_I2C *eeprom);

#endif // WDY_HELPERS_H_
