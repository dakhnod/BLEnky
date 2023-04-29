#include "ble_srv_common.h"

#define UUID_CSC_SERVICE                          0x1816
#define UUID_CSC_CHARACTERISTIC_SPEED_MEASUREMENT 0x2A5B
#define UUID_CSC_CHARACTERISTIC_FEATURE           0x2A5C

void ble_csc_on_ble_evt(ble_evt_t *);
ret_code_t ble_csc_init();