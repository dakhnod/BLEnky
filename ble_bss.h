#ifndef BLE_HRS_H__
#define BLE_HRS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_ble_gatt.h"

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

enum openinig_closing_state {
    CLOSED,
    OPEN
};

enum report_state_t {
    OFF,
    ON
};

typedef struct {
    char parameter_id;
    char parameter_length;
    short reserved;
    char *data;
} message_parameter_t;

typedef struct {
    char parameter_id;
    char parameter_length;
} message_parameter_without_data_t;

typedef struct {
    char rfu_1;
    char message_id;
    char rfu_2;
    char parameter_count;
} message_header_t;



void parse_full_packet_with_split_header(char *data, short length);
void parse_packet_decoded(enum message_id_t message_id, message_parameter_t *parameters, char parameter_count);
void parse_packet(char *data, short length);
void parse_set_sensor_command(message_parameter_t *parameters, char parameter_count);
void handle_set_sensor_command(enum sensor_type_t sensor_type, enum report_state_t report_state);
void parse_get_sensor_command(message_parameter_t *parameters, char parameter_count);
void handle_get_sensor_command(enum sensor_type_t sensor_type);
void handle_get_multiple_open_close_sensor_command();
void handle_get_multiple_human_sensor_command();
void handle_get_multiple_vibration_sensor_command();
void handle_get_single_human_sensor_command();
void handle_get_single_vibration_sensor_command();
void handle_get_single_open_close_sensor_command();
void respond_set_sensor_command(enum result_code_t result_code);
void respond_get_sensor_command(enum result_code_t result_code, enum openinig_closing_state state, short count);
void respond_get_sensor_event(enum result_code_t result_code, enum openinig_closing_state state, short count);
short send_message_with_header(enum message_id_t message_id, message_parameter_t *parameters, char parameter_count);
void respond_set_sensor(enum message_id_t message_id, enum result_code_t result_code);
void respond_get_sensor(enum message_id_t message_id, enum result_code_t result_code, enum openinig_closing_state state, short count);
int ble_bss_response_send(char *data, int data_length);


uint32_t ble_bss_init();


/**@brief Function for handling the GATT module's events.
 *
 * @details Handles all events from the GATT module of interest to the Heart Rate Service.
 *
 * @param[in]   p_hrs      Heart Rate Service structure.
 * @param[in]   p_gatt_evt  Event received from the GATT module.
 */
void ble_bss_on_gatt_evt(nrf_ble_gatt_evt_t * p_gatt_evt);


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
uint32_t ble_bss_response_send(uint16_t heart_rate);


#ifdef __cplusplus
}
#endif

#endif // BLE_HRS_H__

/** @} */
