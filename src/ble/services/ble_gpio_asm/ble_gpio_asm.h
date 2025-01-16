#include "stdint.h"
#include "sdk_common.h"
#include "ble_srv_common.h"
#include "sensor_gpio.h"

#define UUID_GPIO_ASM_SERVICE   0x0000
#define UUID_GPIO_ASM_DATA      0x0001

#define UUID_GPIO_ASM_BASE { 0xb0, 0x2a, 0x86, 0xb3, 0xcd, 0xc1, 0x4f, 0x78, 0xa2, 0xd5, 0x74, 0x2a, 0x81, 0xab, 0x19, 0xb1 }

void ble_gpio_asm_init();
void ble_gpio_asm_on_ble_evt(ble_evt_t *p_ble_evt);
void ble_gpio_asm_handle_input_change();