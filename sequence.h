#include <stdint.h>
#include <string.h>
#include "nrf_log.h"

typedef enum {
    PUSH_SUCCESS,
    PUSH_MISSED_PACKET,
    PUSH_OVERFLOW,
    PUSH_FINAL_PACKET
} SEQUENCE_PACKET_PUSH_RESULT;

typedef enum {
    DECODE_SUCCESS,
    DECODE_OVERFLOW
} SEQUENCE_DECODE_RESULT;

typedef void (*pin_data_handler_t)(uint8_t *pin_data, uint32_t pin_data_length);
typedef void (*sequence_progress_update_handler_t)(uint8_t sequence_is_running, uint32_t packet_index, uint32_t remaining_repetitions);

void sequence_start();
void sequence_stop(uint8_t should_notify);
void sequence_init(
    uint32_t pin_data_length,
    pin_data_handler_t pin_data_handler,
    sequence_progress_update_handler_t progress_update_handler
);
uint8_t sequence_is_running();
uint32_t sequence_get_packet_index();
uint32_t sequence_get_repeat_count();
SEQUENCE_PACKET_PUSH_RESULT sequence_push_packet(uint8_t *data, uint32_t length);