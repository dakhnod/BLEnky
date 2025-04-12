#ifndef ble_temperature_H
#define ble_temperature_H

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
// #include "nrf_ble_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UUID_TEMPERATURE_SERVICE         0x181A
#define UUID_TEMPERATURE_CHARACTERISTIC  0x2A1F

    uint32_t ble_temperature_init();

    void ble_temperature_on_ble_evt(const ble_evt_t *p_ble_evt);
#ifdef __cplusplus
}
#endif

#endif // ble_temperature_H

/** @} */
