#include "storage.h"
#include "app_timer.h"
#include "crc32.h"
#include "nrf_delay.h"
#include "feature_config.h"
#include "nrf_fstorage_sd.h"
#include "nrf_fstorage_nvmc.h"

NRF_FSTORAGE_DEF(nrf_fstorage_t m_storage) =
{
    .evt_handler    = fs_evt_handler,
    // addresses taken from linker script
    // which in turn takes those from the bootloader start
    .start_addr     = (0xA1000),
    .end_addr       = (0xA2000)
};

void fs_evt_handler(nrf_fstorage_evt_t * p_evt) {


  NRF_LOG_DEBUG("fstorage callback: event %d,  result %d\n", p_evt->id, p_evt->result);

  if (p_evt->result == NRF_SUCCESS)
    {
        NRF_LOG_DEBUG("Flash %s success: addr=%p",
                      (p_evt->id == NRF_FSTORAGE_EVT_WRITE_RESULT) ? "write" : "erase",
                      p_evt->addr);
    }
    else
    {
        NRF_LOG_DEBUG("Flash %s failed (0x%x): addr=%p, len=0x%x bytes",
                      (p_evt->id == NRF_FSTORAGE_EVT_WRITE_RESULT) ? "write" : "erase",
                      p_evt->result, p_evt->addr, p_evt->len);
    }

    if ((*(uint8_t *)(p_evt->p_param)) == 0x01) {
      // reboot requested
      NVIC_SystemReset();
      return;
    }
}

void storage_erase(){
  ret_code_t ret_code = nrf_fstorage_erase(
    &m_storage,
    m_storage.start_addr,
    1,
    NULL
  );

  if (ret_code != NRF_SUCCESS) {
    NRF_LOG_DEBUG("fstorage erase failure: %d\n", ret_code);
    return;
  }
}

uint32_t checksum_compute(uint8_t *data, uint32_t length){
  return crc32_compute(data, length, NULL);
};

void storage_read(uint32_t offset, uint8_t *buffer, uint32_t length) {
  // casting p_start_addr, so that offset calculation does not add offset * sizeof(uint32_t)
  memcpy(buffer, ((uint8_t *)m_storage.start_addr) + offset, length);
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
  nrf_delay_ms(3);

  return;
};

void storage_init() {
  nrf_fstorage_api_t * p_api_impl;
  bool sd_irq_initialized = true;

  NRF_LOG_DEBUG("Calling nrf_dfu_flash_init(sd_irq_initialized=%s)...",
                sd_irq_initialized ? "true" : "false");

  /* Setup the desired API implementation. */
#ifdef BLE_STACK_SUPPORT_REQD
  if (sd_irq_initialized)
  {
      NRF_LOG_DEBUG("Initializing nrf_fstorage_sd backend.");
      p_api_impl = &nrf_fstorage_sd;
  }
  else
#endif
  {
      NRF_LOG_DEBUG("Initializing nrf_fstorage_nvmc backend.");
      p_api_impl = &nrf_fstorage_nvmc;
  }

  ret_code_t err_code = nrf_fstorage_init(&m_storage, p_api_impl, NULL);
  APP_ERROR_CHECK(err_code);

  storage_checksum_check();
}

void storage_read_pin_configuration(uint8_t *buffer) {
  storage_read(OFFSET_PIN_CONFIGURATION, buffer, PIN_CONFIGURATION_LENGTH);
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

void storage_store(uint32_t offset, const uint8_t *data, uint32_t length, const uint8_t reboot) {
  // PIN_CONFIGURATION_LENGTH bytes for pin configuration + 10 bytes for connection param configuration + 20 bytes for device name
  const uint32_t size = OFFSET_CHECKSUM;

  static uint8_t storage_data[CONFIGURATION_SIZE]; 
  storage_read(0, storage_data, size); // read whole storage

  memcpy(storage_data + offset, data, length);

  uint32_t checksum = checksum_compute(storage_data, OFFSET_CHECKSUM);

  // apend checksum to buffer
  memcpy(storage_data + OFFSET_CHECKSUM, (uint8_t*)(&checksum), 4);

  storage_erase();

  static uint8_t context;
  context = reboot ? 1 : 0;

  uint32_t ret_code = nrf_fstorage_write(
    &m_storage,
    m_storage.start_addr,
    (uint32_t *)storage_data,
    CONFIGURATION_SIZE,
    &context
  );

  APP_ERROR_CHECK(ret_code);
}

void storage_store_pin_configuration(uint8_t *data) {
  storage_store(OFFSET_PIN_CONFIGURATION, data, PIN_CONFIGURATION_LENGTH, true);
}

void storage_store_connection_params_configuration(const uint8_t *data) {
  storage_store(OFFSET_CONNECTION_PARAMS_CONFIGURATION, data, 10, true);
}

void storage_store_device_name(const uint8_t *name, int length) {
  uint8_t name_buffer[LENGTH_DEVICE_NAME];
  memcpy(name_buffer, name, MIN(length, LENGTH_DEVICE_NAME));
  if(length < LENGTH_DEVICE_NAME){
    name_buffer[length] = 0;
  }

  storage_store(OFFSET_DEVICE_NAME, name_buffer, LENGTH_DEVICE_NAME, true);
}