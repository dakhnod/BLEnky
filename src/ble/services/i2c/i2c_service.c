#include "app_twi.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "ble_helpers.h"
#include "nrf_log.h"

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

bool send_i2c_response = false;

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
    .max_length = 19,
    .value_handle = &ble_i2c_rx_characteristic_handle,
    .cccd_handle = &ble_i2c_rx_cccd_handle
  };
  APP_ERROR_CHECK(ble_helper_characteristic_add(&init_rx));
}

void authorization_respond(bool success) {
    ble_gatts_rw_authorize_reply_params_t authorize_params = {
        .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
        .params.write = {
            .update = 1,
            .gatt_status = success ? BLE_GATT_STATUS_SUCCESS : BLE_GATT_STATUS_ATTERR_INVALID_ATT_VAL_LENGTH
        }
    };

    ret_code_t err_code = sd_ble_gatts_rw_authorize_reply(
        ble_i2c_connection_handle,
        &authorize_params
    );

    NRF_LOG_DEBUG("auth: %d\n", err_code);
}

void i2c_enable(){
    return nrf_drv_twi_enable(&twi_instance.twi);
}

void i2c_disable(){
    return nrf_drv_twi_disable(&twi_instance.twi);
}

ret_code_t i2c_write(uint8_t address, uint8_t *data, uint8_t length) {    
    app_twi_transfer_t transfer = APP_TWI_WRITE(address, data, length, 0);

    NRF_LOG_DEBUG("writing i2c\n");
    NRF_LOG_HEXDUMP_INFO(data, length);

    i2c_enable();
    // TODO: this should be async
    ret_code_t err_code = app_twi_perform(
        &twi_instance,
        &transfer,
        1,
        NULL
    );
    i2c_disable();

    return err_code;
}

ret_code_t i2c_read(uint8_t address, uint8_t register_address, uint8_t *buffer, uint8_t length) {
    app_twi_transfer_t transfers[] = {
        APP_TWI_WRITE(address, &register_address, 1, APP_TWI_NO_STOP),
        APP_TWI_READ(address, buffer, length, 0)
    };

    i2c_enable();
    // TODO: this should be async
    ret_code_t err_code = app_twi_perform(
        &twi_instance,
        transfers,
        2,
        NULL
    );
    i2c_disable();

    return err_code;
}

void on_i2c_tx_authorize_write(ble_evt_t *ble_evt) {
    NRF_LOG_INFO("auth\n");

    ret_code_t err_code;

    ble_gatts_evt_rw_authorize_request_t *req = &(ble_evt
                                                    ->evt.gatts_evt
                                                    .params
                                                    .authorize_request);

    if (req->type != BLE_GATTS_AUTHORIZE_TYPE_WRITE)
    {
        return;
    }

    ble_gatts_evt_write_t *write_evt = &req
                        ->request
                        .write;

    if (write_evt->handle != ble_i2c_tx_characteristic_handle)
    {
        return;
    }

    uint8_t *data = write_evt->data;
    uint8_t len = write_evt->len;

    if(len < 2) {
        return;
    }

    uint8_t address = data[0];

    uint8_t is_write = (address & 0b10000000) == 0;

    // clear read bit
    address &= (~0b10000000);

    NRF_LOG_INFO("write: %x, addres: %x\n", is_write, address);

    if(is_write) {
        if(len < 3) {
            return;
        }

        err_code = i2c_write(address, data + 1, len - 1);

        authorization_respond(err_code == 0);
        return;
    }

    uint8_t buffer[19];

    uint16_t read_len = 1;

    if(len > 2) {
        read_len = data[2];
    }

    for(int i = 0; i < len; i++) {
        NRF_LOG_DEBUG("%x\n", data[i]);
    }
    NRF_LOG_DEBUG("reading %i bytes\n", read_len);

    err_code = i2c_read(address, data[1], buffer, read_len);


    authorization_respond(err_code == 0);

    ble_gatts_hvx_params_t params = {
        .handle = ble_i2c_rx_characteristic_handle,
        .type = BLE_GATT_HVX_NOTIFICATION,
        .offset = 0,
        .p_len = &read_len,
        .p_data = buffer
    };

    NRF_LOG_DEBUG("data: ");
    NRF_LOG_HEXDUMP_INFO(buffer, read_len);

    err_code = sd_ble_gatts_hvx(
        ble_i2c_connection_handle,
        &params
    );
    APP_ERROR_CHECK(err_code);
}

void ble_i2c_rx_cccd_on_write(ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t *write_evt = &p_ble_evt
                                            ->evt
                                            .gatts_evt
                                            .params
                                            .write;

    uint16_t handle = write_evt->handle;

    if (handle == ble_i2c_rx_cccd_handle)
    {
        if (write_evt->len == 2)
        {
            send_i2c_response = ble_srv_is_notification_enabled(write_evt->data);
            NRF_LOG_INFO("i2c response: %d\n", send_i2c_response);
        }
        return;
    }
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

    case BLE_GATTS_EVT_WRITE:
        ble_i2c_rx_cccd_on_write(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}

ret_code_t ble_i2c_init() {
    NRF_LOG_INFO("init i2c\n");
    ret_code_t err_code;

    nrf_drv_twi_config_t const config = {
       .sda                = 0,
       .scl                = 2,
       .frequency          = NRF_TWI_FREQ_100K,
       .clear_bus_init     = true
    };
    UNUSED_PARAMETER(err_code);
    UNUSED_PARAMETER(config);

    APP_TWI_INIT(&twi_instance, &config, 1, err_code);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_disable(&twi_instance.twi);

    i2c_add_service();

    i2c_add_characteristics();

    return NRF_SUCCESS;
}