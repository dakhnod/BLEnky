#include "sensor_timer.h"

#define APP_ADV_INTERVAL_FAST           MSEC_TO_UNITS(250, UNIT_0_625_MS)
#define APP_ADV_INTERVAL_SLOW           MSEC_TO_UNITS(1000, UNIT_0_625_MS)

#define APP_ADV_TIMEOUT_FAST_SECS       5
#define APP_ADV_TIMEOUT_SLOW_SECS       0


#define DEVICE_NAME "binary sensor"

#define BLE_DEFAULT_MIN_CONN_INTERVAL               MSEC_TO_UNITS(10, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define BLE_DEFAULT_MAX_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (1 second). */
#define BLE_DEFAULT_SLAVE_LATENCY                   0                                         /**< Slave latency. */
#define BLE_DEFAULT_CONN_SUP_TIMEOUT                MSEC_TO_UNITS(6000, UNIT_10_MS)             /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */