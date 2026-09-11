#include "storage.h"
#include "app_timer.h"
#include "crc32.h"
#include "nrf_delay.h"
#include "feature_config.h"
#include "nrf_fstorage_sd.h"
#include "nrf_fstorage_nvmc.h"
#include "preconfiguration.h"
#include "fds.h"

#define FILE_ID_BLENKY 0x0000
#define RECORD_ID_PIN_SETTINGS 0x0001
#define RECORD_ID_CONNECTION_PARAMETERS 0x0002
#define RECORD_ID_DEVICE_NAME 0x0003

bool reboot_requested = false;

static void fds_evt_handler(fds_evt_t const * p_fds_evt)
{
    switch (p_fds_evt->id)
    {
      case FDS_EVT_INIT:
        NRF_LOG_DEBUG("fds init result: %d", p_fds_evt->result);
        break;
      case FDS_EVT_UPDATE:
      case FDS_EVT_WRITE: {
        NRF_LOG_DEBUG("fds write result: %d", p_fds_evt->result);
        if ((p_fds_evt->result == NRF_SUCCESS) && reboot_requested) {
          NVIC_SystemReset();
        }
      }
      default:
        break;
  }
}

void storage_read(uint8_t record_id, uint8_t *buffer, uint8_t *length, bool *file_found) {
  // casting p_start_addr, so that offset calculation does not add offset * sizeof(uint32_t)
  ret_code_t err_code;

  fds_record_desc_t record_desc;
  fds_find_token_t find_token;

  NRF_LOG_DEBUG("looking for record %d", record_id);

  err_code = fds_record_find(FILE_ID_BLENKY, record_id, &record_desc, &find_token);
  if (err_code == FDS_ERR_NOT_FOUND) {
    NRF_LOG_DEBUG("BLEnky file not found", err_code);
    *file_found = false;
    return;
  }
  APP_ERROR_CHECK(err_code);

  fds_flash_record_t flash_record;
  err_code = fds_record_open(&record_desc, &flash_record);
  APP_ERROR_CHECK(err_code);

  uint8_t data_length = ((uint8_t*)flash_record.p_data)[0];
  uint8_t read_length = MIN(*length, data_length);

  memcpy(buffer, flash_record.p_data + 1, read_length);

  err_code = fds_record_close(&record_desc);

  *length = read_length;
  *file_found = true;
}

void storage_init() {
  ret_code_t err_code;
  NRF_LOG_DEBUG("initializing fds...");

  err_code = fds_register(fds_evt_handler);
  APP_ERROR_CHECK(err_code);

  err_code = fds_init();
  NRF_LOG_DEBUG("fds init return: %d", err_code);
  APP_ERROR_CHECK(err_code);

  nrf_delay_us(10000);
}

void storage_read_pin_configuration(uint8_t *buffer) {
  uint8_t length = PIN_CONFIGURATION_LENGTH;
  bool configuration_present;
  storage_read(RECORD_ID_PIN_SETTINGS, buffer, &length, &configuration_present);

  if (!configuration_present) {
    memset(buffer, 0xFF, PIN_CONFIGURATION_LENGTH);
    preconfiguration_load(buffer);
  }
}

void storage_read_connection_params_configuration(uint8_t *buffer, bool *configuration_present) {
  uint8_t length = 10;
  storage_read(RECORD_ID_CONNECTION_PARAMETERS, buffer, &length, configuration_present);
}

void storage_read_device_name(uint8_t *buffer, uint8_t *length, bool *configuration_present) {
  storage_read(RECORD_ID_DEVICE_NAME, buffer, length, configuration_present);
}

void storage_store(uint8_t record_key, const uint8_t *data, uint32_t length, const uint8_t reboot) {
  ret_code_t err_code;
  reboot_requested = reboot;

  static uint8_t internal_buffer[256];

  internal_buffer[0] = length;
  memcpy(internal_buffer + 4, data, length);

  if ((length % 4) != 0) {
    length += 4 - (length % 4);
  }

  fds_record_desc_t desc;
  fds_find_token_t find_token;
  
  const fds_record_t record = {
    .data = {
      .p_data = internal_buffer,
      .length_words = length / 4
    },
    .file_id = FILE_ID_BLENKY,
    .key = record_key
  };

  err_code = fds_record_find(FILE_ID_BLENKY, record_key, &desc, &find_token);

  if (err_code == FDS_ERR_NOT_FOUND) {
    err_code = fds_record_write(&desc, &record);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("record created at 0x%x", desc.p_record);
    return;
  }
  APP_ERROR_CHECK(err_code);

  err_code = fds_record_update(&desc, &record);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_DEBUG("record updated at 0x%x", desc.p_record);
}

void storage_store_pin_configuration(uint8_t *data) {
  uint8_t length = PIN_CONFIGURATION_LENGTH;
  storage_store(RECORD_ID_PIN_SETTINGS, data, length, true);
}

void storage_store_connection_params_configuration(const uint8_t *data) {
  storage_store(RECORD_ID_CONNECTION_PARAMETERS, data, 10, true);
}

void storage_store_device_name(const uint8_t *name, uint8_t length) {
  storage_store(RECORD_ID_DEVICE_NAME, name, MIN(length, LENGTH_DEVICE_NAME), true);
}