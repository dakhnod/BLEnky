#include "ble_srv_common.h"
#include "app_timer.h"
#include "sensor_timer.h"
#include "sensor_gpio.h"

#define UUID_CSC_SERVICE                          0x1816
#define UUID_CSC_CHARACTERISTIC_SPEED_MEASUREMENT 0x2A5B
#define UUID_CSC_CHARACTERISTIC_FEATURE           0x2A5C

#define MEASUREMENT_REPORT_INTERVAL APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)

#define CSC_MEAS_FLAG_MASK_WHEEL_REV_DATA_PRESENT (0x01 << 0)  /**< Wheel revolution data present flag bit. */

void ble_csc_on_ble_evt(ble_evt_t *);
ret_code_t ble_csc_init();
void ble_csc_handle_input_change(uint32_t, gpio_config_input_digital_t *);