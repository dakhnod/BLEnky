#define NRF_LOG_MODULE_NAME "APP"

#include <stdint.h>
#include <string.h>

#include "fstorage.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "sensor_ble.h"
#include "sensor_gpio.h"
#include "sensor_timer.h"

#include "ble_bss.h"

uint16_t event_count = 0;
uint8_t last_contact = false;

void handle_sensor(uint8_t closed) {
    // disable_input();

    enum opening_closing_state_t state = closed ? CLOSED : OPEN;

    if (state == OPEN) {
        event_count++;
    }
    last_contact = closed;

    ble_bss_set_state(state, event_count);

    debounce_timer_start();
}

void handle_debounce_timeout() {
    /*
    NRF_LOG_DEBUG("debounce timeout\n");
    if (contact_get() == last_contact)
    {
        // sensor unchanged
        enable_input();
        return;
    }
    */
    // sensor changed during debounce period
    NRF_LOG_DEBUG("contact changed during debounce\n");
    last_contact = !last_contact;
    handle_sensor(last_contact);
}

int main(void) {
    ret_code_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("starting\n");

    fs_init();

    timer_init(handle_debounce_timeout);

    gpio_init(handle_sensor);

    ble_init();

    uint8_t storage_data[16];
    gpio_pin_configuration_data_read(storage_data);
    err_code = ble_aio_pin_configuraion_data_set(storage_data, 16);
    APP_ERROR_CHECK(err_code);

    advertising_start();

    NRF_LOG_INFO("setup done\n");

    // Enter main loop.
    for (;;) {
        if (NRF_LOG_PROCESS() == false) {
            power_manage();
            app_sched_execute();
        }
    }
}
