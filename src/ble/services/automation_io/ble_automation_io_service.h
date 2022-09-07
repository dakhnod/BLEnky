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
#define UUID_ANALOG_CHARACTERISTIC      0x2A58
#define UUID_COMBINED_SEQUENCE           0x0056

    uint32_t ble_aio_init();


    void ble_aio_on_ble_evt(ble_evt_t *p_ble_evt);
    void ble_aio_authorize_digital_out();

    void ble_aio_on_authorize(ble_evt_t *p_ble_evt);
    void encode_states_to_bytes(uint8_t *states, uint32_t state_count, uint8_t *buffer);
    uint32_t ble_aio_get_byte_count_from_pins(uint32_t pin_count);

#ifdef __cplusplus
}
#endif

#endif // BLE_AIO_H

/** @} */
