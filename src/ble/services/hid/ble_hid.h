#include "ble_srv_common.h"
#include "sensor_gpio.h"

#define BLE_UUID_HID_SERVICE                      0x1812
#define BLE_UUID_HID_CHARACTERISTIC_INFORMATION   0x2A4A
#define BLE_UUID_HID_CHARACTERISTIC_REPORT_MAP    0x2A4B
#define BLE_UUID_HID_CHARACTERISTIC_CONTROL_POINT 0x2A4C
#define BLE_UUID_HID_CHARACTERISTIC_REPORT        0x2A4D

ret_code_t ble_hid_init();