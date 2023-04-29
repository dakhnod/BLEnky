#include "sdk_common.h"
#include "ble_cycling_speed_cadence.h"
#include <string.h>
#include "ble_l2cap.h"
#include "app_error.h"
#include "ble_helpers.h"
#include "app_timer.h"
#include "sensor_gpio.h"

uint16_t ble_csc_connection_handle = BLE_CONN_HANDLE_INVALID;

uint16_t ble_csc_service_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_csc_measurement_write_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_csc_measurement_cccd_handle = BLE_GATT_HANDLE_INVALID;

void ble_csc_on_connect(ble_evt_t *p_ble_evt)
{
    ble_csc_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

void ble_csc_on_disconnect(ble_evt_t *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    ble_csc_connection_handle = BLE_CONN_HANDLE_INVALID;
}

void handle_speed_measurement_notification_enabled(uint8_t enabled){

}

void handle_csc_measurement_cccd_write(ble_gatts_evt_write_t *write_evt)
{
    if (write_evt->len == 2)
    {
        // TODO enabke update timer here
        handle_speed_measurement_notification_enabled(
            ble_srv_is_notification_enabled(write_evt->data)
        );
    }
}

ret_code_t ble_csc_characteristic_measurement_add()
{
    return ble_helper_characteristic_add(
        ble_csc_service_handle,
        UUID_CSC_CHARACTERISTIC_SPEED_MEASUREMENT,
        BLE_UUID_TYPE_BLE,
        "Speed measurement",
        false,
        false,
        true,
        false,
        false,
        0,
        &ble_csc_measurement_write_handle,
        &ble_csc_measurement_cccd_handle);
}



void ble_csc_on_write(ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t *write_evt = &p_ble_evt
                                            ->evt
                                            .gatts_evt
                                            .params
                                            .write;

    uint16_t handle = write_evt->handle;

    if (handle == ble_csc_measurement_cccd_handle)
    {
        handle_csc_measurement_cccd_write(write_evt);
        return;
    }
}

void ble_csc_on_ble_evt(ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        ble_csc_on_connect(p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        ble_csc_on_disconnect(p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        ble_csc_on_write(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}

void ble_csc_handle_input_change(uint32_t index, gpio_config_input_digital_t *config)
{
    if (index == 0)
    {
        // ble_bss_set_state(config->state, (uint16_t)config->trigger_count);
    }
}

ret_code_t ble_csc_init()
{
    ret_code_t err_code;
    ble_uuid_t ble_uuid;

    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_CSC_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_csc_service_handle);
    APP_ERROR_CHECK(err_code);

    err_code = ble_csc_characteristic_measurement_add();
    APP_ERROR_CHECK(err_code);

    return NRF_SUCCESS;
}
