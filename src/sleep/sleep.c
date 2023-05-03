#include "sleep.h"
#include "app_timer.h"
#include "feature_config.h"
#include "nrf_log.h"
#include "ble_advertising.h"

#define SLEEP_TIMER_TIMEOUT APP_TIMER_TICKS(60 * 1000, APP_TIMER_PRESCALER)

APP_TIMER_DEF(sleep_timeout_timer);

bool sleep_allow_advertise = true;

uint16_t sleep_connection_handle;

uin32_t inactivity_count = 0;

void sleep_timeout_handler(void *context){
    if(inactivity_count < SLEEP_TIMEOUT_MINUTES){
        inactivity_count++;
        NRF_LOG_DEBUG("new inactivity count: %i\n", inactivity_count);
        return;
    }
#if SLEEP_MODE == SLEEP_MODE_SYSTEM_ON
    NRF_LOG_DEBUG("going to light sleep...\n");
    sleep_allow_advertise = false;
    if(conn_handle != BLE_CONN_HANDLE_INVALID){
        sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    }
    ble_advertising_stop();
#else
    NRF_LOG_DEBUG("going to deep sleep...\n");
    // turning chip and clock off, GPIO should be set up to wake up again
    NVIC_SystemOff();
#endif
}

void sleep_handle_gpio_event(uint32_t index, gpio_config_input_digital_t *input){
    inactivity_count = 0;
}

void sleep_handle_ble_evt(ble_evt_t *p_ble_evt){
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            sleep_connection_handle = p_ble_evt->p_ble_evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            sleep_connection_handle = BLE_CONN_HANDLE_INVALID;
            break;
    }
}

bool sleep_allow_advertise(){
    return sleep_allow_advertise;
}

void sleep_init(){
    ret_code_t err_code;

    err_code = app_timer_create(
        &sleep_timeout_timer,
        APP_TIMER_MODE_REPEATED,
        sleep_timeout_handler
    );
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(
        sleep_timeout_timer,
        SLEEP_TIMER_TIMEOUT,
        NULL
    );
    APP_ERROR_CHECK(err_code);
}