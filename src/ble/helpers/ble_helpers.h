#include "sdk_common.h"
#include "stdint.h"
#include "feature_config.h"

#if STATIC_PASSKEY_ENABLED == 1
#define SET_MODE_SECURE BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM
#else
#define SET_MODE_SECURE BLE_GAP_CONN_SEC_MODE_SET_OPEN
#endif

typedef struct {
  uint16_t service_handle;
  uint16_t uuid;
  uint8_t uuid_type;
  char *description_str;
  uint8_t number_of_digitals;
  uint8_t description;
  uint8_t is_writable;
  uint8_t is_readable;
  uint8_t is_notifiable;
  uint8_t authorize_read;
  uint8_t authorize_write;
  uint16_t max_length;
  uint16_t *value_handle;
  uint16_t *cccd_handle;
  uint8_t *initial_value;
  uint16_t initial_value_length;
  bool location_user;
} ble_helper_characteristic_init_t;

ret_code_t ble_helper_characteristic_add(ble_helper_characteristic_init_t*);