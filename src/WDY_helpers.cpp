#include "mbed.h"
#include <sys/types.h>
#include <stdint.h>
#include <math.h>

#include "WDY_helpers.h"
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

    _cmd = (wdy_command_t)(DMXcommand / 16);

    memcpy(mcommand, &_cmd, sizeof(uint8_t));

    return 0;
}

float length_to_drum_turns(float length) {
    float Nturns;

    Nturns = (_S_THK - _D_DIA + sqrt(
                pow(_D_DIA - _S_THK, 2) + ((4 * _S_THK * (
                            (WDY_MAX_POSITION - length))) / _PI))) /
        (2 * _S_THK) + WDY_DRUM_OFFSET_TURNS;

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
