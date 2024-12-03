#pragma once

#include <inttypes.h>
#include "device.h"

#typedef struct {
    char[32] device_name,
    uint16_t number_of_diffrent_data_types,
    types_of_data* types,
    uint16_t checksum,
} slave_connect_struct;

#typedef struct {
    uint16_t data_length,
    uint8_t* data,
    uint16_t checksum,
} slave_response_struct;