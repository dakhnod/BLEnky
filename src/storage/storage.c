#include "storage.h"
#include "app_timer.h"
#include "ble_configuration.h"
#include "fstorage.h"
#include "fds.h"
#include "math.h"

APP_TIMER_DEF(reboot_timer);
#define REBOOT_TIMEOUT APP_TIMER_TICKS(500, APP_TIMER_PRESCALER)

#define FILE_ID_SETTINGS 0x0000
#define RECORD_ID_DEVICE_NAME 0x0001
#define RECORD_ID_CONNECTION_SETTINGS 0x0002
#define RECORD_ID_PIN_CONFIGURATION 0x0003

uint8_t reboot_requested = false;


void fds_evt_handler(fds_evt_t const * const p_fds_evt)
{
    switch (p_fds_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_fds_evt->result == FDS_SUCCESS)
            {
                NRF_LOG_DEBUG("fds init success\n");
            }else{
                NRF_LOG_ERROR("fds init error: %d\n", p_fds_evt->result);
            }
            break;
        case FDS_EVT_WRITE:
            if (p_fds_evt->result == FDS_SUCCESS)
            {
              NRF_LOG_DEBUG("fds write success\n");
              if (reboot_requested) {
                NRF_LOG_DEBUG("reboot requested, rebooting...\n");
                ret_code_t err_code = app_timer_start(reboot_timer, REBOOT_TIMEOUT, NULL);
                APP_ERROR_CHECK(err_code);
                return;
              }
            }else{
                NRF_LOG_ERROR("fds write error: %d\n", p_fds_evt->result);
            }
            break;
        default:
            break;
    }
}

void storage_init() {
  ret_code_t err_code;
  
  err_code = fds_register(fds_evt_handler);
  APP_ERROR_CHECK(err_code);

  err_code = fds_init();
  APP_ERROR_CHECK(err_code);

  err_code = app_timer_create(
    &reboot_timer,
    APP_TIMER_MODE_SINGLE_SHOT,
    (app_timer_timeout_handler_t)NVIC_SystemReset
  );
  APP_ERROR_CHECK(err_code);
}

void storage_on_sys_evt(uint32_t sys_evt) {
  fs_sys_event_handler(sys_evt);
}

ret_code_t storage_read(uint16_t file, uint16_t record_key, uint8_t *buffer, uint32_t length) {
  ret_code_t err_code;

  fds_flash_record_t  flash_record;
  fds_record_desc_t   record_desc;
  fds_find_token_t    ftok = {0};

  err_code = fds_record_find(file, record_key, &record_desc, &ftok);

  if(err_code == FDS_ERR_NOT_FOUND){
    return err_code;
  }
  APP_ERROR_CHECK(err_code);

  err_code = fds_record_open(&record_desc, &flash_record);
  APP_ERROR_CHECK(err_code);

  memcpy(buffer, flash_record.p_data, length);

  err_code = fds_record_close(&record_desc);
  APP_ERROR_CHECK(err_code);

  return NRF_SUCCESS;
}

ret_code_t storage_store(uint16_t file, uint16_t record_key, uint8_t *data, uint32_t length, uint8_t reboot) {
  reboot_requested = reboot;

  static uint8_t __ALIGN(4) buffer[32]; // should fit the biggest possible item
  memcpy(buffer, data, length);

  fds_record_desc_t record_desc;

  fds_record_chunk_t record_chunk = {
    .p_data = data,
    .length_words = ceil(length / 4.0)
  };

  fds_record_t record = {
    .file_id = file,
    .key = record_key,
    .data = {
      .num_chunks = 1,
      .p_chunks = &record_chunk
    }
  };

  ret_code_t err_code;

  err_code = fds_record_update(&record_desc, &record);

  APP_ERROR_CHECK(err_code);

  return NRF_SUCCESS;
}

ret_code_t storage_read_pin_configuration(uint8_t *buffer) {
  return storage_read(FILE_ID_SETTINGS, RECORD_ID_PIN_CONFIGURATION, buffer, 16);
}

ret_code_t storage_read_connection_params_configuration(uint8_t *buffer) {
  return storage_read(FILE_ID_SETTINGS, RECORD_ID_CONNECTION_SETTINGS, buffer, 10);
}

ret_code_t storage_read_device_name(uint8_t *buffer, uint32_t *length_) {
  ret_code_t err_code = storage_read(FILE_ID_SETTINGS, RECORD_ID_DEVICE_NAME, buffer, LENGTH_DEVICE_NAME);

  if(err_code != NRF_SUCCESS){
    *length_ = 0;
    return err_code;
  }

  uint32_t len = strlen((char*) buffer);
  *length_ = MIN(len, LENGTH_DEVICE_NAME);

  return NRF_SUCCESS;
}

void storage_store_pin_configuration(uint8_t *data) {
  storage_store(FILE_ID_SETTINGS, RECORD_ID_PIN_CONFIGURATION, data, 16, true);
}

void storage_store_connection_params_configuration(uint8_t *data) {
  storage_store(FILE_ID_SETTINGS, RECORD_ID_CONNECTION_SETTINGS, data, 10, true);
}

void storage_store_device_name(uint8_t *name, int length) {
  uint8_t name_buffer[LENGTH_DEVICE_NAME];
  memcpy(name_buffer, name, MIN(length, LENGTH_DEVICE_NAME));
  if(length < LENGTH_DEVICE_NAME){
    name_buffer[length] = 0;
  }

  storage_store(FILE_ID_SETTINGS, RECORD_ID_DEVICE_NAME, name, LENGTH_DEVICE_NAME, true);
}