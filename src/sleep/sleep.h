#include "sensor_gpio.h"
#include "ble_srv_common.h"

#define SLEEP_MODE_SYSTEM_ON  0x00
#define SLEEP_MODE_SYSTEM_OFF 0x01

typedef void (*sleep_enter_handler_t)();

void sleep_init(sleep_enter_handler_t);
bool sleep_get_allow_advertise();
void sleep_handle_gpio_event(uint32_t index, gpio_config_input_digital_t *input);