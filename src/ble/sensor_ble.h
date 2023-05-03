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
#include "ble_binary_sensor_service.h"
#include "ble_automation_io_service.h"
#include "ble_configuration.h"
#include "storage.h"

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/


/**< Value of the RTC1 PRESCALER register. */

void on_ble_evt(ble_evt_t *p_ble_evt);
void ble_evt_dispatch(ble_evt_t *p_ble_evt);
void power_manage(void);
void on_conn_params_evt(ble_conn_params_evt_t *p_evt);
void conn_params_error_handler(uint32_t nrf_error);
void conn_params_init(void);
void advertising_stack_init(void);
void advertising_event_handler(ble_adv_evt_t event);
void advertising_data_init(void);
void advertising_init(void);
void advertising_start(void);
void ble_stack_init(void);
void gap_params_init(uint8_t *device_name, uint32_t device_name_length);
uint32_t bas_init(void);
uint32_t dfu_init(void);
void services_init(void);
void advertising_stop(void);
void ble_init(void);
void ble_handle_input_change(uint32_t index, gpio_config_input_digital_t *config);
void ble_disable_rf();


#endif
