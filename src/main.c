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
#include "sleep.h"
#include "feature_config.h"
#include "watchdog.h"
#include "bma400.h"

void main_handle_input_change(int highest_changed_index)
{
    #if FEATURE_ENABLED(SLEEP_MODE)
        sleep_handle_gpio_event();
    #endif
    ble_handle_input_change(highest_changed_index);
}

int main(void) {
    (void) NRF_LOG_INIT(NULL);

    NRF_LOG_INFO("starting\n");

    ble_stack_init();

    fs_init();

    timer_init();

    #if FEATURE_ENABLED(SLEEP_MODE)
    sleep_init(ble_disable_rf);
    #endif

    gpio_init(main_handle_input_change);

    ble_init();

    advertising_start();

    ret_code_t err_code = bma400_setup_orientation_detection(main_handle_input_change);
    NRF_LOG_DEBUG("orientation: %d\n", err_code);

    watchdog_init();

    NRF_LOG_INFO("setup done\n");

    // Enter main loop.
    for (;;) {
        // if (NRF_LOG_PROCESS() == false) {
            power_manage();
            watchdog_feed();
            app_sched_execute();
        // }
    }
}
