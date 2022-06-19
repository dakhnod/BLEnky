#include "sensor_timer.h"

APP_TIMER_DEF(debounce_timer);

void (*debounce_timeout_handler_func)();

void debounce_timeout_handler(void * context){
    debounce_timeout_handler_func();
}

void debounce_timer_start(){
	uint32_t err = app_timer_start(debounce_timer, APP_TIMER_TICKS(DEBOUNCE_TIMER_TIMEOUT, APP_TIMER_PRESCALER), NULL);
    NRF_LOG_DEBUG("timer result: %i\n", err);
    APP_ERROR_CHECK(err);
}

void timer_init(void (*debounce_timeout_handler_func_)()){
    uint32_t err_code;
    debounce_timeout_handler_func = debounce_timeout_handler_func_;
	
	APP_SCHED_INIT(sizeof(app_timer_event_t), APP_SCHEDULER_QUEUE_SIZE);
	
	APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
  
    err_code = app_timer_create(&debounce_timer, 
                                         APP_TIMER_MODE_SINGLE_SHOT, 
                                         debounce_timeout_handler);
    APP_ERROR_CHECK(err_code);
}
