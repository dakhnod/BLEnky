#include <stdint.h>
#include <string.h>
#include "nrf_log.h"

#ifndef GPIOASM_BUFFER_SIZE
#define GPIOASM_BUFFER_SIZE 64
#endif

typedef void (*pin_digital_data_handler_t)(
    uint8_t *pin_digital_data, 
    uint32_t pin_digital_data_length
);

typedef void (*pin_analog_data_handler_t)(
    uint32_t pin_analog_index,
    uint16_t pin_analog_duty_cycle
);

typedef struct {
    // code storage
    uint8_t instruction_buffer[GPIOASM_BUFFER_SIZE],

    // registers
    uint32_t write_index = 0,
    uint32_t read_index = 0,
    uint8_t sequence_is_running = false,

    // transmission registers
    uint32_t packet_index = 0,

    // branch registers
    uint32_t jump_counter,
    uint32_t jump_target = 0xffffffff,
    uint32_t sleep_pin_data;

    // handlers
    pin_digital_data_handler_t sequence_pin_digital_data_handler,
    pin_analog_data_handler_t sequence_pin_analog_data_handler,
} gpioasm_engine_t;

typedef struct {
    pin_digital_data_handler_t sequence_pin_digital_data_handler,
    pin_analog_data_handler_t sequence_pin_analog_data_handler,
    pin_digital_input_provider_t
} gpioasm_engine_init_t;

typedef enum {
    PUSH_SUCCESS,
    PUSH_MISSED_PACKET,
    PUSH_OVERFLOW,
    PUSH_FIRST_PACKET,
    PUSH_FINAL_PACKET
} SEQUENCE_PACKET_PUSH_RESULT;

typedef enum {
    DECODE_SUCCESS,
    DECODE_OVERFLOW
} SEQUENCE_DECODE_RESULT;

typedef void (*sequence_progress_update_handler_t)(uint8_t sequence_is_running, uint32_t packet_index, uint32_t remaining_repetitions);

void sequence_start();
void sequence_stop(uint8_t should_notify);
void sequence_init(
    uint32_t pin_data_digital_output_length,
    uint32_t pin_data_digital_input_length,
    uint32_t pin_data_analog_length,
    pin_digital_data_handler_t pin_data_digital_handler,
    pin_analog_data_handler_t pin_data_analog_handler
);
void sequence_step();
uint8_t sequence_is_running();
uint32_t sequence_get_packet_index();
uint32_t sequence_get_repeat_count();
SEQUENCE_PACKET_PUSH_RESULT sequence_push_packet(uint8_t *data, uint32_t length);
void sequence_handle_digital_input_update(uint32_t index, bool is_high);