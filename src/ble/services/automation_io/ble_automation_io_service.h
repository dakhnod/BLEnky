#ifndef BLE_AIO_H
#define BLE_AIO_H

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_log.h"
#include "sensor_gpio.h"
// #include "nrf_ble_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UUID_AUTOMATION_IO_SERVICE         0x1815
#define UUID_DIGITAL_CHARACTERISTIC        0x2A56
#define UUID_DIGITAL_CHARACTERISTIC_OUTPUT 0x2A57
#define UUID_ANALOG_CHARACTERISTIC         0x2A58
#define UUID_PIN_CONFIG_CHARACTERISTIC     0x0001
#define UUID_BLE_CONFIG_CHARACTERISTIC     0x0002
#define UUID_GPIO_ASM_CHARACTERISTIC       0x0003

    uint32_t ble_aio_init();


    void ble_aio_on_ble_evt(ble_evt_t *p_ble_evt);

    void ble_aio_on_authorize(ble_evt_t *p_ble_evt);
    void encode_states_to_bytes(uint8_t *states, uint32_t state_count, uint8_t *buffer, uint16_t buffer_len);
    uint32_t ble_aio_get_byte_count_from_pins(uint32_t pin_count);
    void ble_aio_handle_input_change(uint32_t index, gpio_config_input_digital_t *config);
    void ble_aio_handle_pin_digital_data(uint8_t *pin_data, uint32_t pin_data_length);
    void ble_aio_handle_pin_analog_data(uint32_t index, uint16_t duty_cycle);

#ifdef __cplusplus
}
#endif

#endif // BLE_AIO_H

/** @} */
