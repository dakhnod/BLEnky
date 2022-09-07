#include "sdk_common.h"
#include "ble_automation_io_service.h"
#include <string.h>
#include "ble_l2cap.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "storage.h"
#include "ble_binary_sensor_service.h"
#include "ble_configuration_service.h"
#include "ble_helpers.h"

uint16_t ble_aio_connection_handle = BLE_CONN_HANDLE_INVALID;

uint16_t ble_aio_service_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_digital_out_write_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_aio_digital_out_cccd_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_digital_in_write_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_aio_digital_in_cccd_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_digital_sequence_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_aio_digital_sequence_cccd_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_combined_sequence_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_analog_out_write_handles[GPIO_OUTPUT_ANALOG_PIN_LIMIT];

uint8_t ble_aio_send_digital_input_updates = false;
uint8_t ble_aio_send_digital_output_sequence_updates = false;
uint32_t ble_aio_output_analog_pin_count;

void ble_aio_on_connect(ble_evt_t *p_ble_evt) {
    ble_aio_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

void ble_aio_on_disconnect(ble_evt_t *p_ble_evt) {
    UNUSED_PARAMETER(p_ble_evt);
    ble_aio_connection_handle = BLE_CONN_HANDLE_INVALID;
    ble_aio_send_digital_input_updates = false;
    ble_aio_send_digital_output_sequence_updates = false;
}

void handle_pin_digital_in_cccd_write(ble_gatts_evt_write_t *write_evt) {
    if (write_evt->len == 2) {
        ble_aio_send_digital_input_updates = ble_srv_is_notification_enabled(write_evt->data);
    }
}

void handle_digital_out_sequence_cccd_write(ble_gatts_evt_write_t *write_evt) {
    if (write_evt->len == 2) {
        ble_aio_send_digital_output_sequence_updates = ble_srv_is_notification_enabled(write_evt->data);
    }
}

// return bit and following bit at index
uint8_t read_bits_at_index(uint8_t data, uint8_t index) {
    return (data & (0b11000000 >> index)) >> (6 - index); // & 1 at the end to enforce only last bit set
}

void ble_aio_handle_pin_data(
    uint8_t *pin_data, 
    uint32_t pin_data_length,
    uint16_t *pin_analog_data,
    uint32_t pin_analog_data_length
    ) {

    uint32_t available_output_count = gpio_get_output_digital_pin_count();
    uint32_t sent_output_count = pin_data_length * 4;

    uint32_t parsed_output_count = MIN(available_output_count, sent_output_count);

    for (int index = 0; index < parsed_output_count; index++) {
        uint32_t bit_index_full = index * 2;

        uint32_t byte_index = bit_index_full / 8;
        uint8_t bit_index = bit_index_full % 8;

        uint8_t current_byte = pin_data[byte_index];

        uint8_t output_bits = read_bits_at_index(current_byte, bit_index);

        if (output_bits == 0b11) {
            // don't touch state, 0b11 meand ignore
            continue;
        }
        if (output_bits == 0b10) {
            // tri-state not supported
            continue;
        }
        uint8_t new_state = (output_bits == 0b01);
        if (new_state == gpio_get_output_digital_state(index)) {
            continue;
        }
        gpio_write_output_digital_pin(index, new_state);
    }


    uint32_t available_output_analog_count = gpio_get_output_analog_pin_count();
    uint32_t parsed_output_analog_count = MIN(pin_analog_data_length, available_output_analog_count);

    for(uint32_t index = 0; index < parsed_output_analog_count; index++){
        uint16_t analog_value = pin_analog_data[index];
        if(analog_value == 0xffff){
            NRF_LOG_DEBUG("ignoring analog value %i\n", index);
            continue;
        }
        gpio_write_output_analog_pin_us(index, analog_value);
    }
}

void handle_digital_out_write(ble_gatts_evt_write_t *write_evt) {
    uint8_t *data = write_evt->data;
    uint32_t len = write_evt->len;

    ble_aio_handle_pin_data(data, len, NULL, 0);
}

void handle_digital_out_sequence_write(ble_gatts_evt_write_t *write_evt, uint8_t contains_analog) {
    uint8_t *data = write_evt->data;
    uint32_t len = write_evt->len;

    if (len < 3) {
        sequence_stop(true);
        return;
    }

    uint8_t result = sequence_push_packet(data, len);
    if (result == PUSH_SUCCESS) {
        return;
    }
    if (result == PUSH_OVERFLOW) {
        return;
    }
    if (result == PUSH_MISSED_PACKET) {
        return;
    }
    if (result == PUSH_FINAL_PACKET) {
        sequence_start(contains_analog);
        return;
    }
}

void handle_pin_analog_out_write(uint32_t index, ble_gatts_evt_write_t *write_evt){
    if(write_evt->len != 2){
        NRF_LOG_ERROR("wrong analog value\n");
        return;
    }

    uint16_t value = *((uint16_t*) write_evt->data);

    if(value == 0xffff){
        NRF_LOG_DEBUG("ignoring analog %i value\n", index);
    }

    gpio_write_output_analog_pin_us(index, value);
}

void ble_aio_authorize_digital_out_sequence() {
    uint8_t data[9];

    data[0] = sequence_is_running();
    *((uint32_t *)(data + 1)) = sequence_get_packet_index();
    *((uint32_t *)(data + 5)) = sequence_get_repeat_count();

    ble_gatts_rw_authorize_reply_params_t authorize_params = {
        .type = BLE_GATTS_AUTHORIZE_TYPE_READ,
        .params.read = {
            .gatt_status = BLE_GATT_STATUS_SUCCESS,
            .update = 1,
            .offset = 0,
            .len = 9,
            .p_data = data
        }
    };

    sd_ble_gatts_rw_authorize_reply(
        ble_aio_connection_handle,
        &authorize_params);
}

void ble_aio_authorize_digital_out() {
    uint32_t output_count = gpio_get_output_digital_pin_count();
    uint32_t data_length = ble_aio_get_byte_count_from_pins(output_count);

    uint8_t data[data_length];
    uint8_t output_states[output_count];

    gpio_encode_output_states(output_states);

    encode_states_to_bytes(output_states, output_count, data);

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

ret_code_t ble_aio_characteristic_digital_output_add() {
    return ble_helper_characteristic_digital_add(
        ble_aio_service_handle,
        UUID_DIGITAL_CHARACTERISTIC,
        BLE_UUID_TYPE_BLE,
        "Digital output",
        gpio_get_output_digital_pin_count(),
        0x01,
        true,
        true,
        false,
        true,
        false,
        ble_aio_get_byte_count_from_pins(gpio_get_output_digital_pin_count()),
        &ble_aio_digital_out_write_handle,
        &ble_aio_digital_out_cccd_handle);
}

ret_code_t ble_aio_characteristic_analog_output_add(uint32_t index) {
    return ble_helper_characteristic_digital_add(
        ble_aio_service_handle,
        UUID_ANALOG_CHARACTERISTIC,
        BLE_UUID_TYPE_BLE,
        "Analog output",
        0x00,
        (uint8_t)(index + 1),
        true,
        true,
        false,
        false,
        false,
        2,
        ble_aio_analog_out_write_handles + index,
        &ble_aio_digital_out_cccd_handle);
}

ret_code_t ble_aio_characteristic_digital_input_add() {
    return ble_helper_characteristic_digital_add(
        ble_aio_service_handle,
        UUID_DIGITAL_CHARACTERISTIC,
        BLE_UUID_TYPE_BLE,
        "Digital input",
        gpio_get_input_digital_pin_count(),
        0x02,
        false,
        true,
        true,
        true,
        false,
        ble_aio_get_byte_count_from_pins(gpio_get_input_digital_pin_count()),
        &ble_aio_digital_in_write_handle,
        &ble_aio_digital_in_cccd_handle);
}

ret_code_t ble_aio_characteristic_digital_output_sequence_add() {
    return ble_helper_characteristic_digital_add(
        ble_aio_service_handle,
        UUID_DIGITAL_CHARACTERISTIC,
        ble_configuration_service_get_custom_uuid_type(),
        "Digital output sequence",
        gpio_get_output_digital_pin_count(),
        0x00,
        true,
        true,
        true,
        true,
        false,
        20,
        &ble_aio_digital_sequence_handle,
        &ble_aio_digital_sequence_cccd_handle);
}

ret_code_t ble_aio_characteristic_combined_output_sequence_add() {
    return ble_helper_characteristic_digital_add(
        ble_aio_service_handle,
        UUID_COMBINED_SEQUENCE,
        ble_configuration_service_get_custom_uuid_type(),
        "Combined output sequence",
        gpio_get_output_digital_pin_count(),
        0x00,
        true,
        false,
        false,
        false,
        false,
        20,
        &ble_aio_combined_sequence_handle,
        NULL);
}

void ble_aio_on_write(ble_evt_t *p_ble_evt) {
    ble_gatts_evt_write_t *write_evt = &p_ble_evt
        ->evt
        .gatts_evt
        .params
        .write;

    uint16_t handle = write_evt->handle;

    if (handle == ble_aio_digital_out_write_handle) {
        handle_digital_out_write(write_evt);
        return;
    }
    if (handle == ble_aio_digital_sequence_handle) {
        handle_digital_out_sequence_write(write_evt, false);
        return;
    }
    if (handle == ble_aio_combined_sequence_handle) {
        handle_digital_out_sequence_write(write_evt, true);
        return;
    }
    if (handle == ble_aio_digital_sequence_cccd_handle) {
        handle_digital_out_sequence_cccd_write(write_evt);
        return;
    }
    if (handle == ble_aio_digital_in_cccd_handle) {
        handle_pin_digital_in_cccd_write(write_evt);
        return;
    }
    for(uint32_t i = 0; i < ble_aio_output_analog_pin_count; i++){
        if(handle == ble_aio_analog_out_write_handles[i]){
            handle_pin_analog_out_write(i, write_evt);
            return;
        }
    }
}

void ble_aio_authorize_digital_in() {
    uint32_t input_count = gpio_get_input_digital_pin_count();
    uint32_t data_length = ble_aio_get_byte_count_from_pins(input_count);

    uint8_t data[data_length];
    uint8_t input_states[input_count];

    gpio_encode_input_states(input_states);

    encode_states_to_bytes(input_states, input_count, data);

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
        &authorize_params);
}

void ble_aio_update_digital_in_states() {
    if (ble_aio_connection_handle == BLE_CONN_HANDLE_INVALID || (!ble_aio_send_digital_input_updates)) {
        return;
    }

    uint32_t input_count = gpio_get_input_digital_pin_count();
    uint32_t data_length = ble_aio_get_byte_count_from_pins(input_count);

    uint8_t data[data_length];
    uint8_t input_states[input_count];

    gpio_encode_input_states(input_states);

    encode_states_to_bytes(input_states, input_count, data);

    ret_code_t err_code;

    uint16_t len = data_length;

    ble_gatts_hvx_params_t params = {
        .handle = ble_aio_digital_in_write_handle,
        .type = BLE_GATT_HVX_NOTIFICATION,
        .offset = 0,
        .p_len = &len,
        .p_data = data
    };

    err_code = sd_ble_gatts_hvx(
        ble_aio_connection_handle,
        &params
    );
    APP_ERROR_CHECK(err_code);
}

uint32_t ble_aio_get_byte_count_from_pins(uint32_t pin_count) {
    return CEIL_DIV(pin_count, 4);
}

void write_bit(uint8_t *byte, uint32_t position, uint8_t value) {
    uint8_t shifted = 0b10000000 >> position;
    if (value) {
        (*byte) |= shifted;
    }
    else {
        (*byte) &= ~shifted;
    }
}

void encode_states_to_bytes(uint8_t *states, uint32_t state_count, uint8_t *buffer) {
    uint32_t byte_index = 0;
    uint32_t bit_index = 0;
    uint8_t *current_byte = 0;

    for (uint32_t i = 0; i < state_count; i++) {
        byte_index = i / 4;
        bit_index = (i % 4) * 2;

        uint8_t state = states[i];
        current_byte = buffer + byte_index;

        write_bit(current_byte, bit_index + 1, state);
        write_bit(current_byte, bit_index + 0, 0);
    }

    for (uint32_t i = bit_index + 2; i < 8; i++) {
        write_bit(current_byte, i, 1);
    }
}

void ble_aio_on_authorize(ble_evt_t *p_ble_evt) {
    ble_gatts_evt_rw_authorize_request_t req = p_ble_evt
        ->evt.gatts_evt
        .params
        .authorize_request;
    uint16_t handle = req
        .request
        .read
        .handle;

    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_READ) {
        if (handle == ble_aio_digital_out_write_handle) {
            ble_aio_authorize_digital_out();
            return;
        }
        if (handle == ble_aio_digital_in_write_handle) {
            ble_aio_authorize_digital_in();
            return;
        }
        if (handle == ble_aio_digital_sequence_handle) {
            ble_aio_authorize_digital_out_sequence();
            return;
        }
        return;
    }
}

void ble_aio_on_ble_evt(ble_evt_t *p_ble_evt) {
    switch (p_ble_evt->header.evt_id) {
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

void ble_aio_handle_input_change(uint32_t index, gpio_config_input_digital_t *config) {
    NRF_LOG_DEBUG("ble pin %d changed to %d\n", index, config->state);
    ble_aio_update_digital_in_states();

    if (index == 0) {
        ble_bss_set_state(config->state, (uint16_t)config->trigger_count);
    }
}

void ble_aio_handle_sequence_progress_update(uint8_t is_running, uint32_t packet_index, uint32_t repetitions_remaining) {
    uint16_t length = 9;
    uint8_t data[length];

    data[0] = is_running;
    *((uint32_t *)(data + 1)) = packet_index;
    *((uint32_t *)(data + 5)) = repetitions_remaining;

    if (ble_aio_connection_handle != BLE_CONN_HANDLE_INVALID && ble_aio_send_digital_output_sequence_updates) {
        ble_gatts_hvx_params_t params = {
            .handle = ble_aio_digital_sequence_handle,
            .type = BLE_GATT_HVX_NOTIFICATION,
            .offset = 0,
            .p_len = &length,
            .p_data = data
        };

        sd_ble_gatts_hvx(
            ble_aio_connection_handle,
            &params
        );
    }
}

ret_code_t ble_aio_init() {
    ret_code_t err_code;
    ble_uuid_t ble_uuid;

    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_AUTOMATION_IO_SERVICE);

    uint32_t output_digital_pin_count = gpio_get_output_digital_pin_count();
    uint32_t output_analog_pin_count = gpio_get_output_analog_pin_count();
    uint32_t input_digital_pin_count = gpio_get_input_digital_pin_count();

    ble_aio_output_analog_pin_count = output_analog_pin_count;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_aio_service_handle);
    APP_ERROR_CHECK(err_code);

    uint8_t init_sequence = false;

    if (output_digital_pin_count > 0) {
        err_code = ble_aio_characteristic_digital_output_add();
        APP_ERROR_CHECK(err_code);

        err_code = ble_aio_characteristic_digital_output_sequence_add();
        APP_ERROR_CHECK(err_code);

        init_sequence = true;
    }

    if (output_analog_pin_count > 0 || output_digital_pin_count > 0){
        for(uint32_t i = 0; i < output_analog_pin_count; i++){
            err_code = ble_aio_characteristic_analog_output_add(i);
        }

        err_code = ble_aio_characteristic_combined_output_sequence_add();
        APP_ERROR_CHECK(err_code);

        init_sequence = true;
    }

    if(init_sequence){
        sequence_init(
            ble_aio_get_byte_count_from_pins(output_digital_pin_count),
            output_analog_pin_count,
            ble_aio_handle_pin_data,
            ble_aio_handle_sequence_progress_update
        );
    }

    if (input_digital_pin_count > 0) {
        err_code = ble_aio_characteristic_digital_input_add();
        APP_ERROR_CHECK(err_code);

        ble_aio_update_digital_in_states();
    }

    gpio_set_input_change_handler(ble_aio_handle_input_change);

    if (input_digital_pin_count > 0) {
        err_code = ble_bss_init();
        APP_ERROR_CHECK(err_code);
    }

    return NRF_SUCCESS;
}
