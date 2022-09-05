#include "stdint.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "sdk_common.h"


#define CUSTOM_UUID_BASE_CONFIGURATION_SERVICE {0xdc, 0x71, 0xd1, 0xc1, 0xfd, 0x01, 0x49, 0x15, 0xa7, 0x8f, 0xf1, 0x5c, 0x00, 0x00, 0x10, 0x9c}

#define UUID_SERVICE_CONFIGURATION 0x0000
#define UUID_CHARACTERISTIC_PIN_CONFIGURATION 0x0001
#define UUID_CHARACTERISTIC_CONNECITON_PARAMS_CONFIGURATION 0x0002

typedef struct {
  uint16_t min_conn_interval;
  uint16_t max_conn_interval;
  uint16_t slave_latency;
  uint16_t conn_sup_timeout;
  uint16_t advertising_interval;

} ble_configuration_connection_params_packet_t;

typedef void (*ble_configuration_connection_params_update_handler_t)(ble_configuration_connection_params_packet_t *);

uint8_t ble_configuration_service_get_custom_uuid_type();
ret_code_t ble_configuration_service_init(ble_configuration_connection_params_update_handler_t connection_params_update_handler);
void ble_configuration_on_ble_event(ble_evt_t *p_ble_evt);