#ifndef SENSOR_TIMER_H
#define SENSOR_TIMER_H

#include "app_timer.h"
#include "app_timer_appsh.h"
#include "app_scheduler.h"
#include "nrf_log.h"

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         6                                 /**< Size of timer operation queues. */
#define APP_SCHEDULER_QUEUE_SIZE 				6

#define DEBOUNCE_TIMER_TIMEOUT 2000

void timer_init(void (*)());
void debounce_timer_start(void);

#endif
