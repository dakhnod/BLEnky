#include "app_twi.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "ble_helpers.h"

#define TRANSACTION_QUEUE_SIZE 1

#define UUID_I2C_BASE { 0x06, 0xC4, 0x44, 0x05, 0xD9, 0x7A, 0x45, 0x89, 0xBF, 0x5E, 0xF3, 0xE9, 0xA4, 0x37, 0x24, 0x2C } 
#define UUID_I2C_SERVICE 0x0000
#define UUID_I2C_TX_CHARACTERISTIC 0x0001
#define UUID_I2C_RX_CHARACTERISTIC 0x0002

app_twi_t twi_instance = APP_TWI_INSTANCE(0);

uint16_t ble_i2c_service_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_i2c_tx_characteristic_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_i2c_rx_characteristic_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_i2c_rx_cccd_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_i2c_connection_handle = BLE_GATT_HANDLE_INVALID;

uint8_t ble_i2c_custom_uuid_type;

void i2c_add_service() {
    ret_code_t err_code;

    ble_uuid128_t vs_uuid = {
        .uuid128 = UUID_I2C_BASE
    };

    err_code = sd_ble_uuid_vs_add(&vs_uuid, &ble_i2c_custom_uuid_type);
    APP_ERROR_CHECK(err_code);

    ble_uuid_t uuid_service = {
        .uuid = UUID_I2C_SERVICE,
        .type = ble_i2c_custom_uuid_type
    };

    err_code = sd_ble_gatts_service_add(
        BLE_GATTS_SRVC_TYPE_PRIMARY,
        &uuid_service,
        &ble_i2c_service_handle
    );
}

void i2c_add_characteristics(){
  ble_helper_characteristic_init_t init_tx = {
    .service_handle = ble_i2c_service_handle,
    .uuid = UUID_I2C_TX_CHARACTERISTIC,
    .is_writable = true,
    .authorize_write = true,
    .max_length = 19,
    .value_handle = &ble_i2c_tx_characteristic_handle
  };
  APP_ERROR_CHECK(ble_helper_characteristic_add(&init_tx));


  ble_helper_characteristic_init_t init_rx = {
    .service_handle = ble_i2c_service_handle,
    .uuid = UUID_I2C_RX_CHARACTERISTIC,
    .is_notifiable = true,
    .value_handle = &ble_i2c_rx_characteristic_handle,
    .cccd_handle = &ble_i2c_rx_cccd_handle
  };
  APP_ERROR_CHECK(ble_helper_characteristic_add(&init_rx));
}

void authorization_respond(bool success) {
    ble_gatts_rw_authorize_reply_params_t authorize_params = {
        .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
        .params.write = {
            .update = 0,
            .gatt_status = success
        }
    };

    sd_ble_gatts_rw_authorize_reply(
        ble_i2c_connection_handle,
        &authorize_params
    );
}

void i2c_write(uint8_t address, uint8_t *data, uint8_t length) {
    ret_code_t err_code;
    
    app_twi_transfer_t transfer = APP_TWI_WRITE(address, data, length, 0);

    // TODO: this should be async
    err_code = app_twi_perform(
        &twi_instance,
        &transfer,
        1,
        NULL
    );

    authorization_respond(err_code == 0);
}

void i2c_read(uint8_t address, uint8_t register_address, uint8_t *buffer, uint8_t length) {
    ret_code_t err_code;
    
    app_twi_transfer_t transfers[] = {
        APP_TWI_WRITE(address, &register_address, 1, 0),
        APP_TWI_READ(address, buffer, length, 0)
    };

    // TODO: this should be async
    err_code = app_twi_perform(
        &twi_instance,
        transfers,
        2,
        NULL
    );

    authorization_respond(err_code == 0);
}

void on_i2c_tx_authorize_write(ble_evt_t *ble_evt) {
    ret_code_t err_code;

    ble_gatts_evt_rw_authorize_request_t *req = &(ble_evt
                                                    ->evt.gatts_evt
                                                    .params
                                                    .authorize_request);

    if (req->type != BLE_GATTS_AUTHORIZE_TYPE_WRITE)
    {
        return;
    }

    ble_gatts_evt_write_t write_evt = req
                        ->request
                        .write;

    if (write_evt.handle != ble_i2c_tx_characteristic_handle)
    {
        return;
    }

    uint8_t *data = write_evt.data;
    uint8_t len = write_evt.len;

    if(len < 2) {
        return;
    }

    uint8_t address = data[0];

    uint8_t is_write = (address & 0b10000000) == 0b10000000;

    if(is_write) {
        i2c_write(address, data, len);
        return;
    }

    uint8_t buffer[19];

    uint16_t read_len = 1;

    if(len > 2) {
        read_len = data[2];
    }

    i2c_read(address, data[1], buffer, read_len);

    ble_gatts_hvx_params_t params = {
        .handle = ble_i2c_rx_characteristic_handle,
        .type = BLE_GATT_HVX_NOTIFICATION,
        .offset = 0,
        .p_len = &read_len,
        .p_data = buffer
    };

    err_code = sd_ble_gatts_hvx(
        ble_i2c_connection_handle,
        &params);
    APP_ERROR_CHECK(err_code);
}

void ble_i2c_on_ble_evt(ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        // ble_csc_on_connect(p_ble_evt);
        ble_i2c_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        // ble_csc_on_disconnect(p_ble_evt);
        ble_i2c_connection_handle = BLE_GATT_HANDLE_INVALID;
        break;

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        on_i2c_tx_authorize_write(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}

ret_code_t ble_i2c_init() {
    ret_code_t err_code;

    nrf_drv_twi_config_t const config = {
       .scl                = 1,
       .sda                = 0
    };

    APP_TWI_INIT(&twi_instance, &config, TRANSACTION_QUEUE_SIZE, err_code);
    APP_ERROR_CHECK(err_code);

    i2c_add_service();
    i2c_add_characteristics();

    return NRF_SUCCESS;
}