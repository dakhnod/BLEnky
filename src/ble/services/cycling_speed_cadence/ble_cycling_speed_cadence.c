#include "sdk_common.h"
#include "ble_cycling_speed_cadence.h"
#include <string.h>
#include "ble_l2cap.h"
#include "app_error.h"
#include "ble_helpers.h"
#include "sensor_gpio.h"
#include "nrf_log.h"

uint16_t ble_csc_connection_handle = BLE_CONN_HANDLE_INVALID;

uint16_t ble_csc_service_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_csc_measurement_write_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_csc_measurement_cccd_handle = BLE_GATT_HANDLE_INVALID;

uint32_t last_revolution_count = 0;
uint16_t last_revolution_time = 0;

APP_TIMER_DEF(measurement_report_timer);


uint32_t get_rtc1_ticks(){
    return NRF_RTC1->COUNTER;
}

uint16_t get_time_units(){
    return (get_rtc1_ticks() / 32) % 65536;
}

void ble_csc_measurement_report(){
    ret_code_t err_code;

    if(ble_csc_connection_handle == BLE_CONN_HANDLE_INVALID){
        // no client connected
        return;
    }

    uint16_t len = 7;
    static uint8_t buffer[7];
    buffer[0] = CSC_MEAS_FLAG_MASK_WHEEL_REV_DATA_PRESENT;

    // copy wheel revolutions
    memcpy(buffer + 1, &last_revolution_count, 4);

    // copy lasst wheel revolution time
    memcpy(buffer + 5, &last_revolution_time, 2);

    ble_gatts_hvx_params_t params = {
        .handle = ble_csc_measurement_write_handle,
        .type = BLE_GATT_HVX_NOTIFICATION,
        .offset = 0,
        .p_len = &len,
        .p_data = buffer
    };

    err_code = sd_ble_gatts_hvx(
        ble_csc_connection_handle,
        &params);
    APP_ERROR_CHECK(err_code);
}

void measurement_timer_timeout_handler(void *context){
    NRF_LOG_DEBUG("timeout %i!\n", last_revolution_time);

    ble_csc_measurement_report();
}

void ble_csc_timer_init(){
    ret_code_t err_code = app_timer_create(
        &measurement_report_timer,
        APP_TIMER_MODE_REPEATED,
        measurement_timer_timeout_handler
    );
    APP_ERROR_CHECK(err_code);
}

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
    NRF_LOG_DEBUG("csc measurement enabled: %d\n", enabled);

    ret_code_t err_code;

    if(enabled){
        err_code = app_timer_start(
            measurement_report_timer,
            MEASUREMENT_REPORT_INTERVAL,
            NULL
        );
    }else{
        err_code = app_timer_stop(measurement_report_timer);
    }

    APP_ERROR_CHECK(err_code);
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
        7,
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

void ble_csc_handle_sensor_trigger(uint32_t trigger_count){
    last_revolution_count = trigger_count;
    last_revolution_time = get_time_units();

    NRF_LOG_DEBUG("revolutions: %d\n", trigger_count);
}

void ble_csc_handle_input_change(uint32_t index, gpio_config_input_digital_t *config)
{
    if (index != 0)
    {
        return;
    }
    if(config->state == 0){
        return;
    }
    ble_csc_handle_sensor_trigger(config->trigger_count);
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

    ble_csc_timer_init();

    return NRF_SUCCESS;
}
