#include "sdk_common.h"
#include "ble_aio.h"
#include <string.h>
#include "ble_l2cap.h"
#include "ble_srv_common.h"
#include "app_error.h"

uint16_t ble_aio_connection_handle = BLE_CONN_HANDLE_INVALID;

uint16_t ble_aio_service_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_digital_out_write_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_aio_digital_out_cccd_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_digital_in_write_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_aio_digital_in_cccd_handle = BLE_GATT_HANDLE_INVALID;

bool ble_aio_send_responses = false;
bool ble_aio_send_updates = false;

void ble_aio_on_connect(ble_evt_t *p_ble_evt)
{
    ble_aio_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

void ble_aio_on_disconnect(ble_evt_t *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    ble_aio_connection_handle = BLE_CONN_HANDLE_INVALID;
    ble_aio_send_updates = false;
    ble_aio_send_responses = false;
}

void on_aio_cccd_write(ble_gatts_evt_write_t *p_evt_write)
{
    if (p_evt_write->len == 2)
    {
        ble_aio_send_responses = ble_srv_is_indication_enabled(p_evt_write->data);
    }
}

// return bit and following bit at index
uint8_t read_bits_at_index(uint8_t data, uint8_t index){
    return (data & (0b11000000 >> index)) >> (6 - index); // & 1 at the end to enforce only last bit set
}


#if OUTPUTS_NUMBER > 0
void handle_digital_out_write(ble_evt_t *p_ble_evt){
    NRF_LOG_DEBUG("written to digital output\n");

    ble_gatts_evt_write_t write_evt = p_ble_evt
        ->evt.gatts_evt
        .params
        .write;

    uint8_t *data = write_evt.data;
    uint32_t len = write_evt.len;

    uint32_t available_output_count = gpio_get_output_pin_count();
    uint32_t sent_output_count = len * 4;

    uint32_t parsed_output_count = MIN(available_output_count, sent_output_count);

    NRF_LOG_DEBUG("sent %d pins, available %d pins, parsing %d pins\n", sent_output_count, available_output_count, parsed_output_count);

    for(int index = 0; index < parsed_output_count; index++){
        uint32_t bit_index_full = index * 2;

        uint32_t byte_index = bit_index_full / 8;
        uint8_t bit_index = bit_index_full % 8;

        uint8_t current_byte = data[byte_index];

        uint8_t output_bits = read_bits_at_index(current_byte, bit_index);

        if(output_bits == 0b11){
            // don't touch state, 0b11 meand ignore
            NRF_LOG_DEBUG("skipping pin #%d, not supported\n", index);
            continue;
        }
        if(output_bits == 0b10){
            // tri-state not supported
            NRF_LOG_DEBUG("skipping pin #%d, tri-state\n", index);
            continue;
        }
        uint8_t new_state = (output_bits == 0b01);
        if(new_state == gpio_get_output_state(index)){
            NRF_LOG_DEBUG("skipping pin #%d, no change\n", index);
            continue;
        }

        NRF_LOG_DEBUG("updating pin #%d to %d\n", index, new_state);
        gpio_write_output_pin(index, new_state);
    }
}


void ble_aio_authorize_digital_out(){
    uint32_t output_count = gpio_get_output_pin_count();
    uint32_t data_length = ble_aio_get_byte_count_from_pins(output_count);

    uint8_t data[data_length];

    encode_states_to_bytes(gpio_get_output_states(), output_count, data);

    ble_gatts_rw_authorize_reply_params_t authorize_params = {
        .type = BLE_GATTS_AUTHORIZE_TYPE_READ,
        .params.read = {
            .gatt_status = BLE_GATT_STATUS_SUCCESS,
            .update = 1,
            .offset = 0,
            .len = data_length,
            .p_data = data
        }
    };
    
    sd_ble_gatts_rw_authorize_reply(
        ble_aio_connection_handle,
        &authorize_params
    );
}

ret_code_t ble_aio_characteristic_digital_output_add(){
    return ble_aio_characteristic_digital_add(
        "Digital output", 
        gpio_get_output_pin_count(),
        0x01,
        true, 
        false, 
        &ble_aio_digital_out_write_handle,
        &ble_aio_digital_out_cccd_handle
    );
}
#else
void handle_digital_out_write(ble_evt_t *p_ble_evt){}
void ble_aio_authorize_digital_out(){}
ret_code_t ble_aio_characteristic_digital_output_add(){
    return NRF_SUCCESS;
}
#endif

void ble_aio_on_write(ble_evt_t *p_ble_evt)
{
    uint16_t handle = p_ble_evt
        ->evt.gatts_evt
        .params
        .write
        .handle;

    if(handle == ble_aio_digital_out_write_handle){
        handle_digital_out_write(p_ble_evt);
        return;
    }
}

uint32_t ble_aio_get_byte_count_from_pins(uint32_t pin_count){ 
    return CEIL_DIV(pin_count, 4);
}

void write_bit(uint8_t *byte, uint32_t position, uint8_t value){
    uint8_t shifted = 0b10000000 >> position;
    if(value){
        (*byte) |= shifted;
    }else{
        (*byte) &= ~shifted;
    }
}

void encode_states_to_bytes(uint8_t *states, uint32_t state_count, uint8_t *buffer){
    uint32_t byte_index = 0;
    uint32_t bit_index = 0;
    uint8_t *current_byte = 0;

    for(uint32_t i = 0; i < state_count; i++){
        byte_index = i / 4;
        bit_index = (i % 4) * 2;

        uint8_t state = states[i];
        current_byte = buffer + byte_index;

        write_bit(current_byte, bit_index + 1, state);
        write_bit(current_byte, bit_index + 0, 0);
    }

    for(uint32_t i = bit_index + 2; i < 8; i++){
        write_bit(current_byte, i, 1);
    }
}

void ble_aio_on_authorize(ble_evt_t *p_ble_evt)
{
    
    uint16_t handle = p_ble_evt
        ->evt.gatts_evt
        .params
        .authorize_request
        .request
        .read
        .handle;


    if(handle == ble_aio_digital_out_write_handle) {
        NRF_LOG_DEBUG("requesting digital out\n");
        ble_aio_authorize_digital_out();
        return;
    }
    if(handle == ble_aio_digital_in_write_handle) {
        NRF_LOG_DEBUG("requesting digital in\n");
        return;
    }
}

void ble_aio_on_ble_evt(ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        ble_aio_on_connect(p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        ble_aio_on_disconnect(p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        ble_aio_on_write(p_ble_evt);
        break;

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        ble_aio_on_authorize(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}

ret_code_t ble_aio_characteristic_digital_input_add(){
    return ble_aio_characteristic_digital_add(
        "Digital input", 
        69,
        0x02,
        false, 
        true, 
        &ble_aio_digital_in_write_handle,
        &ble_aio_digital_in_cccd_handle
        );
}

ret_code_t ble_aio_characteristic_digital_add(
    char *description_str, 
    uint8_t number_of_digitals,
    uint8_t description,
    uint8_t is_writable, 
    uint8_t is_notifiable, 
    uint16_t *value_handle,
    uint16_t *cccd_handle
){
    ble_gatts_char_handles_t p_handles;

    ble_gatts_attr_md_t cccd_md = {
        .vloc = BLE_GATTS_VLOC_STACK
    };
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    ble_gatts_char_pf_t char_pf = {
        .unit = 0x2700,
        .format = BLE_GATT_CPF_FORMAT_STRUCT,
        .name_space = 0x01,
        .exponent = 0x00,
        .desc = description
    };

    ble_gatts_attr_md_t user_description_metadata = {
        .vlen = 0x00,
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = 0,
        .wr_auth = 0
    };
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&user_description_metadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&user_description_metadata.write_perm);

    ble_gatts_char_md_t char_md = {
        .char_props.read = 1,
        .char_props.write = is_writable,
        .char_props.notify = is_notifiable,
        .char_props.indicate = 0,
        .p_char_user_desc = (uint8_t *) description_str,
        .char_user_desc_max_size = strlen(description_str),
        .char_user_desc_size = strlen(description_str),
        .p_cccd_md = &cccd_md,
        .p_char_pf = &char_pf,
        .p_user_desc_md = &user_description_metadata,
        .p_sccd_md = NULL
    };

    ble_uuid_t ble_uuid = {
        .type = BLE_UUID_TYPE_BLE,
        .uuid = UUID_DIGITAL_CHARACTERISTIC
    };

    ble_gatts_attr_md_t attr_md = {
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = 1,
        .wr_auth = 0,
        .vlen = 1
    };
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    ble_gatts_attr_t attr_char_value = {
        .p_uuid = &ble_uuid,
        .p_attr_md = &attr_md,
        .max_len = ble_aio_get_byte_count_from_pins(number_of_digitals)
    };
    ret_code_t err_code = sd_ble_gatts_characteristic_add(ble_aio_service_handle,
                                                    &char_md,
                                                    &attr_char_value,
                                                    &p_handles);

    if(err_code != NRF_SUCCESS){
        return err_code;
    }

    *value_handle = p_handles.value_handle;
    *cccd_handle  = p_handles.cccd_handle;

    uint16_t number_of_digitals_handle;

    ble_gatts_attr_md_t number_of_digitals_metadata = {
        .vlen = 0x00,
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = 0,
        .wr_auth = 0
    };
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&number_of_digitals_metadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&number_of_digitals_metadata.write_perm);

    ble_uuid_t number_of_digitals_uuid = {
        .type = BLE_UUID_TYPE_BLE,
        .uuid = 0x2909
    };

    ble_gatts_attr_t number_of_digitals_descriptor_attributes = {
        .init_offs = 0,
        .init_len = 1,
        .max_len = 1,
        .p_uuid = &number_of_digitals_uuid,
        .p_value = &number_of_digitals,
        .p_attr_md = &number_of_digitals_metadata
    };

    sd_ble_gatts_descriptor_add(
        p_handles.value_handle,
        &number_of_digitals_descriptor_attributes,
        &number_of_digitals_handle
    );

    return err_code;
}

ret_code_t ble_aio_init()
{
    ret_code_t err_code;
    ble_uuid_t ble_uuid;

    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_AUTOMATION_IO_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_aio_service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = ble_aio_characteristic_digital_output_add();
        if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = ble_aio_characteristic_digital_input_add();
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}