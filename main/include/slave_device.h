#pragma once

#include "device.h"
#include <inttypes.h>

typedef struct
{
    uint8_t name[32];
    uint16_t send_intervall;
} slave;

typedef struct
{
    uint8_t device_name[32];
    uint16_t checksum;
} slave_connect_struct;

typedef struct
{
    uint32_t data;
    uint16_t checksum;
} slave_response_struct;