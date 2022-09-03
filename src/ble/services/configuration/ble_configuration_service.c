#include "ble_configuration_service.h"
#include "app_error.h"

uint8_t configuration_custom_uuid_type;

uint16_t ble_service_handle;

ret_code_t ble_configuration_service_init() {

  ret_code_t err_code;

  ble_uuid128_t vs_uuid = {
      .uuid128 = CUSTOM_UUID_BASE_CONFIGURATION_SERVICE
  };

  err_code = sd_ble_uuid_vs_add(&vs_uuid, &configuration_custom_uuid_type);
  APP_ERROR_CHECK(err_code);


  ble_uuid_t ble_uuid = {
    .type = configuration_custom_uuid_type,
    .uuid = UUID_SERVICE_CONFIGURATION
  };

  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_service_handle);
  APP_ERROR_CHECK(err_code);

  return NRF_SUCCESS;
}

uint8_t ble_configuration_service_get_custom_uuid_type() {
  return configuration_custom_uuid_type;
}