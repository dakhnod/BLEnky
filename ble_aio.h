#ifndef BLE_AIO_H
#define BLE_AIO_H

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_log.h"
#include "sensor_gpio.h"
#include "sequence.h"
// #include "nrf_ble_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UUID_AUTOMATION_IO_SERVICE       0x1815
#define UUID_PIN_CONFIG_CHARACTERISTIC   0x0001
#define UUID_DIGITAL_CHARACTERISTIC      0x2A56


#define CUSTOM_UUID_BASE {0xdc, 0x71, 0xd1, 0xc1, 0xfd, 0x01, 0x49, 0x15, 0xa7, 0x8f, 0xf1, 0x5c, 0x00, 0x00, 0x10, 0x9c}


    uint32_t ble_aio_init();


    void ble_aio_on_ble_evt(ble_evt_t *p_ble_evt);

    ret_code_t ble_aio_characteristic_digital_add(
        uint16_t uuid,
        char *description_str,
        uint8_t number_of_digitals,
        uint8_t description,
        uint8_t is_writable,
        uint8_t is_readable,
        uint8_t is_notifiable,
        uint8_t uuid_type,
        uint16_t max_length,
        uint16_t *value_handle,
        uint16_t *cccd_handle
    );
    void ble_aio_authorize_digital_out();

    void ble_aio_on_authorize(ble_evt_t *p_ble_evt);
    void encode_states_to_bytes(uint8_t *states, uint32_t state_count, uint8_t *buffer);
    uint32_t ble_aio_get_byte_count_from_pins(uint32_t pin_count);

#ifdef __cplusplus
}
#endif

#endif // BLE_AIO_H

/** @} */
