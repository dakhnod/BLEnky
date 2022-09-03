#include "sdk_common.h"
#include "stdint.h"

ret_code_t ble_characteristic_digital_add(
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
  uint16_t max_length,
  uint16_t *value_handle,
  uint16_t *cccd_handle
);