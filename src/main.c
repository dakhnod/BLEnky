#define NRF_LOG_MODULE_NAME "APP"

#include <stdint.h>
#include <string.h>

#include "fstorage.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "sensor_ble.h"
#include "sensor_gpio.h"
#include "sensor_timer.h"
#include "ble_configuration_service.h"

int main(void) {
    ret_code_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("starting\n");

    fs_init();

    timer_init();

    gpio_init();

    ble_init();

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
