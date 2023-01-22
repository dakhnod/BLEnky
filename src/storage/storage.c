#include "storage.h"
#include "app_timer.h"
#include "ble_configuration.h"
#include "crc32.h"
#include "nrf_delay.h"

APP_TIMER_DEF(reboot_timer);
#define REBOOT_TIMEOUT APP_TIMER_TICKS(500, APP_TIMER_PRESCALER)

#define OFFSET_PIN_CONFIGURATION 0x00
#define OFFSET_CONNECTION_PARAMS_CONFIGURATION 0x10
#define OFFSET_DEVICE_NAME 0x1A
#define OFFSET_CHECKSUM (OFFSET_DEVICE_NAME + LENGTH_DEVICE_NAME)

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
  }else if(evt->id == FS_EVT_ERASE) {
    if (result != FS_SUCCESS) {
      NRF_LOG_DEBUG("fstorage erase failed: %d\n", result);
      return;
    }
    NRF_LOG_DEBUG("fstorage erase successfull\n");
  }
  NRF_LOG_DEBUG("fstorage callback: event %d,  result %d\n", evt->id, result);
}

void storage_erase(){
  ret_code_t ret_code = fs_erase(
    &fs_config,
    fs_config.p_start_addr,
    1,
    NULL
  );

  if (ret_code != FS_SUCCESS) {
    NRF_LOG_DEBUG("fstorage erase failure: %d\n", ret_code);
    return;
  }
}

uint32_t checksum_compute(uint8_t *data, uint32_t length){
  return crc32_compute(data, length, NULL);
};

void storage_read(uint32_t offset, uint8_t *buffer, uint32_t length) {
  // casting p_start_addr, so that offset calculation does not add offset * sizeof(uint32_t)
  memcpy(buffer, ((uint8_t *)fs_config.p_start_addr) + offset, length);
}

void storage_checksum_check(){
  uint32_t length = OFFSET_CHECKSUM;
  // add 4 bytes for checksum
  uint8_t data[length + 4];

  // read 4 more to capcure checksum
  storage_read(0x00, data, length + 4);

  bool is_erased = true;

  for(uint32_t i = 0; i < length; i++){
    if(data[i] != 0xFF){
      is_erased = false;
      break;
    }
  }

  if(is_erased){
    NRF_LOG_DEBUG("flash erased, not checking checksum\n");
    return;
  }

  uint32_t checksum_calculated = checksum_compute(data, length);
  uint32_t checksum_stored = 0;
  // need to do it this way since checksum may not be memory-aligned
  for(int i = 0; i < 4; i++){
    checksum_stored |= (data[OFFSET_CHECKSUM + i]) << (i * 8);
  }

  NRF_LOG_DEBUG("calculated checksum: %x, stored: %x\n", checksum_calculated, checksum_stored);

  if(checksum_calculated == checksum_stored){
    // checksum valid
    return;
  }

  NRF_LOG_ERROR("checksum invalid, erasing settings page\n");

  // erase flash if checksum invalid
  storage_erase();

  // giving flash some time to erase flash page
  nrf_delay_ms(1000);

  uint32_t count;

  (void)fs_queued_op_count_get(&count);

  return;
};

void storage_init() {
  fs_ret_t ret = fs_init();
  if (ret != FS_SUCCESS) {
    NRF_LOG_DEBUG("fstorage init failure\n");
    return;
  }
  NRF_LOG_DEBUG("fstorage init success, address %x - %x\n", (uint32_t)fs_config.p_start_addr, (uint32_t)fs_config.p_end_addr);

  storage_checksum_check();

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

void storage_read_pin_configuration(uint8_t *buffer) {
  storage_read(OFFSET_PIN_CONFIGURATION, buffer, 16);
}

void storage_read_connection_params_configuration(uint8_t *buffer) {
  storage_read(OFFSET_CONNECTION_PARAMS_CONFIGURATION, buffer, 10);
}

void storage_read_device_name(uint8_t *buffer, uint32_t *length_) {
  storage_read(OFFSET_DEVICE_NAME, buffer, LENGTH_DEVICE_NAME);

  uint32_t length;

  for(length = 0; ; length++){
    if(length >= LENGTH_DEVICE_NAME){
      break;
    }
    if(buffer[length] == 0){
      break;
    }
    if(buffer[length] == 0xFF){
      break;
    }
  }

  *length_ = length;
}

void storage_store(uint32_t offset, uint8_t *data, uint32_t length, uint8_t reboot) {
  fs_ret_t ret_code;

  // 16 bytes for pin configuration + 10 bytes for connection param configuration + 20 bytes for device name
  const uint32_t size = OFFSET_CHECKSUM;
  
  // should should be done dynamically, but at compile-time
  // we are also allocating 4 bytes for checksum + 2 bytes for alignment
  const uint32_t size_aligned = 52; // calculate 4-byte-alignet size

  const uint32_t data_size_32 = size_aligned / 4; // calculate size in 32-bit-words

  // we should use size_aligned as the size, but that isn't constant enough for the compiler...
  static uint8_t storage_data[52]; 
  storage_read(0, storage_data, size); // read whole storage

  memcpy(storage_data + offset, data, length);

  uint32_t checksum = checksum_compute(storage_data, OFFSET_CHECKSUM);

  // apend checksum to buffer
  memcpy(storage_data + OFFSET_CHECKSUM, (uint8_t*)(&checksum), 4);

  storage_erase();

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
  storage_store(OFFSET_CONNECTION_PARAMS_CONFIGURATION, data, 10, true);
}

void storage_store_device_name(uint8_t *name, int length) {
  uint8_t name_buffer[LENGTH_DEVICE_NAME];
  memcpy(name_buffer, name, MIN(length, LENGTH_DEVICE_NAME));
  if(length < LENGTH_DEVICE_NAME){
    name_buffer[length] = 0;
  }

  storage_store(OFFSET_DEVICE_NAME, name_buffer, LENGTH_DEVICE_NAME, true);
}