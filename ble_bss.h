#ifndef BLE_HRS_H__
#define BLE_HRS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_log.h"
// #include "nrf_ble_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UUID_BINARY_SENSOR_SERVICE       0x183B
#define UUID_BINARY_SENSOR_CONTROL_POINT 0x2B2B
#define UUID_BINARY_SENSOR_RESPONSE      0x2B2C

enum message_id_t {
    MESSAGE_ID_GET_SENSOR_STATUS_COMMAND,
    MESSAGE_ID_GET_SENSOR_STATUS_RESPONSE,
    MESSAGE_ID_SET_SENSOR_STATUS_COMMAND,
    MESSAGE_ID_SET_SENSOR_STATUS_RESPONSE,
    MESSAGE_ID_SENSOR_STATUS_EVENT
};

enum result_code_t {
    OK,
    FAILURE
};

enum sensor_type_t {
    OPENING_CLOSING,
    HUMAN_DETECTION,
    VIBRATION_DETECTION,
    MULTIPLE_OPENING_CLOSING = 0x80,
    MULTIPLE_HUMAN_DETECTION,
    MULTIPLE_VIBRATION_DETECTION,
};

enum parameter_id_t {
    RESULT_CODE,
    CANCEL,
    SENSOR_TYPE,
    REPORT_STATUS,
    SENSOR_STATUS = 0x0A,
    MULTIPLE_SENSOR_STATUS,
    NAME
};

enum opening_closing_state_t {
    CLOSED,
    OPEN
};

enum report_state_t {
    OFF,
    ON
};

typedef struct {
    uint8_t parameter_id;
    uint8_t parameter_length;
    uint16_t reserved;
    uint8_t *data;
} message_parameter_t;

typedef struct {
    uint8_t parameter_id;
    uint8_t parameter_length;
} message_parameter_without_data_t;

typedef struct {
    uint8_t rfu_1;
    uint8_t message_id;
    uint8_t rfu_2;
    uint8_t parameter_count;
} message_header_t;



void parse_full_packet_with_split_header(uint8_t *data, uint16_t length);
void parse_packet_decoded(enum message_id_t message_id, message_parameter_t *parameters, uint8_t parameter_count);
void parse_packet(uint8_t *data, uint16_t length);
void parse_set_sensor_command(message_parameter_t *parameters, uint8_t parameter_count);
void handle_set_sensor_command(enum sensor_type_t sensor_type, enum report_state_t report_state);
void parse_get_sensor_command(message_parameter_t *parameters, uint8_t parameter_count);
void handle_get_sensor_command(enum sensor_type_t sensor_type);
void handle_get_multiple_open_close_sensor_command();
void handle_get_multiple_human_sensor_command();
void handle_get_multiple_vibration_sensor_command();
void handle_get_single_human_sensor_command();
void handle_get_single_vibration_sensor_command();
void handle_get_single_open_close_sensor_command();
void respond_set_sensor_command(enum result_code_t result_code);
void respond_get_sensor_command(enum result_code_t result_code, enum opening_closing_state_t state, uint16_t count);
void respond_get_sensor_event(enum result_code_t result_code, enum opening_closing_state_t state, uint16_t count);
ret_code_t send_message_with_header(enum message_id_t message_id, message_parameter_t *parameters, uint8_t parameter_count);
void respond_set_sensor(enum message_id_t message_id, enum result_code_t result_code);
void respond_get_sensor(enum message_id_t message_id, enum result_code_t result_code, enum opening_closing_state_t state, uint16_t count);
ret_code_t ble_bss_response_send(uint8_t *data, uint8_t data_length);
void ble_bss_set_state(enum opening_closing_state_t, uint16_t);


uint32_t ble_bss_init();


/**@brief Function for handling the GATT module's events.
 *
 * @details Handles all events from the GATT module of interest to the Heart Rate Service.
 *
 * @param[in]   p_hrs      Heart Rate Service structure.
 * @param[in]   p_gatt_evt  Event received from the GATT module.
 */
// void ble_bss_on_gatt_evt(nrf_ble_gatt_evt_t * p_gatt_evt);


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Heart Rate Service.
 *
 * @param[in]   p_hrs      Heart Rate Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_bss_on_ble_evt(ble_evt_t * p_ble_evt);

/**@brief Function for sending heart rate measurement if notification has been enabled.
 *
 * @details The application calls this function after having performed a heart rate measurement.
 *          If notification has been enabled, the heart rate measurement data is encoded and sent to
 *          the client.
 *
 * @param[in]   p_hrs                    Heart Rate Service structure.
 * @param[in]   heart_rate               New heart rate measurement.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
ret_code_t ble_bss_response_send(uint8_t *data, uint8_t data_length);


#ifdef __cplusplus
}
#endif

#endif // BLE_HRS_H__

/** @} */
