#include "fstorage.h"
#include "nrf_log.h"

void storage_init();
void storage_store(uint8_t *data, uint32_t length);
void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result);
void storage_on_sys_evt(uint32_t sys_evt);
void storage_read(uint8_t *buffer, uint32_t length);
void storage_read_flip(uint8_t *buffer, uint32_t length);