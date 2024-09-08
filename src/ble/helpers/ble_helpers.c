#include "ble_helpers.h"
#include "ble.h"
#include "app_error.h"
#include "nrf_log.h"
#include "feature_config.h"


ret_code_t ble_helper_characteristic_add(ble_helper_characteristic_init_t *init) {
  ble_gatts_char_handles_t p_handles;

  ble_gatts_attr_md_t cccd_md = {
      .vloc = BLE_GATTS_VLOC_STACK
  };

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
  SET_MODE_SECURE(&cccd_md.write_perm);

  ble_gatts_char_pf_t char_pf = {
      .unit = 0x2700,
      .format = BLE_GATT_CPF_FORMAT_STRUCT,
      .name_space = 0x01,
      .exponent = 0x00,
      .desc = init->description
  };

  ble_gatts_char_md_t char_md = {
      .char_props.read = init->is_readable,
      .char_props.write = init->is_writable,
      .char_props.notify = init->is_notifiable,
      .char_props.indicate = 0,
      .p_cccd_md = &cccd_md
  };

  ble_gatts_attr_md_t user_description_metadata;
  if(init->description_str != NULL){
    user_description_metadata.vlen = 0x00;
    user_description_metadata.vloc = BLE_GATTS_VLOC_STACK;
    user_description_metadata.rd_auth = 0x00;
    user_description_metadata.wr_auth = 0x00;

    char_md.p_user_desc_md = &user_description_metadata;

    SET_MODE_SECURE(&user_description_metadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&user_description_metadata.write_perm);

    char_md.p_char_user_desc = (uint8_t *)init->description_str;
    char_md.char_user_desc_max_size = strlen(init->description_str);
    char_md.char_user_desc_size = strlen(init->description_str);
  }

  if (init->description > 0) {
    char_md.p_char_pf = &char_pf;
  }

  uint8_t type = init->uuid_type;
  if(type == BLE_UUID_TYPE_UNKNOWN){
    type = BLE_UUID_TYPE_BLE;
  }
  ble_uuid_t ble_uuid = {
      .type = type,
      .uuid = init->uuid
  };

  ble_gatts_attr_md_t attr_md = {
      .vloc = init->location_user ? BLE_GATTS_VLOC_USER : BLE_GATTS_VLOC_STACK,
      .rd_auth = init->authorize_read,
      .wr_auth = init->authorize_write,
      .vlen = 1,
  };

  SET_MODE_SECURE(&attr_md.read_perm);
  SET_MODE_SECURE(&attr_md.write_perm);

  ble_gatts_attr_t attr_char_value = {
      .p_uuid = &ble_uuid,
      .p_attr_md = &attr_md,
      .max_len = init->max_length,
  };

  if((init->initial_value_length > 0) && (init->initial_value != NULL)){
    attr_char_value.init_len = init->initial_value_length;
    attr_char_value.p_value = init->initial_value;
  }

  ret_code_t err_code = sd_ble_gatts_characteristic_add(
    init->service_handle,
    &char_md,
    &attr_char_value,
    &p_handles
  );

  APP_ERROR_CHECK(err_code);

  *(init->value_handle) = p_handles.value_handle;
  *(init->cccd_handle) = p_handles.cccd_handle;

  if (init->number_of_digitals > 0) {
    uint16_t number_of_digitals_handle;

    ble_gatts_attr_md_t number_of_digitals_metadata = {
        .vlen = 0x00,
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = 0,
        .wr_auth = 0
    };

    SET_MODE_SECURE(&number_of_digitals_metadata.read_perm);

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
        .p_value = &init->number_of_digitals,
        .p_attr_md = &number_of_digitals_metadata
    };

    err_code = sd_ble_gatts_descriptor_add(
      p_handles.value_handle,
      &number_of_digitals_descriptor_attributes,
      &number_of_digitals_handle
    );


    APP_ERROR_CHECK(err_code);
  }

  return err_code;
}