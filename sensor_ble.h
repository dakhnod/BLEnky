#ifndef SENSOR_BLE_H
#define SENSOR_BLE_H

#include "ble_conn_params.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "softdevice_handler.h"
#include "sensor_gpio.h"
#include "battery.h"
#include "ble_dfu.h"
#include "app_timer.h"
#include "ble_hci.h"
#include "fstorage.h"
#include "ble_advertising.h"
#include "boards.h"
#include "ble_bss.h"
#include "ble_aio.h"

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/


#define APP_ADV_INTERVAL_FAST           MSEC_TO_UNITS(250, UNIT_0_625_MS)
#define APP_ADV_INTERVAL_SLOW           MSEC_TO_UNITS(1000, UNIT_0_625_MS)

#define APP_ADV_TIMEOUT_FAST_SECS       5
#define APP_ADV_TIMEOUT_SLOW_SECS       0


#define DEVICE_NAME "binary sensor"

#define ADVERTISING_TIMEOUT 0

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(300, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                         /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(1000, UNIT_10_MS)             /**< Connection supervisory time-out (4 seconds). */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */

void on_ble_evt(ble_evt_t * p_ble_evt);
void ble_evt_dispatch(ble_evt_t * p_ble_evt);
void power_manage(void);
void on_conn_params_evt(ble_conn_params_evt_t * p_evt);
void conn_params_error_handler(uint32_t nrf_error);
void conn_params_init(void);
void advertising_stack_init (void);
void advertising_event_handler(ble_adv_evt_t event);
void advertising_data_init (void);
void advertising_init(void);
void advertising_start (void);
void ble_stack_init(void);
void gap_params_init(void);
uint32_t bas_init(void);
uint32_t dfu_init(void);
void services_init(void);
void advertising_stop (void);
void ble_init(void);


#endif
