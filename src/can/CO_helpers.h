#ifndef CO_HELPERS_
#define CO_HELPERS_

#include "mbed.h"

#include "config.h"

extern "C" {
#include "CANopen.h"
#include "CO_SDO.h"
#include "CO_SDOmaster.h"
}

extern Timer    CO_timer;    /* from main */

extern CO_t *CO;

uint32_t swapBytes32(uint32_t from);

uint16_t swapBytes16(uint16_t from);

int CO_SDO_write(uint8_t remoteNodeId, uint16_t index, uint8_t subIndex,
        uint8_t *dataTx, uint32_t dataSize, uint32_t *SDOabortCode, uint16_t SDOtimeoutTime);

int CO_SDO_read(uint8_t remoteNodeId, uint16_t index, uint8_t subIndex, uint8_t *dataRx,
        uint32_t dataSize, uint32_t *SDOabortCode, uint32_t *dataReadSize,
        uint16_t SDOtimeoutTime);

#endif
