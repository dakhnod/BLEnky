#include "stdint.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "sdk_common.h"


#define CUSTOM_UUID_BASE_CONFIGURATION_SERVICE {0xdc, 0x71, 0xd1, 0xc1, 0xfd, 0x01, 0x49, 0x15, 0xa7, 0x8f, 0xf1, 0x5c, 0x00, 0x00, 0x10, 0x9c}

#define UUID_SERVICE_CONFIGURATION 0x0000

uint8_t ble_configuration_service_get_custom_uuid_type();
ret_code_t ble_configuration_service_init();