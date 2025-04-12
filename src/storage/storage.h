#if FAMILY == 51
#include "fstorage.h"
#else
#include "nrf_fstorage.h"
#endif
#include "nrf_log.h"

#define PIN_CONFIGURATION_LENGTH (HARDWARE_PIN_COUNT / 2)
#define LENGTH_DEVICE_NAME 20

#define OFFSET_PIN_CONFIGURATION 0x00
#define OFFSET_CONNECTION_PARAMS_CONFIGURATION (OFFSET_PIN_CONFIGURATION + PIN_CONFIGURATION_LENGTH)
#define OFFSET_DEVICE_NAME (OFFSET_CONNECTION_PARAMS_CONFIGURATION + 10)
#define OFFSET_CHECKSUM (OFFSET_DEVICE_NAME + LENGTH_DEVICE_NAME)

// the +4 bytes are for storing a checksum
// the +2 bytes are to allign to 4 bytes since the size comes out to 50 or 66
#define CONFIGURATION_SIZE (OFFSET_CHECKSUM + 4 + 2)

void storage_init();
#if FAMILY == 51
void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result);
#else
void fs_evt_handler(nrf_fstorage_evt_t * p_evt);
#endif
void storage_on_sys_evt(uint32_t sys_evt);
void storage_store_pin_configuration(uint8_t *buffer);
void storage_store_device_name(const uint8_t *name, int length);
void storage_store_connection_params_configuration(const uint8_t *buffer);

void storage_read_connection_params_configuration(uint8_t *buffer);
void storage_read_device_name(uint8_t *buffer, uint32_t *length);
void storage_read_pin_configuration(uint8_t *buffer);