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

int main(void) {
    ret_code_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("starting\n");

    fs_init();

    timer_init();

    gpio_init();

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
