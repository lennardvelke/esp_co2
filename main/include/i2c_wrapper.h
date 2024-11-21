#ifndef __I2CWRAPPER_H__
#define __I2CWRAPPER_H__

#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include <inttypes.h>

int8_t i2c_write(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size);

int8_t i2c_read(i2c_master_dev_handle_t i2c_dev, uint8_t *read_buffer, size_t read_size);

int8_t i2c_init(const i2c_master_bus_config_t *bus_config, const i2c_device_config_t *dev_config, i2c_master_bus_handle_t *ret_bus_handle, i2c_master_dev_handle_t *ret_handle);

#endif