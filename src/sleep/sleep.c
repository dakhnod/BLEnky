#include "feature_config.h"
#if FEATURE_ENABLED(SLEEP_MODE)

#include "sleep.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "ble_advertising.h"
#include "ble_hci.h"
#include "sensor_timer.h"
#include "softdevice_handler.h"

// Tick every minute and check the timeout
// if this is changed the timeout will be too early / late
// since the timeout is specified in minutes
#define TICK_TIMEOUT_MS 60 * 1000

#define SLEEP_TIMER_TIMEOUT APP_TIMER_TICKS(TICK_TIMEOUT_MS, APP_TIMER_PRESCALER)

APP_TIMER_DEF(sleep_timeout_timer);

bool sleep_allow_advertise = true;

uint32_t inactivity_count = 0;

bool timer_running = false;

sleep_enter_handler_t sleep_enter_handler;

void sleep_timer_stop(){
    if(!timer_running){
        return;
    }
    timer_running = false;
    ret_code_t err_code = app_timer_stop(sleep_timeout_timer);
    APP_ERROR_CHECK(err_code);
}

void sleep_timer_start(){
    if(timer_running){
        return;
    }
    timer_running = true;
    ret_code_t err_code = app_timer_start(
        sleep_timeout_timer,
        SLEEP_TIMER_TIMEOUT,
        NULL
    );
    APP_ERROR_CHECK(err_code);
}

void sleep_timeout_handler(void *context){
    if(inactivity_count < SLEEP_TIMEOUT_MINUTES){
        inactivity_count++;
        NRF_LOG_DEBUG("new inactivity count: %i\n", inactivity_count);
        return;
    }
#if SLEEP_MODE == SLEEP_MODE_SYSTEM_ON
    NRF_LOG_DEBUG("going to light sleep...\n");
    // stop watchdog timer since we're sleeping anyways
    sleep_timer_stop();
    sleep_allow_advertise = false;
    sleep_enter_handler();
#else
    NRF_LOG_DEBUG("going to deep sleep...\n");
    // turning chip and clock off, GPIO should be set up to wake up again
    sd_power_system_off();
#endif
}

void sleep_handle_gpio_event(){
    inactivity_count = 0;
    sleep_allow_advertise = true;
    // ensure that the watchdog is active
    sleep_timer_start();
}

bool sleep_get_allow_advertise(){
    return sleep_allow_advertise;
}

void sleep_init(sleep_enter_handler_t sleep_enter_handler_){
    ret_code_t err_code;

    sleep_enter_handler = sleep_enter_handler_;

    err_code = app_timer_create(
        &sleep_timeout_timer,
        APP_TIMER_MODE_REPEATED,
        sleep_timeout_handler
    );
    APP_ERROR_CHECK(err_code);

    sleep_timer_start();
}

#endif