#include "sensor_timer.h"
#include "mem_manager.h"
#include "nrf_log.h"

#define DEBOUNCE_TIMEOUT APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)

app_timer_t *debounce_timers;


void debounce_timeout_handler(void *context) {
    // uint32_t pin = *((uint32_t *)context);

    NRF_LOG_INFO("debounce timer for pin %x\n", (uint32_t)context);
}


void sensor_timer_initialize_debounce_timers(uint32_t input_count) {
    ret_code_t err_code;
    uint32_t size;

    size = sizeof(app_timer_t) * input_count;
    err_code = nrf_mem_reserve(
        (uint8_t **)(&debounce_timers),
        &size
    );
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("timers size: %d\n", size);

    for (uint32_t i = 0; i < input_count; i++) {
        app_timer_id_t timer_id = debounce_timers + i;
        err_code = app_timer_create(
            &timer_id,
            APP_TIMER_MODE_SINGLE_SHOT,
            debounce_timeout_handler
        );
        APP_ERROR_CHECK(err_code);

        err_code = app_timer_start(
            timer_id,
            DEBOUNCE_TIMEOUT,
            (void *)i
        );
        APP_ERROR_CHECK(err_code);
    }
}

void timer_init() {
    // uint32_t err_code;

    APP_SCHED_INIT(sizeof(app_timer_event_t), APP_SCHEDULER_QUEUE_SIZE);

    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
}
