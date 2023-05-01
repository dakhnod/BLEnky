#include "ble_hid.h"
#include "sdk_common.h"
#include <string.h>
#include "ble_l2cap.h"
#include "app_error.h"
#include "ble_helpers.h"
#include "sensor_gpio.h"
#include "nrf_log.h"

uint16_t ble_hid_service_handle;

uint16_t ble_hid_characteristic_information_handle;

ret_code_t ble_hid_characteristic_informatioin_add()
{
    return ble_helper_characteristic_add(
        ble_hid_service_handle,
        BLE_UUID_HID_CHARACTERISTIC_INFORMATION,
        BLE_UUID_TYPE_BLE,
        "HID information",
        false,
        true,
        false,
        false,
        false,
        0,
        NULL,
        NULL);
}

ret_code_t ble_hid_init()
{
    ret_code_t err_code;
    ble_uuid_t ble_uuid;

    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_HID_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_hid_service_handle);
    APP_ERROR_CHECK(err_code);

    err_code = ble_hid_characteristic_informatioin_add();
    APP_ERROR_CHECK(err_code);

    return NRF_SUCCESS;
}