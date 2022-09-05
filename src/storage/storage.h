#include "fstorage.h"
#include "nrf_log.h"

#define OFFSET_PIN_CONFIGURATION 0x00
#define OFFSET_CONNECTION_PARAMS_CONFIGURATION 0x10

void storage_init();
void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result);
void storage_on_sys_evt(uint32_t sys_evt);
void storage_read_pin_configuration(uint8_t *buffer);
void storage_store_pin_configuration(uint8_t *buffer);

void storage_read_connection_params_configuration(uint8_t *buffer);
void storage_store_connection_params_configuration(uint8_t *buffer);