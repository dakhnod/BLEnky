#include "sequence.h"
#include "sensor_timer.h"
#include "instructions.h"

#define BUFFER_SIZE 50

uint8_t sequence_buffer[BUFFER_SIZE]; // 127 packets, each 19 bytes
uint32_t sequence_buffer_write_index = 0;
uint8_t sequence_current_write_seq_num = 0;

uint32_t sequence_buffer_read_index = 0;

uint32_t sequence_pin_digital_data_length;
uint32_t sequence_pin_analog_data_length;

pin_digital_data_handler_t sequence_pin_digital_data_handler;
pin_analog_data_handler_t sequence_pin_analog_data_handler;
sequence_progress_update_handler_t sequence_progress_update_handler;

uint8_t sequence_is_running_ = false;
uint32_t sequence_packet_index = 0;

void handle_sequence_update() {
    sequence_progress_update_handler(sequence_is_running_, sequence_packet_index, 0);
}

void sequence_stop(uint8_t should_notify) {
    timer_sequence_stop();
    sequence_is_running_ = false;
    if (should_notify) {
        handle_sequence_update();
    }
}

void sequence_reset() {
    sequence_buffer_read_index = 0;
    sequence_packet_index = 0;
}

SEQUENCE_PACKET_PUSH_RESULT sequence_push_packet(uint8_t *data, uint32_t length) {
    uint8_t sequence_number = data[0] & 0b01111111;

    NRF_LOG_DEBUG("packet seq %d\n", sequence_number);

    SEQUENCE_PACKET_PUSH_RESULT result = PUSH_SUCCESS;

    if (sequence_number == 0) {
        sequence_stop(false);
        sequence_reset();
        sequence_current_write_seq_num = 0;
        sequence_buffer_write_index = 0;

        result = PUSH_FIRST_PACKET;
    }
    else if (sequence_number != (sequence_current_write_seq_num + 1)) {
        return PUSH_MISSED_PACKET;
    }
    sequence_current_write_seq_num = sequence_number;

    uint32_t payload_length = length - 1;
    uint8_t *payload = data + 1;

    if ((sequence_buffer_write_index + payload_length) > BUFFER_SIZE) {
        return PUSH_OVERFLOW;
    }

    memcpy(sequence_buffer + sequence_buffer_write_index, payload, payload_length);

    sequence_buffer_write_index += payload_length;

    if ((data[0] & 0b10000000) == 0b00000000) {
        for (int i = 0; i < sequence_buffer_write_index; i++) {
        }
        return PUSH_FINAL_PACKET;
    }

    return result;
}

uint8_t sequence_is_running() {
    return sequence_is_running_;
}

uint32_t sequence_get_packet_index() {
    return sequence_packet_index;
}

uint64_t sequence_read_varint() {
    uint64_t current = 0;
    for (uint32_t i = 0; i < 8; i++) {
        uint8_t byte = sequence_buffer[sequence_buffer_read_index];
        sequence_buffer_read_index++;
        current |= ((byte & 0b01111111) << (i * 7));

        if (!(byte & 0b10000000)) {
            return current;
        }
    }
    return 0;
}

uint16_t sequence_read_uint16_t() {
    uint16_t result = *(uint16_t*)(sequence_buffer + sequence_buffer_read_index);

    sequence_buffer_read_index += 2;

    return result;
}

uint8_t sequence_read_instruction(){
    return sequence_buffer[sequence_buffer_read_index++];
}

void sequence_read_bytes(uint8_t **buffer, uint32_t *length) {
    uint32_t bytes_available = sequence_buffer_write_index - sequence_buffer_read_index;

    *length = MIN(bytes_available, *length);

    *buffer = sequence_buffer + sequence_buffer_read_index;
    sequence_buffer_read_index += bytes_available;
}

uint8_t sequence_read_has_reached_end() {
    return sequence_buffer_read_index >= sequence_buffer_write_index;
}

void sequence_execute_instruction_write_digital_outputs(){
    uint32_t pin_data_length = sequence_pin_digital_data_length;
    uint8_t *pin_data_digital;

    NRF_LOG_DEBUG("instruction write digital\n");

    sequence_read_bytes(&pin_data_digital, &pin_data_length);

    sequence_pin_digital_data_handler(pin_data_digital, pin_data_length);
}

void sequence_execute_instruction_write_analog_output(uint32_t channel){
    uint16_t duty_cycle = sequence_read_uint16_t();

    NRF_LOG_DEBUG("instruction analog channel %i %i\n", channel, duty_cycle);

    sequence_pin_analog_data_handler(channel, duty_cycle);
}

void sequence_execute_instruction_sleep_ms(){
    uint64_t delay = sequence_read_varint();
    NRF_LOG_DEBUG("instruction sleep: %i\n", delay);
    timer_sequence_start(delay);
}

void sequence_execute_instruction_jump_unconditionally(){
    uint16_t target = sequence_read_uint16_t();
    target = MIN(target, sequence_buffer_write_index);

    NRF_LOG_DEBUG("instruction jump to %i\n", target);

    sequence_buffer_read_index = target;
}

void sequence_execute_instruction(uint8_t instruction, bool *should_run_next) {
    *should_run_next = true;
    switch(instruction){
        case INSTRUCTION_WRITE_OUTPUT_DIGITAL_PINS:
            sequence_execute_instruction_write_digital_outputs();
            break;
        case INSTRUCTION_SLEEP_MS:
            sequence_execute_instruction_sleep_ms();
            *should_run_next = false;
            break;
        case INSTRUCTION_SLEEP_MATCH_INPUTS:
            // handler function needs to decide whether we continue with next instruction
            *should_run_next = false;
            break;
        case INSTRUCTION_WRITE_OUTPUT_ANALOG_PIN_0:
        case INSTRUCTION_WRITE_OUTPUT_ANALOG_PIN_1:
        case INSTRUCTION_WRITE_OUTPUT_ANALOG_PIN_2:
        case INSTRUCTION_WRITE_OUTPUT_ANALOG_PIN_3:
            sequence_execute_instruction_write_analog_output(instruction & 0b00001111);
            break;
        case INSTRUCTION_JUMP_UNCONDITIONALLY:
            sequence_execute_instruction_jump_unconditionally();
            break;
        case INSTRUCTION_JUMP_MATCH_PINS:
            break;
        case INSTRCUTION_JUMP_N_TIMES:
            break;
        case INSTRUCTION_STOP_EXECUTION:
            NRF_LOG_DEBUG("stopping execution\n");
            *should_run_next = false;
            break;
        default:
            NRF_LOG_DEBUG("instruction %x unknown\n", instruction);
            break;
    }
}

void sequence_buffer_next_packet() {
    bool should_run_next;
    do{
        uint8_t instruction = sequence_read_instruction();
        sequence_execute_instruction(instruction, &should_run_next);
    }while(should_run_next);
}

void sequence_step() {
    sequence_buffer_next_packet();
}

void sequence_timer_timeout_handler() {
    if (sequence_read_has_reached_end()) {
        return;
    }
    sequence_step();
}

void sequence_start(uint8_t contains_analog) {
    NRF_LOG_DEBUG("starting seuqnece (contains analog: %i)\n", contains_analog);

    sequence_reset();

    sequence_is_running_ = true;

    sequence_step();
}

void sequence_init(
    uint32_t pin_data_digital_length,
    uint32_t pin_data_analog_length,
    pin_digital_data_handler_t pin_digital_data_handler,
    pin_analog_data_handler_t pin_analog_data_handler,
    sequence_progress_update_handler_t progress_update_handler
) {
    sequence_pin_digital_data_length = pin_data_digital_length;
    sequence_pin_analog_data_length = pin_data_analog_length;
    timer_sequence_set_timeout_handler(sequence_timer_timeout_handler);
    sequence_pin_digital_data_handler = pin_digital_data_handler;
    sequence_pin_analog_data_handler = pin_analog_data_handler;
    sequence_progress_update_handler = progress_update_handler;
}