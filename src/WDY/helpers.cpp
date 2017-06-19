#include "mbed.h"
#include <sys/types.h>
#include <stdint.h>
#include <math.h>

#include "helpers.h"
#include "config.h"

#define _PI             WDY_PI
#define _S_THK          WDY_STRAP_THICKNESS
#define _D_DIA          WDY_DRUM_CORE_DIA

uint8_t map_DMX16_to_world(uint16_t DMXvalue, float *mvalue, int maxValue) {
    float _value;

    _value = (float)(DMXvalue / (float)DMX_MAX_VALUE16) * (float)maxValue;

    memcpy(mvalue, &_value, sizeof(float));

    return 0;
}

uint8_t map_DMX8_to_world(uint8_t DMXvalue, float *mvalue, int maxValue) {
    float _value;

    _value = (float)(DMXvalue / (float)DMX_MAX_VALUE8) * (float)maxValue;

    memcpy(mvalue, &_value, sizeof(float));

    return 0;
}

uint8_t map_DMXcommand_to_command(uint8_t DMXcommand, wdy_command_t *mcommand) {
    wdy_command_t _cmd;

    _cmd = (wdy_command_t)(DMXcommand / WDY_COMMAND_SLOTS);

    memcpy(mcommand, &_cmd, sizeof(uint8_t));

    return 0;
}

float map(int from_val1, int from_val2, int to_val1, int to_val2, float value) {
    float res = 0.0;
    res = to_val1 + (float) ((value-from_val1)/(from_val2-from_val1)) * (float) (to_val2-to_val1);

    // Don't clamp to to_val1 and to_val2, check to_val2 < res < to_val1 also
    if (res > (float) to_val1 && res > (float) to_val2) {
        if ((float) to_val2 > (float) to_val1)
            return (float) to_val2;
        else
            return (float) to_val1;
    }
    else if (res < (float) to_val2 && res < (float) to_val1) {
        if ((float) to_val2 < (float) to_val1)
            return (float) to_val2;
        else
            return (float) to_val1;
    }

    return res;
}

float length_to_drum_turns(float length) {
    float Nturns;

    Nturns = (_S_THK - _D_DIA + sqrt(
                pow(_D_DIA - _S_THK, 2) + ((4 * _S_THK * (
                            (WDY_MAX_POSITION - length))) / _PI))) /
        (2 * _S_THK);

    return Nturns;
}

float length_to_drum_diameter(float length) {
    float Ddiameter;

    Ddiameter = 2 * length_to_drum_turns(length) * _S_THK + _D_DIA;

    return Ddiameter;
}

float linear_to_rot(float lvalue, float diameter) {
    float perimeter;
    float rot;

    perimeter = diameter * _PI;
    rot = lvalue / perimeter;

#ifdef WDY_GEARBOX_RATIO
    return rot * WDY_GEARBOX_RATIO;
#else
    return rot;
#endif
}

Mutex wdy_eeprom_mutex;

bool WDY_eeprom_read_config(AT24CXX_I2C *eeprom, wdy_eeprom_data_t *data) {
    wdy_eeprom_t eedata;

    wdy_eeprom_mutex.lock();
    bool ret = eeprom->read(WDY_EEPROM_START_ADDRESS, eedata.raw, WDY_EEPROM_DATA_SIZE);
    memcpy(data, &eedata.data, WDY_EEPROM_CONF_SIZE);
    wdy_eeprom_mutex.unlock();

    return ret;
}

bool WDY_eeprom_write_config(AT24CXX_I2C *eeprom, wdy_config_t *config) {
    wdy_eeprom_t eedata;
    memcpy(&eedata.data.config, config, WDY_EEPROM_DATA_SIZE);
    eedata.data.eeprom_version = WDY_EEPROM_VERSION;
    eedata.data.eeprom_state = WDY_EEPROM_STATE_PRESENT;

    wdy_eeprom_mutex.lock();
    bool ret = eeprom->write(WDY_EEPROM_START_ADDRESS, eedata.raw, WDY_EEPROM_DATA_SIZE);
    for (uint8_t i = 0; i<WDY_EEPROM_DATA_SIZE; i++) {
        printf("%d ", eedata.raw[i]);
    }
    wdy_eeprom_mutex.unlock();

    return ret;
}

bool WDY_eeprom_erase_config(AT24CXX_I2C *eeprom) {
    bool ret;

    ret = eeprom->erase_memory(WDY_EEPROM_START_ADDRESS, WDY_EEPROM_DATA_SIZE);

    wdy_eeprom_t eedata;
    wdy_config_t config;
    eedata.data.eeprom_version = WDY_EEPROM_VERSION;
    eedata.data.eeprom_state = WDY_EEPROM_STATE_BLANK;

    memcpy(&eedata.data.config, &config, WDY_EEPROM_CONF_SIZE);

    ret = eeprom->write(WDY_EEPROM_START_ADDRESS, eedata.raw, WDY_EEPROM_DATA_SIZE);

    return ret;
}
