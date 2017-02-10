#ifndef CO_UNITS_H_
#define CO_UNITS_H_

#define BDATA_SIZE  4

typedef union {
    uint8_t bytes[BDATA_SIZE];
    float to_float;
    int to_int;
} bdata_t;

#endif // CO_UNITS_H_
