#include "sensor_gpio.h"
#include "ble_srv_common.h"

#define SLEEP_MODE_SYSTEM_ON  0x00
#define SLEEP_MODE_SYSTEM_OFF 0x01

void sleep_init();
bool sleep_get_allow_advertise();
void sleep_handle_gpio_event(uint32_t index, gpio_config_input_digital_t *input);
void sleep_handle_ble_evt(ble_evt_t *p_ble_evt);