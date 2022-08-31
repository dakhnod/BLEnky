#ifndef SENSOR_TIMER_H
#define SENSOR_TIMER_H

#include "app_timer.h"
#include "app_timer_appsh.h"
#include "app_scheduler.h"

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         6                                 /**< Size of timer operation queues. */
#define APP_SCHEDULER_QUEUE_SIZE 				6

#define DEBOUNCE_TIMER_TIMEOUT 2000

typedef void (*debounce_timer_timeout_handler_t)(uint32_t timer_index);

void timer_init(void);
void sensor_timer_debounce_timer_start(uint32_t timer_index);
void sensor_timer_initialize_debounce_timers(uint32_t input_count, debounce_timer_timeout_handler_t timeout_handler);
#endif
