#include "ble_srv_common.h"
#include "sensor_gpio.h"

#define BLE_UUID_OFFSET 0x0000

#define BLE_UUID_HID_SERVICE                      0x1812 + BLE_UUID_OFFSET
#define BLE_UUID_HID_CHARACTERISTIC_INFORMATION   0x2A4A + BLE_UUID_OFFSET
#define BLE_UUID_HID_CHARACTERISTIC_REPORT_MAP    0x2A4B + BLE_UUID_OFFSET
#define BLE_UUID_HID_CHARACTERISTIC_CONTROL_POINT 0x2A4C + BLE_UUID_OFFSET
#define BLE_UUID_HID_CHARACTERISTIC_REPORT        0x2A4D + BLE_UUID_OFFSET
#define BLE_UUID_HID_DESCRIPTOR_REPORT_REFERENCE  0x2908 + BLE_UUID_OFFSET

ret_code_t ble_hid_init();
void ble_hid_handle_input_change(uint32_t index, gpio_config_input_digital_t *config);
void ble_hid_on_ble_evt(ble_evt_t *p_ble_evt);