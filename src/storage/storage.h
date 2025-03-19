#include "fstorage.h"
#include "nrf_log.h"

#define PIN_CONFIGURATION_LENGTH (HARDWARE_PIN_COUNT / 2)
#define LENGTH_DEVICE_NAME 20

void storage_init();
void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result);
void storage_on_sys_evt(uint32_t sys_evt);
void storage_store_pin_configuration(uint8_t *buffer);
void storage_store_device_name(uint8_t *name, int length);
void storage_store_connection_params_configuration(uint8_t *buffer);

void storage_read_connection_params_configuration(uint8_t *buffer);
void storage_read_device_name(uint8_t *buffer, uint32_t *length);
void storage_read_pin_configuration(uint8_t *buffer);