#include "i2c_wrapper.h"
#include "esp_err.h"

int8_t i2c_write(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size)
{

    ESP_ERROR_CHECK(i2c_master_transmit(i2c_dev, write_buffer, write_size, -1));
    return 0;
}

int8_t i2c_read(i2c_master_dev_handle_t i2c_dev, uint8_t *read_buffer, size_t read_size)
{
    ESP_ERROR_CHECK(i2c_master_receive(i2c_dev, read_buffer, read_size, -1));
    return 0;
}

int8_t i2c_init(const i2c_master_bus_config_t *bus_config, const i2c_device_config_t *dev_config, i2c_master_bus_handle_t *ret_bus_handle, i2c_master_dev_handle_t *ret_handle)
{
    i2c_new_master_bus(bus_config, ret_bus_handle);
    i2c_master_bus_add_device(*ret_bus_handle, dev_config, ret_handle);
    return 0;
}
