#include "sdk_common.h"
#include "ble_bss.h"
#include <string.h>
#include "ble_l2cap.h"
#include "ble_srv_common.h"

uint16_t connection_handle = BLE_CONN_HANDLE_INVALID;
uint16_t service_handle;
uint16_t control_handle;
uint16_t cccd_handle;

bool send_responses = false;
bool send_updates = false;

static void on_connect(ble_evt_t *p_ble_evt)
{
    connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

static void on_disconnect(ble_evt_t *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    connection_handle = BLE_CONN_HANDLE_INVALID;
    send_updates = false;
    send_responses = false;
}

static void on_bss_cccd_write(ble_gatts_evt_write_t *p_evt_write)
{
    if (p_evt_write->len == 2)
    {
        send_responses = ble_srv_is_indication_enabled(p_evt_write->data);
    }
}

static void on_control_characteristic_write(ble_evt_t *p_ble_evt)
{
    uint16_t length = p_ble_evt->evt.gatts_evt.params.write.len;
    uint8_t *data = p_ble_evt->evt.gatts_evt.params.write.data;

    parse_full_packet_with_split_header(data, length);
}

static void parse_full_packet_with_split_header(uint8_t *data, uint16_t length)
{
    if (length < 1)
    {
        return;
    }
    uint8_t header = data[0];
    // here we could buffer split messages
    parse_packet(data + 1, length - 1);
}

static void parse_packet(uint8_t *data, uint16_t length)
{
    if (length < 4)
    {
        return;
    }
    message_header_t *message_header = data;
    message_parameter_t params[message_header->parameter_count];
    uint8_t *payload_ptr = data + 4;
    for (uint8_t i = 0; i < message_header->parameter_count; i++)
    {
        message_parameter_without_data_t *parameter = payload_ptr;

        params[i].parameter_id = parameter->parameter_id;
        params[i].parameter_length = parameter->parameter_length;
        params[i].data = payload_ptr + 4;

        payload_ptr += parameter->parameter_length + 4;
    }

    parse_packet_decoded(message_header->message_id, params, message_header->parameter_count);
}

static void parse_packet_decoded(enum message_id_t message_id, message_parameter_t *parameters, uint8_t parameter_count)
{
    switch (message_id)
    {
    case MESSAGE_ID_GET_SENSOR_STATUS_COMMAND:
        parse_get_sensor_command(parameters, parameter_count);
        break;
    case MESSAGE_ID_SET_SENSOR_STATUS_COMMAND:
        parse_set_sensor_command(parameters, parameter_count);
        break;

    default:
        break;
    }
}

static void parse_set_sensor_command(message_parameter_t *parameters, uint8_t parameter_count)
{
    if (parameter_count != 2)
    {
        respond_set_sensor_command(FAILURE);
    }
    enum sensor_type_t sensor_type = parameters[0].data[0];
    enum report_state_t report_state = parameters[1].data[0];

    handle_get_sensor_command(sensor_type);
}

static void handle_set_sensor_command(enum sensor_type_t sensor_type, enum report_state_t report_state){
    if(sensor_type != OPENING_CLOSING){
        respond_set_sensor_command(FAILURE);
    }

    send_updates = report_state;
    respond_set_sensor_command(OK);
};

static void parse_get_sensor_command(message_parameter_t *parameters, uint8_t parameter_count)
{
    if (parameter_count < 1)
    {
        respond_get_sensor_command(FAILURE, 0, 0);
    }
    enum sensor_type_t sensor_type = parameters[0].data[0];

    handle_get_sensor_command(sensor_type);
}

static void handle_get_sensor_command(enum sensor_type_t sensor_type)
{
    if (sensor_type == OPENING_CLOSING){
        handle_get_single_open_close_sensor_command();
    }else if(sensor_type == VIBRATION_DETECTION){
        handle_get_single_vibration_sensor_command();
    }else if(sensor_type == HUMAN_DETECTION){
        handle_get_single_human_sensor_command();
    }else if (sensor_type == MULTIPLE_OPENING_CLOSING){
        handle_get_multiple_open_close_sensor_command();
    }else if(sensor_type == MULTIPLE_VIBRATION_DETECTION){
        handle_get_multiple_vibration_sensor_command();
    }else if(sensor_type == MULTIPLE_HUMAN_DETECTION){
        handle_get_multiple_human_sensor_command();
    }
}

static void handle_get_multiple_open_close_sensor_command(){
    respond_get_sensor_command(FAILURE, 0, 0);
}

static void handle_get_multiple_human_sensor_command(){
    respond_get_sensor_command(FAILURE, 0, 0);
}

static void handle_get_multiple_vibration_sensor_command(){
    respond_get_sensor_command(FAILURE, 0, 0);
}

static void handle_get_single_human_sensor_command(){
    respond_get_sensor_command(FAILURE, 0, 0);
}

static void handle_get_single_vibration_sensor_command(){
    respond_get_sensor_command(FAILURE, 0, 0);
}

static void handle_get_single_open_close_sensor_command(){
    // TODO: simulation
    static bool open = false;
    static uint16_t count = 0;
    respond_get_sensor_command(OK, open, count);
    open ^= 1;
    count++;
}

static void respond_set_sensor_command(enum result_code_t result_code){
    respond_set_sensor(MESSAGE_ID_SET_SENSOR_STATUS_RESPONSE, result_code);
}

static void respond_get_sensor_command(enum result_code_t result_code, enum openinig_closing_state state, uint16_t count){
    respond_get_sensor(MESSAGE_ID_GET_SENSOR_STATUS_RESPONSE, result_code, state, count);
}

static void respond_get_sensor_event(enum result_code_t result_code, enum openinig_closing_state state, uint16_t count){
    respond_get_sensor(MESSAGE_ID_SENSOR_STATUS_EVENT, result_code, state, count);
}



ret_code_t send_message_with_header(enum message_id_t message_id, message_parameter_t *parameters, uint8_t parameter_count)
{
    int data_length = 5; // header

    for (uint8_t i = 0; i < parameter_count; i++)
    {
        data_length += parameters[i].parameter_length;
    }

    uint8_t data[data_length] = {
        0b10000001, // split flags: server->client | execute flag
        0x00,       // RFU
        message_id,
        0x00, // RFU
        parameter_count};

    uint8_t *buffer_ptr = data + 5;

    for (int i = 0; i < parameter_count; i++)
    {
        memcpy(buffer_ptr, parameters + i, 2);                                      // copy header
        memcpy(buffer_ptr + 4, parameters[i].data, parameters[i].parameter_length); // copy data
        buffer_ptr += parameters[i].parameter_length + 4;
    }

    return ble_bss_response_send(data, data_length);
}

static void respond_set_sensor(enum message_id_t message_id, enum result_code_t result_code)
{
    message_parameter_t parameters[] = {
        {
            .parameter_id = RESULT_CODE,
            .parameter_length = 1,
            .data = &result_code
        }
    };

    send_message_with_header(message_id, parameters, 1);
}

static void respond_get_sensor(enum message_id_t message_id, enum result_code_t result_code, enum openinig_closing_state state, uint16_t count)
{
    uint16_t data = (count & 0b11111111111) | (state << 11);

    message_parameter_t parameters[] = {
        {
            .parameter_id = RESULT_CODE,
            .parameter_length = 1,
            .data = &result_code
        },
        {
            .parameter_id = SENSOR_STATUS,
            .parameter_length = 2,
            .data = &data
         }
    };

    send_message_with_header(message_id, parameters, 2);
}

static void on_write(ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == cccd_handle)
    {
        on_bss_cccd_write(p_evt_write);
    }
    else if (p_evt_write->handle == control_handle)
    {
        on_control_characteristic_write(p_evt_write);
    }
}

void ble_bss_on_ble_evt(ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        on_connect(p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        on_disconnect(p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        on_write(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}

static uint32_t characteristic_control_point_add()

{
    ble_gatts_char_handles_t p_handles;

    ble_gatts_attr_md_t cccd_md = {
        .vloc = BLE_GATTS_VLOC_STACK};
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&cccd_md.write_perm);

    ble_gatts_char_md_t char_md = {
        .char_props.read = 0,
        .char_props.write = 1,
        .char_props.notify = 0,
        .p_char_user_desc = NULL,
        .p_char_pf = NULL,
        .p_user_desc_md = NULL,
        .p_cccd_md = &cccd_md,
        .p_sccd_md = NULL};

    ble_uuid_t ble_uuid = {
        .type = BLE_UUID_TYPE_BLE,
        .uuid = UUID_BINARY_SENSOR_CONTROL_POINT};

    ble_gatts_attr_md_t attr_md = {
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = 0,
        .wr_auth = 0,
        .vlen = 0};
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    ble_gatts_attr_t attr_char_value = {
        .p_uuid = &ble_uuid,
        .p_attr_md = &attr_md,
        .max_len = 19,
    };
    uint32_t code = sd_ble_gatts_characteristic_add(BLE_GATT_HANDLE_INVALID,
                                                    &char_md,
                                                    &attr_char_value,
                                                    &p_handles);

    control_handle = p_handles.value_handle;

    return code;
}

static uint32_t characteristic_response_add()

{
    ble_gatts_char_handles_t p_handles;

    ble_gatts_attr_md_t cccd_md = {
        .vloc = BLE_GATTS_VLOC_STACK};
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    ble_gatts_char_md_t char_md = {
        .char_props.read = 0,
        .char_props.write = 0,
        .char_props.notify = 0,
        .char_props.indicate = 1,
        .p_char_user_desc = NULL,
        .p_char_pf = NULL,
        .p_user_desc_md = NULL,
        .p_cccd_md = &cccd_md,
        .p_sccd_md = NULL};

    ble_uuid_t ble_uuid = {
        .type = BLE_UUID_TYPE_BLE,
        .uuid = UUID_BINARY_SENSOR_RESPONSE};

    ble_gatts_attr_md_t attr_md = {
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = 0,
        .wr_auth = 0,
        .vlen = 0};
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);

    ble_gatts_attr_t attr_char_value = {
        .p_uuid = &ble_uuid,
        .p_attr_md = &attr_md,
        .max_len = 1,
    };
    uint32_t code = sd_ble_gatts_characteristic_add(BLE_GATT_HANDLE_INVALID,
                                                    &char_md,
                                                    &attr_char_value,
                                                    &p_handles);

    control_handle = p_handles.value_handle;
    cccd_handle = p_handles.cccd_handle;

    return code;
}

uint32_t ble_bss_init()
{
    uint32_t err_code;
    ble_uuid_t ble_uuid;

    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_BINARY_SENSOR_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = characteristic_control_point_add();
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = characteristic_response_add();
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
}

ret_code_t send_sensor_status_response(bool is_closed, uint16_t count)
{
    return send_sensor_status(MESSAGE_ID_GET_SENSOR_STATUS_RESPONSE, is_closed, count);
}

ret_code_t send_sensor_status_event(bool is_closed, uint16_t count)
{
    return send_sensor_status(MESSAGE_ID_SENSOR_STATUS_EVENT, is_closed, count);
}

uint32_t ble_bss_response_send(uint8_t *data, uint8_t length)
{
    if (connection_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }
    if(!send_responses){
        return;
    }
    ble_gatts_hvx_params_t hvx_params = {
        .handle = connection_handle,
        .type = BLE_GATT_HVX_INDICATION,
        .offset = 0,
        .p_len = length,
        .p_data = data};

    return sd_ble_gatts_hvx(connection_handle, &hvx_params);
}