#include "sensor_timer.h"
#include "nrf_log.h"
#include "feature_config.h"

#define DEBOUNCE_TIMEOUT APP_TIMER_TICKS_COMPAT(GPIO_DEBOUNCE_TIMEOUT_MS, APP_TIMER_PRESCALER)
#define MAX_TICKS 32000

// was using mem_manager and malloc, but there's no point since it reserves the max amount anyway
app_timer_t debounce_timers[GPIO_INPUT_COUNT_MAX];

debounce_timer_timeout_handler_t debounce_timer_timeout_handler;
sequence_timer_handler_t sequence_timer_handler;
uint64_t sequence_timer_remaining_ticks;

APP_TIMER_DEF(sequence_timer);

void debounce_timeout_handler(void *context) {
    debounce_timer_timeout_handler((uint32_t)context);
}

void sensor_timer_debounce_timer_start(uint32_t timer_index) {
    ret_code_t err_code = app_timer_start(
        debounce_timers + timer_index,
        DEBOUNCE_TIMEOUT,
        (void *)timer_index
    );
    APP_ERROR_CHECK(err_code);
}

void sensor_timer_initialize_debounce_timers(uint32_t input_count, debounce_timer_timeout_handler_t timeout_handler) {
    ret_code_t err_code;

    for (uint32_t i = 0; i < input_count; i++) {
        app_timer_id_t timer_id = debounce_timers + i;
        err_code = app_timer_create(
            &timer_id,
            APP_TIMER_MODE_SINGLE_SHOT,
            debounce_timeout_handler
        );
        APP_ERROR_CHECK(err_code);
    }

    debounce_timer_timeout_handler = timeout_handler;
}

void timer_gpioasm_stop() {
    app_timer_stop(sequence_timer);
}

void timer_gpioasm_start_ticks(uint64_t total_ticks, uint64_t *remaining_ticks) {
    uint32_t actual_ticks = MIN(total_ticks, MAX_TICKS);
    *remaining_ticks = total_ticks - actual_ticks;

    uint32_t err_code = app_timer_start(
        sequence_timer,
        actual_ticks,
        NULL
    );
    APP_ERROR_CHECK(err_code);
}

void timer_sequence_timeout_handler(void *context) {
    if (sequence_timer_remaining_ticks > 0) {
        timer_gpioasm_start_ticks(sequence_timer_remaining_ticks, &sequence_timer_remaining_ticks);
        return;
    }
    if (sequence_timer_handler == NULL) {
        return;
    }
    sequence_timer_handler();
}

void timer_sequence_set_timeout_handler(sequence_timer_handler_t timeout_handler) {
    sequence_timer_handler = timeout_handler;
}

void timer_gpioasm_start(uint64_t millis) {
    uint64_t total_ticks = APP_TIMER_TICKS_COMPAT(millis, APP_TIMER_PRESCALER);
    timer_gpioasm_start_ticks(total_ticks, &sequence_timer_remaining_ticks);
}

void timer_init() {
    APP_SCHED_INIT(sizeof(app_timer_event_t), APP_SCHEDULER_QUEUE_SIZE);

    #ifdef NRF51
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
    #else
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    #endif

    err_code = app_timer_create(
        &sequence_timer,
        APP_TIMER_MODE_SINGLE_SHOT,
        timer_sequence_timeout_handler
    );
    APP_ERROR_CHECK(err_code);
}
