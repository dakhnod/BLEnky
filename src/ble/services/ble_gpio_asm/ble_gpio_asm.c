#include "ble_gpio_asm.h"
#include "ble_helpers.h"
#include "encoding.h"
#include "app_error.h"
#include "ble_automation_io_service.h"
#include "sequence.h"

uint16_t ble_gpio_asm_connection_handle;
uint16_t ble_gpio_asm_service_handle;
uint16_t ble_gpio_asm_characteristic_data_handle;

uint8_t ble_gpio_asm_custom_uuid_type;

gpioasm_engine_t engine;

ret_code_t ble_gpio_asm_characteristic_asm_data_add()
{
  ble_helper_characteristic_init_t init = {
    .service_handle = ble_gpio_asm_service_handle,
    .uuid = UUID_GPIO_ASM_DATA,
    .uuid_type = ble_gpio_asm_custom_uuid_type,
    .description_str = "gpioASM data",
    .is_writable = true,
    .authorize_write = true,
    .max_length = 20,
    .value_handle = &ble_gpio_asm_characteristic_data_handle,
  };
  return ble_helper_characteristic_add(&init);
}


uint8_t ble_gpio_asm_handle_data_write(uint8_t *data, uint32_t length)
{
    static uint8_t is_overflown = false;

    if (length == 0)
    {
        gpioasm_stop(&engine);
        return is_overflown;
    }

    uint8_t result = gpioasm_push_packet(&engine, data, length);
    if (result == PUSH_OVERFLOW)
    {
        NRF_LOG_ERROR("buffer overflown\n");
        is_overflown = true;
        return is_overflown;
    }
    if (result == PUSH_FIRST_PACKET)
    {
        is_overflown = false;
        NRF_LOG_DEBUG("first packet\n");
        return is_overflown;
    }
    if (result == PUSH_FINAL_PACKET)
    {
        if (is_overflown)
        {
            NRF_LOG_ERROR("not starting sequence due to overflow\n");
            return is_overflown;
        }
        NRF_LOG_DEBUG("last packet\n");
        gpioasm_start(&engine);
    }
    return is_overflown;
}

void ble_gpio_asm_handle_input_change(uint32_t index, gpio_config_input_digital_t *config)
{
    gpioasm_handle_digital_input_update(&engine, index, config->state);
}

void ble_gpio_asm_on_connect(ble_evt_t *p_ble_evt)
{
    ble_gpio_asm_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

void ble_gpio_asm_on_disconnect(ble_evt_t *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    ble_gpio_asm_connection_handle = BLE_CONN_HANDLE_INVALID;
}


void ble_gpio_asm_authorize_data_write(ble_gatts_evt_write_t *write_req)
{
    uint16_t status = BLE_GATT_STATUS_SUCCESS;

    ble_gatts_rw_authorize_reply_params_t authorize_params = {
        .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
        .params.write = {
            .update = 1,
            .offset = 0,
            .len = write_req->len,
            .p_data = write_req->data}};

    if(ble_gpio_asm_handle_data_write(write_req->data, write_req->len)){
        status = BLE_GATT_STATUS_ATTERR_INVALID_ATT_VAL_LENGTH;
    }

    authorize_params.params.write.gatt_status = status;

    sd_ble_gatts_rw_authorize_reply(
        ble_gpio_asm_connection_handle,
        &authorize_params);
}

void ble_gpio_asm_on_authorize(ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_rw_authorize_request_t *req = &(p_ble_evt
                                                      ->evt.gatts_evt
                                                      .params
                                                      .authorize_request);

    if (req->type == BLE_GATTS_AUTHORIZE_TYPE_READ)
    {
        /*
        uint16_t handle = req
                              ->request
                              .read
                              .handle;
        */
    }
    else if (req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
    {
        uint16_t handle = req
                              ->request
                              .write
                              .handle;

        if (handle == ble_gpio_asm_characteristic_data_handle)
        {
            ble_gpio_asm_authorize_data_write(&(req->request.write));
            return;
        }
        return;
    }
}

void ble_gpio_asm_on_ble_evt(ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        ble_gpio_asm_on_connect(p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        ble_gpio_asm_on_disconnect(p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        // ble_gpio_asm_on_write(p_ble_evt);
        break;

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        ble_gpio_asm_on_authorize(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}

void ble_gpio_asm_init(){
    ret_code_t err_code;

    ble_uuid128_t vs_uuid = {
        .uuid128 = UUID_GPIO_ASM_BASE
    };

    err_code = sd_ble_uuid_vs_add(&vs_uuid, &ble_gpio_asm_custom_uuid_type);
    APP_ERROR_CHECK(err_code);

    ble_uuid_t uuid_service = {
        .uuid = UUID_GPIO_ASM_SERVICE,
        .type = ble_gpio_asm_custom_uuid_type
    };

    err_code = sd_ble_gatts_service_add(
        BLE_GATTS_SRVC_TYPE_PRIMARY,
        &uuid_service,
        &ble_gpio_asm_service_handle
    );
    APP_ERROR_CHECK(err_code);

    err_code = ble_gpio_asm_characteristic_asm_data_add();
    APP_ERROR_CHECK(err_code);

    gpioasm_init(&engine, NULL);
}