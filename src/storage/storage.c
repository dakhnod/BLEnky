#include "storage.h"
#include "app_timer.h"
#include "ble_configuration.h"

APP_TIMER_DEF(reboot_timer);
#define REBOOT_TIMEOUT APP_TIMER_TICKS(500, APP_TIMER_PRESCALER)

FS_REGISTER_CFG(fs_config_t fs_config) =
{
    .callback = fs_evt_handler, // Function for event callbacks.
    .num_pages = 1,      // Number of physical flash pages required.
    .priority = 0xFE,            // Priority for flash usage.
    .p_start_addr = (uint32_t *)0x00035800, // start of last page before bootloader
    .p_end_addr = (uint32_t *)0x00035C00, // start of bootloader
};

void fs_evt_handler(fs_evt_t const *const evt, fs_ret_t result) {
  if (result != FS_SUCCESS) {
    NRF_LOG_DEBUG("fstorage failure\n");
    return;
  }
  if (evt->id == FS_EVT_STORE) {
    if (result != FS_SUCCESS) {
      NRF_LOG_DEBUG("fstorage store failed: %d\n", result);
      return;
    }
    NRF_LOG_DEBUG("fstorage store successfull\n");
    if (((uint8_t *)evt->p_context)[0] == 0x01) {
      NRF_LOG_DEBUG("reboot requested, rebooting...\n");
      ret_code_t err_code = app_timer_start(reboot_timer, REBOOT_TIMEOUT, NULL);
      APP_ERROR_CHECK(err_code);
      return;
    }
  }
  NRF_LOG_DEBUG("fstorage callback: event %d,  result %d\n", evt->id, result);
}

void storage_init() {
  fs_ret_t ret = fs_init();
  if (ret != FS_SUCCESS) {
    NRF_LOG_DEBUG("fstorage init failure\n");
    return;
  }
  NRF_LOG_DEBUG("fstorage init success, address %x - %x\n", (uint32_t)fs_config.p_start_addr, (uint32_t)fs_config.p_end_addr);

  ret_code_t err_code = app_timer_create(
    &reboot_timer,
    APP_TIMER_MODE_SINGLE_SHOT,
    (app_timer_timeout_handler_t)NVIC_SystemReset
  );
  APP_ERROR_CHECK(err_code);
}

void storage_on_sys_evt(uint32_t sys_evt) {
  fs_sys_event_handler(sys_evt);
}

void storage_read(uint32_t offset, uint8_t *buffer, uint32_t length) {
  // casting p_start_addr, so that offset calculation does not add offset * sizeof(uint32_t)
  memcpy(buffer, ((uint8_t *)fs_config.p_start_addr) + offset, length);
}

void storage_read_pin_configuration(uint8_t *buffer) {
  storage_read(OFFSET_PIN_CONFIGURATION, buffer, 16);
}

void storage_read_connection_params_configuration(uint8_t *buffer) {
  storage_read(OFFSET_CONNECTION_PARAMS_CONFIGURATION, buffer, 10);
}

void storage_store(uint32_t offset, uint8_t *data, uint32_t length, uint8_t reboot) {
  fs_ret_t ret_code;

  uint32_t data_size_32 = CEIL_DIV(26, 4);

  // this is prefered, but initializing storage_data with unknown length is illegal
  // uint32_t data_size = data_size_32 * 4;;

  // needs to be static for fs_store
  static uint8_t storage_data[28]; // 16 pins for pin configuration + 10 pins for connection param configuration + 2 bytes for alignment
  storage_read(0, storage_data, 26); // read whole storage

  memcpy(storage_data + offset, data, length);

  ret_code = fs_erase(
    &fs_config,
    fs_config.p_start_addr,
    1,
    NULL
  );

  if (ret_code != FS_SUCCESS) {
    NRF_LOG_DEBUG("fstorage erase failure: %d\n", ret_code);
    return;
  }

  static uint8_t context = false;
  context = reboot;

  ret_code = fs_store(
    &fs_config,
    fs_config.p_start_addr,
    (uint32_t *)storage_data,
    data_size_32,
    &context
  );

  APP_ERROR_CHECK(ret_code);
}

void storage_store_pin_configuration(uint8_t *data) {
  storage_store(OFFSET_PIN_CONFIGURATION, data, 16, true);
}

void storage_store_connection_params_configuration(uint8_t *data) {
  storage_store(OFFSET_CONNECTION_PARAMS_CONFIGURATION, data, 10, false);
}