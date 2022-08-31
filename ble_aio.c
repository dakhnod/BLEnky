#include "sdk_common.h"
#include "ble_aio.h"
#include <string.h>
#include "ble_l2cap.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "storage.h"
#include "ble_bss.h"

uint16_t ble_aio_connection_handle = BLE_CONN_HANDLE_INVALID;

uint16_t ble_aio_service_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_digital_out_write_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_aio_digital_out_cccd_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_digital_in_write_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_aio_digital_in_cccd_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_digital_sequence_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_aio_digital_sequence_cccd_handle = BLE_GATT_HANDLE_INVALID;

uint16_t ble_aio_pin_configuration_handle = BLE_GATT_HANDLE_INVALID;
uint16_t ble_aio_pin_configuration_cccd_handle = BLE_GATT_HANDLE_INVALID;

bool ble_aio_send_digital_input_updates = false;

uint8_t custom_uuid_type;

void ble_aio_on_connect(ble_evt_t *p_ble_evt) {
    ble_aio_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

void ble_aio_on_disconnect(ble_evt_t *p_ble_evt) {
    UNUSED_PARAMETER(p_ble_evt);
    ble_aio_connection_handle = BLE_CONN_HANDLE_INVALID;
    ble_aio_send_digital_input_updates = false;
}

void handle_pin_digital_in_cccd_write(ble_gatts_evt_write_t *write_evt) {
    if (write_evt->len == 2) {
        ble_aio_send_digital_input_updates = ble_srv_is_notification_enabled(write_evt->data);
        NRF_LOG_DEBUG("digital input notifications: %d\n", ble_aio_send_digital_input_updates);
    }
}

// return bit and following bit at index
uint8_t read_bits_at_index(uint8_t data, uint8_t index) {
    return (data & (0b11000000 >> index)) >> (6 - index); // & 1 at the end to enforce only last bit set
}

void handle_digital_out_write(ble_gatts_evt_write_t *write_evt) {
    NRF_LOG_DEBUG("written to digital output\n");

    uint8_t *data = write_evt->data;
    uint32_t len = write_evt->len;

    uint32_t available_output_count = gpio_get_output_pin_count();
    uint32_t sent_output_count = len * 4;

    uint32_t parsed_output_count = MIN(available_output_count, sent_output_count);

    NRF_LOG_DEBUG("sent %d pins, available %d pins, parsing %d pins\n", sent_output_count, available_output_count, parsed_output_count);

    for (int index = 0; index < parsed_output_count; index++) {
        uint32_t bit_index_full = index * 2;

        uint32_t byte_index = bit_index_full / 8;
        uint8_t bit_index = bit_index_full % 8;

        uint8_t current_byte = data[byte_index];

        uint8_t output_bits = read_bits_at_index(current_byte, bit_index);

        if (output_bits == 0b11) {
            // don't touch state, 0b11 meand ignore
            NRF_LOG_DEBUG("skipping pin #%d, not supported\n", index);
            continue;
        }
        if (output_bits == 0b10) {
            // tri-state not supported
            NRF_LOG_DEBUG("skipping pin #%d, tri-state\n", index);
            continue;
        }
        uint8_t new_state = (output_bits == 0b01);
        if (new_state == gpio_get_output_state(index)) {
            NRF_LOG_DEBUG("skipping pin #%d, no change\n", index);
            continue;
        }

        NRF_LOG_DEBUG("updating pin #%d to %d\n", index, new_state);
        gpio_write_output_pin(index, new_state);
    }
}

void handle_pin_configuration_write(ble_gatts_evt_write_t *write_evt) {
    NRF_LOG_DEBUG("written to pin configuration\n");

    uint8_t *data = write_evt->data;
    uint32_t len = write_evt->len;

    uint32_t writable_data_length = MIN(16, len);
    static uint8_t data_to_write[16];
    uint32_t start_index = 16 - writable_data_length;

    memcpy(data_to_write + start_index, data, writable_data_length);

    for (uint32_t i = 0; i < start_index; i++) {
        data_to_write[i] = 0xFF;
    }

    storage_store(data_to_write, 16);
}

void handle_digital_out_sequence_write(ble_gatts_evt_write_t *write_evt) {
    NRF_LOG_DEBUG("written to digital output sequence\n");

    uint8_t *data = write_evt->data;
    uint32_t len = write_evt->len;

    for (int i = 0; i < len; i++) {
        NRF_LOG_DEBUG("%x\n", data[i]);
    }

    uint8_t result = sequence_push_packet(data, len);
    if (result == PUSH_SUCCESS) {
        NRF_LOG_DEBUG("successfully pushed packet\n");
        return;
    }
    if (result == PUSH_OVERFLOW) {
        NRF_LOG_DEBUG("sequence buffer overflown\n");
        return;
    }
    if (result == PUSH_MISSED_PACKET) {
        NRF_LOG_DEBUG("push missed packet\n");
        return;
    }
    if (result == PUSH_FINAL_PACKET) {
        NRF_LOG_DEBUG("received final packet\n");
        sequence_start();
        return;
    }
}

void ble_aio_authorize_digital_out() {
    uint32_t output_count = gpio_get_output_pin_count();
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
            .p_data = data}
    };

    sd_ble_gatts_rw_authorize_reply(
        ble_aio_connection_handle,
        &authorize_params);
}

ret_code_t ble_aio_characteristic_digital_output_add() {
    return ble_aio_characteristic_digital_add(
        UUID_DIGITAL_CHARACTERISTIC,
        "Digital output",
        gpio_get_output_pin_count(),
        0x01,
        true,
        true,
        false,
        true,
        BLE_UUID_TYPE_BLE,
        ble_aio_get_byte_count_from_pins(gpio_get_output_pin_count()),
        &ble_aio_digital_out_write_handle,
        &ble_aio_digital_out_cccd_handle);
}

ret_code_t ble_aio_characteristic_digital_input_add() {
    return ble_aio_characteristic_digital_add(
        UUID_DIGITAL_CHARACTERISTIC,
        "Digital input",
        gpio_get_input_pin_count(),
        0x02,
        false,
        true,
        true,
        false,
        BLE_UUID_TYPE_BLE,
        ble_aio_get_byte_count_from_pins(gpio_get_input_pin_count()),
        &ble_aio_digital_in_write_handle,
        &ble_aio_digital_in_cccd_handle);
}

ret_code_t ble_aio_characteristic_digital_output_sequence_add() {
    return ble_aio_characteristic_digital_add(
        UUID_DIGITAL_CHARACTERISTIC,
        "Digital output sequence",
        gpio_get_output_pin_count(),
        0x00,
        true,
        false,
        false,
        false,
        custom_uuid_type,
        20,
        &ble_aio_digital_sequence_handle,
        &ble_aio_digital_sequence_cccd_handle);
}

ret_code_t ble_aio_characteristic_pin_configuration_add() {
    return ble_aio_characteristic_digital_add(
        UUID_PIN_CONFIG_CHARACTERISTIC,
        "Pin configuration",
        0,
        0x00,
        true,
        true,
        false,
        false,
        custom_uuid_type,
        16,
        &ble_aio_pin_configuration_handle,
        &ble_aio_pin_configuration_cccd_handle);
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
        handle_digital_out_sequence_write(write_evt);
        return;
    }
    if (handle == ble_aio_pin_configuration_handle) {
        handle_pin_configuration_write(write_evt);
        return;
    }
    if (handle == ble_aio_digital_in_cccd_handle) {
        handle_pin_digital_in_cccd_write(write_evt);
        return;
    }
}

void ble_aio_update_digital_in_states() {
    uint32_t input_count = gpio_get_input_pin_count();
    uint32_t data_length = ble_aio_get_byte_count_from_pins(input_count);

    uint8_t data[data_length];
    uint8_t input_states[input_count];

    gpio_encode_input_states(input_states);

    encode_states_to_bytes(input_states, input_count, data);

    ret_code_t err_code;

    if (ble_aio_connection_handle == BLE_CONN_HANDLE_INVALID || (!ble_aio_send_digital_input_updates)) {
        ble_gatts_value_t value = {
            .offset = 0,
            .len = data_length,
            .p_value = data
        };

        err_code = sd_ble_gatts_value_set(
            BLE_CONN_HANDLE_INVALID,
            ble_aio_digital_in_write_handle,
            &value
        );
        APP_ERROR_CHECK(err_code);

        return;
    }

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
    NRF_LOG_DEBUG("read authorize request\n");

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
            NRF_LOG_DEBUG("requesting digital out read\n");
            ble_aio_authorize_digital_out();
            return;
        }
        if (handle == ble_aio_digital_in_write_handle) {
            NRF_LOG_DEBUG("requesting digital in read\n");
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

    ble_bss_on_ble_evt(p_ble_evt);
}

void ble_aio_handle_input_change(uint32_t index, gpio_config_input_t *config) {
    NRF_LOG_DEBUG("ble pin %d changed to %d\n", index, config->state);
    ble_aio_update_digital_in_states();

    if (index == 0) {
        ble_bss_set_state(config->state, (uint16_t)config->state);
    }
}

ret_code_t ble_aio_pin_configuraion_data_set(uint8_t *data, uint32_t data_length) {
    ble_gatts_value_t value = {
        .offset = 0,
        .len = data_length,
        .p_value = data
    };

    return sd_ble_gatts_value_set(
        BLE_CONN_HANDLE_INVALID,
        ble_aio_pin_configuration_handle,
        &value
    );
}

ret_code_t ble_aio_characteristic_digital_add(
    uint16_t uuid,
    char *description_str,
    uint8_t number_of_digitals,
    uint8_t description,
    uint8_t is_writable,
    uint8_t is_readable,
    uint8_t is_notifiable,
    uint8_t authorize_read,
    uint8_t uuid_type,
    uint16_t max_length,
    uint16_t *value_handle,
    uint16_t *cccd_handle
) {
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
        .char_props.read = is_readable,
        .char_props.write = is_writable,
        .char_props.notify = is_notifiable,
        .char_props.indicate = 0,
        .p_char_user_desc = (uint8_t *)description_str,
        .char_user_desc_max_size = strlen(description_str),
        .char_user_desc_size = strlen(description_str),
        .p_cccd_md = &cccd_md,
        .p_char_pf = NULL,
        .p_user_desc_md = &user_description_metadata,
        .p_sccd_md = NULL
    };

    if (description > 0) {
        char_md.p_char_pf = &char_pf;
    }

    ble_uuid_t ble_uuid = {
        .type = uuid_type,
        .uuid = uuid
    };

    ble_gatts_attr_md_t attr_md = {
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = authorize_read,
        .wr_auth = 0,
        .vlen = 1,
    };
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    ble_gatts_attr_t attr_char_value = {
        .p_uuid = &ble_uuid,
        .p_attr_md = &attr_md,
        .max_len = max_length
    };
    ret_code_t err_code = sd_ble_gatts_characteristic_add(ble_aio_service_handle,
        &char_md,
        &attr_char_value,
        &p_handles);

    VERIFY_SUCCESS(err_code);

    *value_handle = p_handles.value_handle;
    *cccd_handle = p_handles.cccd_handle;

    if (number_of_digitals > 0) {
        uint16_t number_of_digitals_handle;

        ble_gatts_attr_md_t number_of_digitals_metadata = {
            .vlen = 0x00,
            .vloc = BLE_GATTS_VLOC_STACK,
            .rd_auth = 0,
            .wr_auth = 0 };
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
            &number_of_digitals_handle);
    }

    return err_code;
}

ret_code_t ble_aio_init() {
    ret_code_t err_code;
    ble_uuid_t ble_uuid;

    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_AUTOMATION_IO_SERVICE);

    ble_uuid128_t vs_uuid = {
        .uuid128 = CUSTOM_UUID_BASE
    };

    uint32_t output_pin_count = gpio_get_output_pin_count();
    uint32_t input_pin_count = gpio_get_input_pin_count();

    err_code = sd_ble_uuid_vs_add(&vs_uuid, &custom_uuid_type);
    VERIFY_SUCCESS(err_code);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_aio_service_handle);
    VERIFY_SUCCESS(err_code);

    if (output_pin_count > 0) {
        err_code = ble_aio_characteristic_digital_output_add();
        VERIFY_SUCCESS(err_code);

        err_code = ble_aio_characteristic_digital_output_sequence_add();
        VERIFY_SUCCESS(err_code);
    }

    if (input_pin_count > 0) {
        err_code = ble_aio_characteristic_digital_input_add();
        VERIFY_SUCCESS(err_code);

        ble_aio_update_digital_in_states();
    }

    err_code = ble_aio_characteristic_pin_configuration_add();
    VERIFY_SUCCESS(err_code);

    gpio_set_input_change_handler(ble_aio_handle_input_change);

    if (input_pin_count > 0) {
        err_code = ble_bss_init();
        VERIFY_SUCCESS(err_code);
    }

    return NRF_SUCCESS;
}