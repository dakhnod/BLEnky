#include "ble_hid.h"
#include "sdk_common.h"
#include <string.h>
#include "ble_l2cap.h"
#include "app_error.h"
#include "ble_helpers.h"
#include "sensor_gpio.h"
#include "nrf_log.h"

uint16_t ble_hid_service_handle;

uint16_t ble_hid_characteristic_information_handle;

uint8_t descriptor_value[] = {
    /* Gamepad with eight buttons */
    0x05, 0x01, /* Usage Page (Generic Desktop) */
    0x09, 0x05, /* Usage (Game Pad) */
    0xA1, 0x01, /* Collection (Application) */
    0x85, 0x01, /* Report ID (1) */
    0x05, 0x09, /* Usage Page (Button) */
    0x19, 0x01, /* Usage Minimum (Button 1) */
    0x29, 0x08, /* Usage Maximum (Button 8) */
    0x15, 0x00, /* Logical Minimum (0) */
    0x25, 0x01, /* Logical Maximum (1) */
    0x75, 0x01, /* Report Size (1) */
    0x95, 0x08, /* Report Count (8) */
    0x81, 0x02, /* Input (Data,Var,Abs) */
    0xC0 /* End Collection */
};

uint8_t report_data[] = {
    0x01, // report id, as specified in report map
    0x00, // buttons, on bit each
    0x00 // padding, questionable
};

uint8_t information_value[] = {
    0x01, 0x10, // spec version 1.10
    0x00, 0x00, // no country code
    0x00 // no special features
};

ret_code_t ble_hid_characteristic_informatioin_add()
{
  ble_helper_characteristic_init_t init = {
    .service_handle = ble_hid_service_handle,
    .uuid = BLE_UUID_HID_CHARACTERISTIC_INFORMATION,
    .is_readable = true,
    .max_length = sizeof(information_value),
    .initial_value_length = sizeof(information_value),
    .initial_value = information_value
  };

  return ble_helper_characteristic_add(&init);
}

ret_code_t ble_hid_characteristic_report_map_add()
{
  ble_helper_characteristic_init_t init = {
    .service_handle = ble_hid_service_handle,
    .uuid = BLE_UUID_HID_CHARACTERISTIC_REPORT_MAP,
    .is_readable = true,
    .max_length = sizeof(descriptor_value),
    .initial_value_length = sizeof(descriptor_value),
    .initial_value = descriptor_value
  };

  return ble_helper_characteristic_add(&init);
}

ret_code_t ble_hid_characteristic_control_point_add()
{
  ble_helper_characteristic_init_t init = {
    .service_handle = ble_hid_service_handle,
    .uuid = BLE_UUID_HID_CHARACTERISTIC_CONTROL_POINT,
    .is_writable = true,
    .max_length = 1
  };

  return ble_helper_characteristic_add(&init);
}

ret_code_t ble_hid_characteristic_report_add()
{
  ble_helper_characteristic_init_t init = {
    .service_handle = ble_hid_service_handle,
    .uuid = BLE_UUID_HID_CHARACTERISTIC_REPORT,
    .is_readable = true,
    .is_notifiable = true,
    .max_length = sizeof(report_data),
    .initial_value_length = sizeof(report_data),
    .initial_value = report_data
  };

  return ble_helper_characteristic_add(&init);
}

ret_code_t ble_hid_init()
{
    ret_code_t err_code;
    ble_uuid_t ble_uuid;

    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_HID_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_hid_service_handle);
    APP_ERROR_CHECK(err_code);

    err_code = ble_hid_characteristic_informatioin_add();
    APP_ERROR_CHECK(err_code);

    err_code = ble_hid_characteristic_report_map_add();
    APP_ERROR_CHECK(err_code);

    err_code = ble_hid_characteristic_control_point_add();
    APP_ERROR_CHECK(err_code);

    err_code = ble_hid_characteristic_report_add();
    APP_ERROR_CHECK(err_code);

    return NRF_SUCCESS;
}