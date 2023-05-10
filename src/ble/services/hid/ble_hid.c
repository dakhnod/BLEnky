#include "ble_hid.h"
#include "sdk_common.h"
#include <string.h>
#include "ble_l2cap.h"
#include "app_error.h"
#include "ble_helpers.h"
#include "sensor_gpio.h"
#include "nrf_log.h"
#include "feature_config.h"

#define HID_REPORT_ID_GAMEPAD 0x01

uint16_t ble_hid_connection_handle = BLE_CONN_HANDLE_INVALID;

uint16_t ble_hid_service_handle;

uint16_t ble_hid_characteristic_information_handle;

uint16_t ble_hid_characteristic_report_value_handle;
uint16_t ble_hid_characteristic_report_cccd_handle;

bool ble_hid_report_notification_enabled = false;

uint8_t descriptor_value[] = {
  0x05, 0x01,   // Usage Page (Generic Desktop)
  0x09, 0x05,   // Usage (Game Pad)
  0xA1, 0x01,   // Collection (Application)
  0x85, HID_REPORT_ID_GAMEPAD,   // Report ID (1)
  0x05, 0x09,   // Usage Page (Button)
  0x19, 0x01,   // Usage Minimum (Button 1)
  0x29, 0x10,   // Usage Maximum (Button 16)
  0x15, 0x00,   // Logical Minimum (0)
  0x25, 0x01,   // Logical Maximum (1)
  0x75, 0x01,   // Report Size (1)
  0x95, 0x10,   // Report Count (16)
  0x81, 0x02,   // Input (Data, Variable, Absolute) - 4 buttons

  0x05, 0x01,   // Usage Page (Generic Desktop)
  0x09, 0x39,   // Usage (Hat Switch)
  0x15, 0x01,   // Logical Minimum (1)
  0x25, 0x08,   // Logical Maximum (8)
  0x35, 0x00,   // Physical Minimum (0)
  0x46, 0x3B, 0x01, // Physical Maximum (315)
  0x65, 0x14,   // Unit (Eng Rot:Angular Pos)
  0x75, 0x04,   // Report Size (4)
  0x95, 0x01,   // Report Count (1)
  0x81, 0x42,   // Input (Data, Variable, Absolute, Null State) - hat switch
  0x95, 0x01,   // Report count (1)
  0x75, 0x04,   // Report size (4)
  0x81, 0x03,   // Input (Cnst,Var,Abs)

  0xC0          // End Collection
};

uint8_t report_data[] = {
    // 0x01, // (left out, since present in descriptor) report id, as specified in report map
    0x00, // buttons, on bit each
    0x00,
    0x00, // hat switch rotation
};

uint8_t information_value[] = {
    0x01, 0x10, // spec version 1.10
    0x00, // no country code
    0x00 // no special features
};


void ble_hid_handle_input_change(uint32_t index, gpio_config_input_digital_t *config)
{
  uint32_t input_count = gpio_get_input_digital_pin_count();
  uint8_t input_states[input_count];
  gpio_encode_input_states(input_states);

  #if HID_GAMEPAD_ENABLED == 1

  static struct {
    uint8_t gamepad_up_pressed : 1;
    uint8_t gamepad_down_pressed : 1;
    uint8_t gamepad_left_pressed : 1;
    uint8_t gamepad_right_pressed : 1;
  } gamepad = {0};

  /*
  static bool gamepad_up_pressed = false;
  static bool gamepad_down_pressed = false;
  static bool gamepad_left_pressed = false;
  static bool gamepad_right_pressed = false;
  */

  if(false){} // this is just the starting point for the following cases
  #if (HID_D_PAD_UP_ENABLED == 1)
  else if(HID_D_PAD_UP_PIN == config->pin) gamepad.gamepad_up_pressed = config->state;
  #endif
  #if (HID_D_PAD_DOWN_ENABLED == 1)
  else if(HID_D_PAD_DOWN_PIN == config->pin) gamepad.gamepad_down_pressed = config->state;
  #endif
  #if (HID_D_PAD_LEFT_ENABLED == 1)
  else if(HID_D_PAD_LEFT_PIN == config->pin) gamepad.gamepad_left_pressed = config->state;
  #endif
  #if (HID_D_PAD_RIGHT_ENABLED == 1)
  else if(HID_D_PAD_RIGHT_PIN == config->pin) gamepad.gamepad_right_pressed = config->state;
  #endif


  uint8_t bits = *((uint8_t*)&gamepad);

  uint8_t rotation = 0;
  if(bits == 0b0001){
    rotation = 1;
  }else if(bits == 0b0101){
    rotation = 8;
  }else if(bits == 0b1001){
    rotation = 2;
  }else if(bits == 0b0010){
    rotation = 5;
  }else if(bits == 0b0110){
    rotation = 6;
  }else if(bits == 0b1010){
    rotation = 4;
  }else if(bits == 0b0100){
    rotation = 7;
  }else if(bits == 0b1000){
    rotation = 3;
  }
  NRF_LOG_DEBUG("bits: %x, rotation: %d\n", bits, rotation);
  // rotation <<= 4; // make room for buttons
  report_data[2] = rotation;

  if(false){}
  #if HID_BUTTON_A_ENABLED == 1
  else if(config->pin == HID_BUTTON_A_PIN){
    if(config->state) report_data[0] |=  0b01;
    else              report_data[0] &= ~0b01;
  }
  #endif
  #if HID_BUTTON_B_ENABLED == 1
  else if(config->pin == HID_BUTTON_B_PIN){
    if(config->state) report_data[0] |=  0b10;
    else              report_data[0] &= ~0b10;
  }
  #endif
  #if HID_BUTTON_START_ENABLED == 1
  else if(config->pin == HID_BUTTON_START_PIN){
    if(config->state) report_data[1] |=  0b1000;
    else              report_data[1] &= ~0b1000;
  }
  #endif
  #if HID_BUTTON_SELECT_ENABLED == 1
  else if(config->pin == HID_BUTTON_SELECT_PIN){
    if(config->state) report_data[1] |=  0b0100;
    else              report_data[1] &= ~0b0100;
  }
  #endif

  #endif
  
  if(ble_hid_connection_handle == BLE_CONN_HANDLE_INVALID){
      // no client connected
      return;
  }

  // Disabling this since for some reason
  // Android doesn't re-enable notifications on the report
  /*
  if(!ble_hid_report_notification_enabled){
    return;
  }
  */

  uint16_t len = sizeof(report_data);

  ble_gatts_hvx_params_t params = {
      .handle = ble_hid_characteristic_report_value_handle,
      .type = BLE_GATT_HVX_NOTIFICATION,
      .p_len = &len,
      .p_data = report_data
  };

  ret_code_t err_code = sd_ble_gatts_hvx(
      ble_hid_connection_handle,
      &params
  );
  if(err_code == 0x3401){
    NRF_LOG_ERROR("hvx not enabled...\n");
    return;
  }
  APP_ERROR_CHECK(err_code);
}

void ble_hid_on_connect(ble_evt_t *p_ble_evt)
{
    ble_hid_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

void ble_hid_on_disconnect(ble_evt_t *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    ble_hid_connection_handle = BLE_CONN_HANDLE_INVALID;
    ble_hid_report_notification_enabled = false;
}

void handle_hid_report_cccd_write(ble_gatts_evt_write_t *write_evt)
{
    if (write_evt->len == 2)
    {
        ble_hid_report_notification_enabled = ble_srv_is_notification_enabled(write_evt->data);
        NRF_LOG_DEBUG("notification enabled: %d\n", ble_hid_report_notification_enabled);
    }
}

void ble_hid_on_write(ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t *write_evt = &p_ble_evt
                                            ->evt
                                            .gatts_evt
                                            .params
                                            .write;

    uint16_t handle = write_evt->handle;

    if (handle == ble_hid_characteristic_report_cccd_handle)
    {
        handle_hid_report_cccd_write(write_evt);
        return;
    }
}

void ble_hid_on_ble_evt(ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        ble_hid_on_connect(p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        ble_hid_on_disconnect(p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        NRF_LOG_DEBUG("hid write\n");
        ble_hid_on_write(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}

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
  ret_code_t err_code;

  ble_helper_characteristic_init_t init = {
    .service_handle = ble_hid_service_handle,
    .uuid = BLE_UUID_HID_CHARACTERISTIC_REPORT,
    .is_readable = true,
    .is_notifiable = true,
    .max_length = sizeof(report_data),
    .initial_value_length = sizeof(report_data),
    .initial_value = report_data,
    .value_handle = &ble_hid_characteristic_report_value_handle,
    .cccd_handle = &ble_hid_characteristic_report_cccd_handle
  };

  err_code = ble_helper_characteristic_add(&init);
  APP_ERROR_CHECK(err_code);

  static uint8_t data[] = {
    0x01, 0x01
  };

  ble_gatts_attr_md_t reference_descriptor_metadata = {
    .vlen = 0x00,
    .vloc = BLE_GATTS_VLOC_STACK,
    .rd_auth = 0,
    .wr_auth = 0
  };
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&reference_descriptor_metadata.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&reference_descriptor_metadata.write_perm);

  ble_uuid_t reference_uuid = {
    .type = BLE_UUID_TYPE_BLE,
    .uuid = 0x2908
  };

  ble_gatts_attr_t reference_descriptor_attributes = {
      .init_offs = 0,
      .init_len = sizeof(data),
      .max_len = sizeof(data),
      .p_uuid = &reference_uuid,
      .p_value = data,
      .p_attr_md = &reference_descriptor_metadata
  };

  return sd_ble_gatts_descriptor_add(
    ble_hid_characteristic_report_value_handle,
    &reference_descriptor_attributes,
    NULL
  );
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