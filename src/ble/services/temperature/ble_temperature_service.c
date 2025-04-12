#include "sdk_common.h"
#include "ble_automation_io_service.h"
#include <string.h>
#include "ble_l2cap.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "ble_helpers.h"
#include "ble_temperature_service.h"
#include "ble.h"
#include "nrf_soc.h"

uint16_t ble_temperature_connection_handle = BLE_CONN_HANDLE_INVALID;

uint16_t ble_temperature_service_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_temperature_characteristic_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_temperature_cccd_handle = BLE_GATT_HANDLE_INVALID;

void ble_temperature_on_connect(const ble_evt_t *p_ble_evt)
{
    ble_temperature_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

void ble_temperature_on_disconnect(const ble_evt_t *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    ble_temperature_connection_handle = BLE_CONN_HANDLE_INVALID;
}

void ble_temperature_on_cccd_write(ble_gatts_evt_write_t *write_evt)
{
    if (write_evt->len == 2)
    {
        // ble_temperature_send_digital_input_updates = ble_srv_is_notification_enabled(write_evt->data);
    }
}

ret_code_t ble_temperature_characteristic_add()
{
    ble_helper_characteristic_init_t init = {
        .service_handle = ble_temperature_service_handle,
        .uuid = UUID_TEMPERATURE_CHARACTERISTIC,
        .is_readable = true,
        .authorize_read = true,
        .max_length = 2,
        .value_handle = &ble_temperature_characteristic_handle,
        .description_str = "Chip temperature, inaccurate"
    };
    return ble_helper_characteristic_add(&init);
}

void ble_temperature_authorize_temperature(const ble_evt_t *p_ble_evt){
    const ble_gatts_evt_rw_authorize_request_t *req = &(p_ble_evt
                                                      ->evt.gatts_evt
                                                      .params
                                                      .authorize_request);

    if (req->type != BLE_GATTS_AUTHORIZE_TYPE_READ){
        return;
    }

    uint16_t handle = req
                        ->request
                        .read
                        .handle;

    if (handle != ble_temperature_characteristic_handle){
        return;
    }
    
    int32_t raw_value;
    sd_temp_get(&raw_value);

    int16_t calculated = (raw_value * 10) / 4;

    ble_gatts_rw_authorize_reply_params_t authorize_params = {
        .type = BLE_GATTS_AUTHORIZE_TYPE_READ,
        .params.read = {
            .gatt_status = BLE_GATT_STATUS_SUCCESS,
            .update = 1,
            .offset = 0,
            .len = 2,
            .p_data = (uint8_t*) &calculated
        }
    };

    sd_ble_gatts_rw_authorize_reply(
        ble_temperature_connection_handle,
        &authorize_params
    );
}

void ble_temperature_on_ble_evt(const ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        ble_temperature_on_connect(p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        ble_temperature_on_disconnect(p_ble_evt);
        break;

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        ble_temperature_authorize_temperature(p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        // ble_temperature_on_write(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}

ret_code_t ble_temperature_init()
{
    ret_code_t err_code;
    ble_uuid_t ble_uuid;

    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_TEMPERATURE_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_temperature_service_handle);
    APP_ERROR_CHECK(err_code);

    ble_temperature_characteristic_add();

    return NRF_SUCCESS;
}
