#include <ble.h>

ret_code_t ble_i2c_init();
void i2c_enable();
void i2c_disable();
ret_code_t i2c_write(uint8_t address, uint8_t *data, uint8_t length);
ret_code_t i2c_read(uint8_t address, uint8_t register_address, uint8_t *buffer, uint8_t length);
void ble_i2c_on_ble_evt(ble_evt_t *);