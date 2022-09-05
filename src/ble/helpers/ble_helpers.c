#include "ble_helpers.h"
#include "ble.h"
#include "app_error.h"
#include "nrf_log.h"

ret_code_t ble_helper_characteristic_add(
  uint16_t service_handle,
  uint16_t uuid,
  uint8_t uuid_type,
  char *description_str,
  uint8_t is_writable,
  uint8_t is_readable,
  uint8_t is_notifiable,
  uint8_t authorize_read,
  uint8_t authorize_write,
  uint16_t max_length,
  uint16_t *value_handle,
  uint16_t *cccd_handle
) {
  return ble_helper_characteristic_digital_add(
    service_handle,
    uuid,
    uuid_type,
    description_str,
    0x00,
    0x00,
    is_writable,
    is_readable,
    is_notifiable,
    authorize_read,
    authorize_write,
    max_length,
    value_handle,
    cccd_handle
  );
}

ret_code_t ble_helper_characteristic_digital_add(
  uint16_t service_handle,
  uint16_t uuid,
  uint8_t uuid_type,
  char *description_str,
  uint8_t number_of_digitals,
  uint8_t description,
  uint8_t is_writable,
  uint8_t is_readable,
  uint8_t is_notifiable,
  uint8_t authorize_read,
  uint8_t authorize_write,
  uint16_t max_length,
  uint16_t *value_handle,
  uint16_t *cccd_handle
) {
  ble_gatts_char_handles_t p_handles;

  ble_gatts_attr_md_t cccd_md = {
      .vloc = BLE_GATTS_VLOC_STACK
  };
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

  ble_gatts_char_pf_t char_pf = {
      .unit = 0x2700,
      .format = BLE_GATT_CPF_FORMAT_STRUCT,
      .name_space = 0x01,
      .exponent = 0x00,
      .desc = description
  };

  ble_gatts_attr_md_t user_description_metadata = {
      .vlen = 0x00,
      .vloc = BLE_GATTS_VLOC_STACK,
      .rd_auth = 0,
      .wr_auth = 0
  };
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&user_description_metadata.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&user_description_metadata.write_perm);

  ble_gatts_char_md_t char_md = {
      .char_props.read = is_readable,
      .char_props.write = is_writable,
      .char_props.notify = is_notifiable,
      .char_props.indicate = 0,
      .p_char_user_desc = (uint8_t *)description_str,
      .char_user_desc_max_size = strlen(description_str),
      .char_user_desc_size = strlen(description_str),
      .p_cccd_md = &cccd_md,
      .p_char_pf = NULL,
      .p_user_desc_md = &user_description_metadata,
      .p_sccd_md = NULL
  };

  if (description > 0) {
    char_md.p_char_pf = &char_pf;
  }

  ble_uuid_t ble_uuid = {
      .type = uuid_type,
      .uuid = uuid
  };

  ble_gatts_attr_md_t attr_md = {
      .vloc = BLE_GATTS_VLOC_STACK,
      .rd_auth = authorize_read,
      .wr_auth = authorize_write,
      .vlen = 1,
  };
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

  ble_gatts_attr_t attr_char_value = {
      .p_uuid = &ble_uuid,
      .p_attr_md = &attr_md,
      .max_len = max_length
  };
  ret_code_t err_code = sd_ble_gatts_characteristic_add(service_handle,
    &char_md,
    &attr_char_value,
    &p_handles);

  APP_ERROR_CHECK(err_code);

  *value_handle = p_handles.value_handle;
  *cccd_handle = p_handles.cccd_handle;

  if (number_of_digitals > 0) {
    uint16_t number_of_digitals_handle;

    ble_gatts_attr_md_t number_of_digitals_metadata = {
        .vlen = 0x00,
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = 0,
        .wr_auth = 0 };
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&number_of_digitals_metadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&number_of_digitals_metadata.write_perm);

    ble_uuid_t number_of_digitals_uuid = {
        .type = BLE_UUID_TYPE_BLE,
        .uuid = 0x2909
    };

    ble_gatts_attr_t number_of_digitals_descriptor_attributes = {
        .init_offs = 0,
        .init_len = 1,
        .max_len = 1,
        .p_uuid = &number_of_digitals_uuid,
        .p_value = &number_of_digitals,
        .p_attr_md = &number_of_digitals_metadata
    };

    err_code = sd_ble_gatts_descriptor_add(
      p_handles.value_handle,
      &number_of_digitals_descriptor_attributes,
      &number_of_digitals_handle);


    APP_ERROR_CHECK(err_code);
  }

  return err_code;
}