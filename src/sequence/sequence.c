#include "sequence.h"

#include "encoding.h"
#include "instructions.h"
#include "sensor_gpio.h"
#include "sensor_timer.h"

#define BUFFER_SIZE 200

uint8_t sequence_buffer[BUFFER_SIZE];  // 127 packets, each 19 bytes
uint32_t sequence_buffer_write_index = 0;
uint8_t sequence_current_write_seq_num = 0;

uint32_t sequence_buffer_read_index = 0;

uint32_t sequence_pin_digital_output_data_length;
uint32_t sequence_pin_digital_input_data_length;
uint32_t sequence_pin_analog_output_data_length;

pin_digital_data_handler_t sequence_pin_digital_data_handler;
pin_analog_data_handler_t sequence_pin_analog_data_handler;

uint8_t sequence_is_running_ = false;
uint32_t sequence_packet_index = 0;

uint32_t sequence_jump_counter;
uint32_t sequence_jump_instruction_index = 0xffffffff;

uint32_t sleep_pin_data;

enum {
    SLEEP_NO_CONDITION,
    SLEEP_MATCH_PINS_ALL,
    SLEEP_MATCH_PINS_ANY,
    SLEEP_MATCH_PINS_ALL_TIMEOUT,
    SLEEP_MATCH_PINS_ANY_TIMEOUT,
} sequence_sleep_condition = SLEEP_NO_CONDITION;

void sequence_stop(uint8_t should_notify) {
    timer_sequence_stop();
    sequence_is_running_ = false;
}

void sequence_reset() {
    sequence_buffer_read_index = 0;
    sequence_packet_index = 0;
}

SEQUENCE_PACKET_PUSH_RESULT sequence_push_packet(uint8_t *data, uint32_t length) {
    uint8_t sequence_number = data[0] & 0b01111111;

    SEQUENCE_PACKET_PUSH_RESULT result = PUSH_SUCCESS;

    if (sequence_number == 0) {
        sequence_stop(false);
        sequence_reset();
        sequence_current_write_seq_num = 0;
        sequence_buffer_write_index = 0;

        result = PUSH_FIRST_PACKET;
    } else if (sequence_number != (sequence_current_write_seq_num + 1)) {
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
    uint16_t value = 0;
    value |= sequence_buffer[sequence_buffer_read_index] << 0;
    value |= sequence_buffer[sequence_buffer_read_index + 1] << 8;

    sequence_buffer_read_index += 2;

    return value;

    uint16_t *ptr = (uint16_t *)(sequence_buffer + sequence_buffer_read_index);

    sequence_buffer_read_index += 2;

    return *ptr;
}

uint8_t sequence_read_instruction() {
    return sequence_buffer[sequence_buffer_read_index++];
}

void sequence_read_bytes(uint8_t **buffer, uint32_t *length) {
    uint32_t bytes_available = sequence_buffer_write_index - sequence_buffer_read_index;

    *length = MIN(bytes_available, *length);

    *buffer = sequence_buffer + sequence_buffer_read_index;
    sequence_buffer_read_index += *length;
}

uint8_t sequence_read_has_reached_end() {
    return sequence_buffer_read_index >= sequence_buffer_write_index;
}

void sequence_execute_instruction_write_digital_outputs() {
    uint32_t pin_data_length = sequence_pin_digital_output_data_length;
    uint8_t *pin_data_digital;

    NRF_LOG_DEBUG("instruction write digital\n");

    sequence_read_bytes(&pin_data_digital, &pin_data_length);

    sequence_pin_digital_data_handler(pin_data_digital, pin_data_length);
}

void sequence_execute_instruction_write_analog_output(uint32_t channel) {
    uint16_t duty_cycle = sequence_read_uint16_t();

    NRF_LOG_DEBUG("instruction write analog %i %i\n", channel, (uint32_t)duty_cycle);

    sequence_pin_analog_data_handler(channel, duty_cycle);
}

void sequence_execute_instruction_sleep_ms() {
    uint64_t delay = sequence_read_varint();
    NRF_LOG_DEBUG("instruction sleep: %i\n", delay);
    timer_sequence_start(delay);
}

// match all, none right x
// match all, one right x
// match all, all right x
// match any, none right x
// match any, one right x
// match any, all right x

bool sequence_filter_matches_digital_input_pins(uint8_t *pin_filter_data, uint32_t pin_filter_length, bool match_all) {
    bool result = false;
    for (uint32_t index = 0; index < get_pin_count_input_digital(); index++) {
        uint8_t pin_bits = encoding_get_pin_bits(pin_filter_data, pin_filter_length, index);
        if (pin_bits == 0b11) {
            NRF_LOG_DEBUG("ignoring pin %i\n", index);
            continue;
        }
        if (pin_bits == 0b10) {
            NRF_LOG_ERROR("tri-state not supported on pin %i\n", index);
            continue;
        }
        bool pin_should_be_high = (pin_bits == 0b01);
        bool pin_matches = (pin_should_be_high == gpio_get_input_digital_state(index));

        if (pin_matches) {
            result = true;
            if (!match_all) {  // singe match is enough
                return true;
            }
        } else {
            if (match_all) {  // first not match enough to break condition
                return false;
            }
        }
    }
    return result;
}

void sequence_execute_instruction_sleep_match(bool match_all, bool *should_run_next) {
    uint32_t pin_data_length = sequence_pin_digital_input_data_length;
    uint8_t *pin_data_digital;

    sleep_pin_data = sequence_buffer_read_index;

    sequence_read_bytes(&pin_data_digital, &pin_data_length);

    bool match_condition_fulfilled = sequence_filter_matches_digital_input_pins(pin_data_digital, pin_data_length, match_all);

    NRF_LOG_DEBUG("init condition %i\n", match_condition_fulfilled);

    // we are not continuing the execution when pins don't match
    // we wait for a gpio notification to continue
    *should_run_next = match_condition_fulfilled;

    if (!match_condition_fulfilled) {
        if (match_all) {
            sequence_sleep_condition = SLEEP_MATCH_PINS_ALL;
        } else {
            sequence_sleep_condition = SLEEP_MATCH_PINS_ANY;
        }
    }
}

void sequence_execute_instruction_sleep_match_timeout(bool match_all, bool *should_run_next) {
    bool match_condition_fulfilled;

    sequence_execute_instruction_sleep_match(match_all, &match_condition_fulfilled);

    *should_run_next = match_condition_fulfilled;

    uint64_t timeout = sequence_read_varint();

    if (match_condition_fulfilled) {
        return;
    }

    bool already_waiting = (sequence_sleep_condition == SLEEP_MATCH_PINS_ALL_TIMEOUT) || (sequence_sleep_condition == SLEEP_MATCH_PINS_ANY_TIMEOUT);

    if (already_waiting) {
        return;
    }

    if (match_all) {
        sequence_sleep_condition = SLEEP_MATCH_PINS_ALL_TIMEOUT;
    } else {
        sequence_sleep_condition = SLEEP_MATCH_PINS_ANY_TIMEOUT;
    }

    NRF_LOG_DEBUG("instruction sleep timeout: %i\n", timeout);
    timer_sequence_start(timeout);
}

void sequence_handle_digital_input_update(uint32_t index, bool is_high) {
    if (sequence_sleep_condition == SLEEP_NO_CONDITION) {
        return;
    }
    // rewind read head to previous sleep command
    // rewind by (pin data length)
    sequence_buffer_read_index = sleep_pin_data;

    bool execute_next_instruction;

    bool match_all = (sequence_sleep_condition == SLEEP_MATCH_PINS_ALL) || (sequence_sleep_condition == SLEEP_MATCH_PINS_ALL_TIMEOUT);
    bool has_timeout = (sequence_sleep_condition == SLEEP_MATCH_PINS_ALL_TIMEOUT) || (sequence_sleep_condition == SLEEP_MATCH_PINS_ANY_TIMEOUT);

    if (has_timeout) {
        sequence_execute_instruction_sleep_match_timeout(match_all, &execute_next_instruction);
    } else {
        sequence_execute_instruction_sleep_match(match_all, &execute_next_instruction);
    }

    if (execute_next_instruction) {
        if (sequence_sleep_condition == SLEEP_MATCH_PINS_ALL_TIMEOUT || sequence_sleep_condition == SLEEP_MATCH_PINS_ANY_TIMEOUT) {
            timer_sequence_stop();
        }

        sequence_sleep_condition = SLEEP_NO_CONDITION;
        sequence_step();
    }
}

void sequence_execute_instruction_jump_unconditionally() {
    uint32_t target = sequence_read_varint();
    target = MIN(target, sequence_buffer_write_index);

    NRF_LOG_DEBUG("instruction jump to %i\n", target);

    // disable jump counter
    sequence_jump_instruction_index = 0xffffffff;

    sequence_buffer_read_index = target;
}

void sequence_execute_instruction_check_bytecode_version(bool *should_run_next) {
    uint64_t bytecode_version = sequence_read_varint();
    *should_run_next = (bytecode_version == 0);
}

void sequence_execute_instruction_jump_match(bool match_all) {
    uint64_t target = sequence_read_varint();

    uint32_t pin_data_length = sequence_pin_digital_input_data_length;
    uint8_t *pin_data_digital;

    sequence_read_bytes(&pin_data_digital, &pin_data_length);

    bool match_condition_fulfilled = sequence_filter_matches_digital_input_pins(pin_data_digital, pin_data_length, match_all);

    NRF_LOG_DEBUG("jump match condition fulfilled: %i\n", match_condition_fulfilled);

    if (match_condition_fulfilled) {
        // disable jump counter
        sequence_jump_instruction_index = 0xffffffff;

        sequence_buffer_read_index = target;
    }
}

void sequence_execute_instruction_jump_n_times() {
    uint32_t jump_instruction_index = sequence_buffer_read_index - 1;
    uint32_t jump_target = sequence_read_varint();
    uint32_t jump_count = sequence_read_varint();

    if (jump_instruction_index != sequence_jump_instruction_index) {
        // firstly encountering this jump instruction
        sequence_jump_instruction_index = jump_instruction_index;
        // set counter to desired jump count
        sequence_jump_counter = jump_count;
    } else {
        sequence_jump_counter--;
    }

    if (sequence_jump_counter == 0) {
        // jump counter at 0, not jumping any more
        // disable jump counter
        sequence_jump_instruction_index = 0xffffffff;
        return;
    }

    // jump to desired address
    sequence_buffer_read_index = jump_target;
}

uint8_t sequence_instruction_filter_bits(uint8_t instruction) {
    uint8_t instruction_without_last_bits = instruction & 0b11110000;
    switch (instruction_without_last_bits) {
        case INSTRUCTION_WRITE_OUTPUT_DIGITAL_PINS:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL_TIMEOUT:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY_TIMEOUT:
        case INSTRUCTION_JUMP_MATCH_PINS_ALL:
        case INSTRUCTION_JUMP_MATCH_PINS_ANY:
            return instruction_without_last_bits;
    }

    return instruction;
}

void sequence_execute_instruction(uint8_t instruction, bool *should_run_next) {
    *should_run_next = true;

    uint8_t instruction_filtered = sequence_instruction_filter_bits(instruction);

    switch (instruction_filtered) {
        case INSTRUCTION_WRITE_OUTPUT_DIGITAL_PINS:
            sequence_pin_digital_output_data_length = (instruction & 0b00001111);
            break;
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL_TIMEOUT:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY_TIMEOUT:
        case INSTRUCTION_JUMP_MATCH_PINS_ALL:
        case INSTRUCTION_JUMP_MATCH_PINS_ANY:
            sequence_pin_digital_input_data_length = (instruction & 0b00001111);
            break;
    }

    switch (instruction_filtered) {
        case INSTRUCTION_WRITE_OUTPUT_DIGITAL_PINS:
            sequence_execute_instruction_write_digital_outputs();
            break;
        case INSTRUCTION_SLEEP_MS:
            sequence_execute_instruction_sleep_ms();
            *should_run_next = false;
            break;
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY:
            sequence_execute_instruction_sleep_match(instruction_filtered == INSTRUCTION_SLEEP_MATCH_INPUTS_ALL, should_run_next);
            break;
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL_TIMEOUT:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY_TIMEOUT:
            sequence_execute_instruction_sleep_match_timeout(instruction_filtered == INSTRUCTION_SLEEP_MATCH_INPUTS_ALL_TIMEOUT, should_run_next);
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
        case INSTRUCTION_CHECK_BYTECODE_VERSION:
            sequence_execute_instruction_check_bytecode_version(should_run_next);
            break;
        case INSTRUCTION_JUMP_MATCH_PINS_ALL:
        case INSTRUCTION_JUMP_MATCH_PINS_ANY:
            sequence_execute_instruction_jump_match(instruction_filtered == INSTRUCTION_JUMP_MATCH_PINS_ALL);
            break;
        case INSTRCUTION_JUMP_N_TIMES:
            sequence_execute_instruction_jump_n_times();
            break;
        case INSTRUCTION_STOP_EXECUTION:
            *should_run_next = false;
            break;
        default:
            NRF_LOG_DEBUG("instruction %i unknown\n", instruction);
            *should_run_next = false;
            break;
    }
}

void sequence_buffer_next_packet() {
    bool should_run_next;
    do {
        uint8_t instruction = sequence_read_instruction();
        sequence_execute_instruction(instruction, &should_run_next);
    } while (should_run_next && !sequence_read_has_reached_end());
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

void sequence_start() {
    NRF_LOG_DEBUG("starting seuqnece\n");

    sequence_reset();

    sequence_is_running_ = true;

    sequence_step();
}

void sequence_init(uint32_t pin_data_digital_output_length, uint32_t pin_data_digital_input_length, uint32_t pin_data_analog_length, pin_digital_data_handler_t pin_digital_data_handler, pin_analog_data_handler_t pin_analog_data_handler) {
    sequence_pin_digital_output_data_length = pin_data_digital_output_length;
    sequence_pin_digital_input_data_length = pin_data_digital_input_length;
    sequence_pin_analog_output_data_length = pin_data_analog_length;
    timer_sequence_set_timeout_handler(sequence_timer_timeout_handler);
    sequence_pin_digital_data_handler = pin_digital_data_handler;
    sequence_pin_analog_data_handler = pin_analog_data_handler;
}