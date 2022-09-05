#include "ble_configuration_service.h"
#include "app_error.h"
#include "ble_helpers.h"
#include "sensor_timer.h"
#include "storage.h"

uint8_t configuration_custom_uuid_type;

uint16_t ble_configuration_service_handle;

uint16_t ble_configuration_pin_configuration_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_configuration_connection_parameters_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_configuration_connection_handle = BLE_CONN_HANDLE_INVALID;

ble_configuration_connection_params_update_handler_t ble_configuration_connection_params_update_handler;

ret_code_t ble_configuration_characteristic_pin_configuration_add() {
  return ble_helper_characteristic_add(
    ble_configuration_service_handle,
    UUID_CHARACTERISTIC_PIN_CONFIGURATION,
    configuration_custom_uuid_type,
    "Pin configuration",
    true,
    true,
    false,
    false,
    false,
    16,
    &ble_configuration_pin_configuration_handle,
    NULL
  );
}

ret_code_t ble_configuration_characteristic_connections_parameters_add() {
  return ble_helper_characteristic_add(
    ble_configuration_service_handle,
    UUID_CHARACTERISTIC_CONNECITON_PARAMS_CONFIGURATION,
    configuration_custom_uuid_type,
    "Connection parameters configuration",
    true,
    true,
    false,
    false,
    true,
    10,
    &ble_configuration_connection_parameters_handle,
    NULL
  );
}

ret_code_t ble_configuration_characteristic_data_set(uint32_t handle, uint8_t *data, uint32_t data_length) {
  ble_gatts_value_t value = {
      .offset = 0,
      .len = data_length,
      .p_value = data
  };

  return sd_ble_gatts_value_set(
    BLE_CONN_HANDLE_INVALID,
    handle,
    &value
  );
}

ret_code_t ble_configuration_pin_configuraion_data_set(uint8_t data[16]) {
  return ble_configuration_characteristic_data_set(
    ble_configuration_pin_configuration_handle,
    data,
    16
  );
}

ret_code_t ble_configuration_connection_params_configuraion_data_set(uint8_t data[10]) {
  return ble_configuration_characteristic_data_set(
    ble_configuration_connection_parameters_handle,
    data,
    10
  );
}

void ble_configuration_restore_values() {
  ret_code_t err_code;

  uint8_t storage_data[16]; // size 16 to cover both endpoints
  storage_read_pin_configuration(storage_data);
  err_code = ble_configuration_pin_configuraion_data_set(storage_data);
  APP_ERROR_CHECK(err_code);

  storage_read_connection_params_configuration(storage_data);
  err_code = ble_configuration_connection_params_configuraion_data_set(storage_data);
  APP_ERROR_CHECK(err_code);
}

ret_code_t ble_configuration_service_init(ble_configuration_connection_params_update_handler_t connection_params_update_handler) {

  ret_code_t err_code;

  ble_uuid128_t vs_uuid = {
      .uuid128 = CUSTOM_UUID_BASE_CONFIGURATION_SERVICE
  };

  err_code = sd_ble_uuid_vs_add(&vs_uuid, &configuration_custom_uuid_type);
  APP_ERROR_CHECK(err_code);


  ble_uuid_t ble_uuid = {
    .type = configuration_custom_uuid_type,
    .uuid = UUID_SERVICE_CONFIGURATION
  };

  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_configuration_service_handle);
  APP_ERROR_CHECK(err_code);

  err_code = ble_configuration_characteristic_pin_configuration_add();
  APP_ERROR_CHECK(err_code);

  err_code = ble_configuration_characteristic_connections_parameters_add();
  APP_ERROR_CHECK(err_code);

  ble_configuration_restore_values();

  ble_configuration_connection_params_update_handler = connection_params_update_handler;

  return NRF_SUCCESS;
}

void ble_configuration_on_connect(ble_evt_t *p_ble_evt) {
  ble_configuration_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

void ble_configuration_on_disconnect(ble_evt_t *p_ble_evt) {
  ble_configuration_connection_handle = BLE_CONN_HANDLE_INVALID;
}

void ble_configuration_handle_connection_params_configuration_data(uint8_t *data) {
  ble_configuration_connection_params_packet_t *packet = (ble_configuration_connection_params_packet_t *)data;

  storage_store_connection_params_configuration(data);

  ble_configuration_connection_params_update_handler(packet);
}

void ble_configuration_authorize_connection_params_write(ble_gatts_evt_write_t *write_req) {
  uint16_t status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;

  ble_gatts_rw_authorize_reply_params_t authorize_params = {
    .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
    .params.write = {
        .update = 1,
        .offset = 0,
        .len = write_req->len,
        .p_data = write_req->data
      }
  };

  if (write_req->len != 10) {
    status = BLE_GATT_STATUS_ATTERR_INVALID_ATT_VAL_LENGTH;
  }
  else {
    ble_configuration_connection_params_packet_t *packet = (ble_configuration_connection_params_packet_t *)write_req->data;

    ble_gap_conn_params_t real_params = {
      .min_conn_interval = MSEC_TO_UNITS(packet->min_conn_interval, UNIT_1_25_MS),
      .max_conn_interval = MSEC_TO_UNITS(packet->max_conn_interval, UNIT_1_25_MS),
      .slave_latency = packet->slave_latency,
      .conn_sup_timeout = MSEC_TO_UNITS(packet->conn_sup_timeout, UNIT_10_MS)
    };

    if (real_params.min_conn_interval < BLE_GAP_CP_MIN_CONN_INTVL_MIN) {
      NRF_LOG_ERROR("min connection interval min too small\n");
    }
    else if (real_params.min_conn_interval > BLE_GAP_CP_MIN_CONN_INTVL_MAX) {
      NRF_LOG_ERROR("min connection interval min too big\n");
    }
    else if (real_params.max_conn_interval < BLE_GAP_CP_MAX_CONN_INTVL_MIN) {
      NRF_LOG_ERROR("max connection interval min too small\n");
    }
    else if (real_params.max_conn_interval > BLE_GAP_CP_MAX_CONN_INTVL_MAX) {
      NRF_LOG_ERROR("max connection interval min too big\n");
    }
    else if (real_params.min_conn_interval >= real_params.max_conn_interval) {
      NRF_LOG_ERROR("min connection interval must be smaller than max\n");
    }
    else if (real_params.slave_latency > BLE_GAP_CP_SLAVE_LATENCY_MAX) {
      NRF_LOG_ERROR("slave latency too big\n");
    }
    else if (real_params.conn_sup_timeout < BLE_GAP_CP_CONN_SUP_TIMEOUT_MIN) {
      NRF_LOG_ERROR("sup timeout too small\n");
    }
    else if (real_params.conn_sup_timeout > BLE_GAP_CP_CONN_SUP_TIMEOUT_MAX) {
      NRF_LOG_ERROR("sup timeout too big\n");
    }
    else if (packet->advertising_interval < BLE_GAP_ADV_INTERVAL_MIN) {
      NRF_LOG_ERROR("adv interval too small\n");
    }
    else if (packet->advertising_interval > BLE_GAP_ADV_INTERVAL_MAX) {
      NRF_LOG_ERROR("adv interval too big\n");
    }
    else if (packet->conn_sup_timeout <= (packet->max_conn_interval * (packet->slave_latency + 1))) {
      NRF_LOG_ERROR("sup timeout smaller than effective connection interval\n");
    }
    else {
      NRF_LOG_INFO("LGTM\n");
      ble_configuration_handle_connection_params_configuration_data(write_req->data);
      status = BLE_GATT_STATUS_SUCCESS;
    }
  }


  authorize_params.params.write.gatt_status = status;

  sd_ble_gatts_rw_authorize_reply(
    ble_configuration_connection_handle,
    &authorize_params
  );
}

void ble_configuration_on_authorize(ble_evt_t *p_ble_evt) {
  ble_gatts_evt_rw_authorize_request_t *req = &(p_ble_evt
    ->evt.gatts_evt
    .params
    .authorize_request);
  uint16_t handle = req
    ->request
    .write
    .handle;

  if (req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE) {
    ble_gatts_evt_write_t *write_req = &(req->request.write);
    if (handle == ble_configuration_connection_parameters_handle) {
      ble_configuration_authorize_connection_params_write(write_req);
      return;
    }
  }
}


void ble_configuration_handle_pin_configuration_write(ble_gatts_evt_write_t *write_evt) {
  uint8_t *data = write_evt->data;
  uint32_t len = write_evt->len;

  uint32_t writable_data_length = MIN(16, len);
  static uint8_t data_to_write[16];

  memcpy(data_to_write, data, writable_data_length);

  for (uint32_t i = writable_data_length; i < 16; i++) {
    data_to_write[i] = 0xFF;
  }

  storage_store_pin_configuration(data_to_write);
}

void ble_configuration_handle_connection_params_configuration_write(ble_gatts_evt_write_t *write_evt) {
  uint8_t *data = write_evt->data;
  uint32_t len = write_evt->len;

  NRF_LOG_DEBUG("fehler\n");

  if (len != 10) {
    NRF_LOG_ERROR("connection parameters config must be 20 bytes in length (%d)\n", len);
    return;
  }

  ble_configuration_handle_connection_params_configuration_data(data);
}

void ble_configuration_on_write(ble_evt_t *p_ble_evt) {
  ble_gatts_evt_write_t *write_evt = &p_ble_evt
    ->evt
    .gatts_evt
    .params
    .write;

  uint16_t handle = write_evt->handle;

  if (handle == ble_configuration_pin_configuration_handle) {
    ble_configuration_handle_pin_configuration_write(write_evt);
    return;
  }
  if (handle == ble_configuration_connection_parameters_handle) {
    ble_configuration_handle_connection_params_configuration_write(write_evt);
    return;
  }
}

void ble_configuration_on_ble_event(ble_evt_t *p_ble_evt) {

  switch (p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      ble_configuration_on_connect(p_ble_evt);
      break;

    case BLE_GAP_EVT_DISCONNECTED:
      ble_configuration_on_disconnect(p_ble_evt);
      break;

    case BLE_GATTS_EVT_WRITE:
      ble_configuration_on_write(p_ble_evt);
      break;

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
      ble_configuration_on_authorize(p_ble_evt);
      break;

    default:
      // No implementation needed.
      break;
  }
}

uint8_t ble_configuration_service_get_custom_uuid_type() {
  return configuration_custom_uuid_type;
}