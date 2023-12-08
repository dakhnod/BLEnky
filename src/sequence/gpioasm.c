#include "gpioasm.h"
#include "instructions.h"

#ifdef GPIOASM_LOG_IMPORT
#include GPIOASM_LOG_IMPORT
#endif

void gpioasm_stop(gpioasm_engine_t *engine) {
    if(engine->init.timer_handler == NULL){
        GPIOASM_LOG_ERROR("timer_handler not set in init\n");
        return;
    }
    engine->init.timer_handler(0, false);
    engine->is_running = false;
}

void gpioasm_reset(gpioasm_engine_t *engine) {
    engine->instruction_index = 0;
    engine->packet_index = 0;
}

GPIOASM_PACKET_PUSH_RESULT gpioasm_push_packet(gpioasm_engine_t *engine, uint8_t *data, uint32_t length) {
    uint8_t sequence_number = data[0] & 0b01111111;

    GPIOASM_PACKET_PUSH_RESULT result = PUSH_SUCCESS;

    if (sequence_number == 0) {
        gpioasm_stop(engine);
        gpioasm_reset(engine);
        engine->write_index = 0;
        engine->packet_index = 0;

        result = PUSH_FIRST_PACKET;

        GPIOASM_LOG_DEBUG("received final packet\n");
    } else if (sequence_number != (engine->packet_index + 1)) {
        return PUSH_MISSED_PACKET;
    }
    engine->packet_index = sequence_number;

    uint32_t payload_length = length - 1;
    uint8_t *payload = data + 1;

    if ((engine->write_index + payload_length) > GPIOASM_BUFFER_SIZE) {
        return PUSH_OVERFLOW;
    }

    memcpy(engine->instruction_buffer + engine->write_index, payload, payload_length);

    engine->write_index += payload_length;

    if ((data[0] & 0b10000000) == 0b00000000) {
        return PUSH_FINAL_PACKET;
    }

    return result;
}

uint8_t gpioasm_is_running(gpioasm_engine_t *engine) {
    return engine->is_running;
}

uint64_t gpioasm_read_varint(gpioasm_engine_t *engine) {
    uint64_t current = 0;
    for (uint32_t i = 0; i < 8; i++) {
        uint8_t byte = engine->instruction_buffer[engine->instruction_index];
        engine->instruction_index++;
        current |= ((byte & 0b01111111) << (i * 7));

        if (!(byte & 0b10000000)) {
            return current;
        }
    }
    return 0;
}

uint16_t gpioasm_read_uint16(gpioasm_engine_t *engine) {
    uint16_t value = 0;

    memcpy(&value, engine->instruction_buffer + engine->instruction_index, 2);

    engine->instruction_index += 2;

    return value;
}

uint8_t gpioasm_read_instruction(gpioasm_engine_t *engine) {
    return engine->instruction_buffer[engine->instruction_index++];
}

void gpioasm_read_bytes(gpioasm_engine_t *engine, uint8_t **buffer, uint32_t *length) {
    uint32_t bytes_available = engine->write_index - engine->instruction_index;

    *length = MIN(bytes_available, *length);

    *buffer = engine->instruction_buffer + engine->instruction_index;
    engine->instruction_index += *length;
}

bool gpioasm_read_has_reached_end(gpioasm_engine_t *engine) {
    return engine->instruction_index >= engine->write_index;
}

uint8_t gpioasm_decode_pin_bits(uint8_t *pin_data, uint32_t pin_data_length, uint32_t pin_index){
        uint32_t bit_index_full = pin_index * 2;
        uint32_t byte_index = bit_index_full / 8;
        uint8_t bit_index = bit_index_full % 8;
        uint8_t current_byte = pin_data[pin_data_length - byte_index - 1];
        uint8_t output_bits = (current_byte >> bit_index) & 0b00000011;
        return output_bits;
}

uint8_t gpioasm_get_argument_count(gpioasm_engine_t *engine){
    uint8_t instruction = engine->instruction_buffer[engine->instruction_index - 1];
    return instruction & 0b00001111;
}

void gpioasm_execute_instruction_write_digital_outputs(gpioasm_engine_t *engine) {
    GPIOASM_LOG_DEBUG("instruction write digital\n");

    uint32_t pin_data_length = gpioasm_get_argument_count(engine);
    uint8_t *pin_data_digital;

    gpioasm_read_bytes(engine, &pin_data_digital, &pin_data_length);

    if(engine->init.pin_digital_output_handler == NULL){
        GPIOASM_LOG_ERROR("pin_digital_output_handler not set in init\n");
        return;
    }

    for(uint32_t index = 0; index < (pin_data_length * 4); index++){
        uint8_t pin_bits = gpioasm_decode_pin_bits(pin_data_digital, pin_data_length, index);
        if(pin_bits == 0b11){
            continue;
        }
        engine->init.pin_digital_output_handler(index, pin_bits);
    }
}

void gpioasm_execute_instruction_write_analog_output(gpioasm_engine_t *engine) {
    uint32_t channel = gpioasm_get_argument_count(engine);
    uint16_t duty_cycle = gpioasm_read_uint16(engine);

    GPIOASM_LOG_DEBUG("instruction write analog %i %i\n", channel, (uint32_t)duty_cycle);

    if(engine->init.pin_analog_output_handler == NULL){
        GPIOASM_LOG_ERROR("pin_analog_output_handler not set in init\n");
        return;
    }
    engine->init.pin_analog_output_handler(channel, duty_cycle);
}

void gpioasm_execute_instruction_sleep_ms(gpioasm_engine_t *engine) {
    uint64_t delay = gpioasm_read_varint(engine);
    GPIOASM_LOG_DEBUG("instruction sleep: %i\n", delay);
    
    engine->sleep_condition = SLEEP_NO_CONDITION;

    if(engine->init.timer_handler == NULL){
        GPIOASM_LOG_ERROR("timer_handler not set in init\n");
        return;
    }
    engine->init.timer_handler(delay, true);
}

bool gpioasm_filter_matches_digital_input_pins(gpioasm_engine_t *engine, uint8_t *pin_filter_data, uint32_t pin_filter_length, bool match_all) {
    if(engine->init.pin_digital_input_provider == NULL){
        GPIOASM_LOG_ERROR("pin_digital_input_provider not set in init\n");
        return false;
    }
    
    bool result = false;
    uint32_t pin_count = pin_filter_length * 4;
    for (uint32_t index = 0; index < pin_count; index++) {
        uint8_t pin_bits = gpioasm_decode_pin_bits(pin_filter_data, pin_filter_length, index);
        if (pin_bits == 0b11) {
            GPIOASM_LOG_DEBUG("ignoring pin %i\n", index);
            continue;
        }
        if (pin_bits == 0b10) {
            GPIOASM_LOG_ERROR("tri-state not supported on pin %i\n", index);
            continue;
        }
        bool pin_should_be_high = (pin_bits == 0b01);
        bool pin_matches = (pin_should_be_high == engine->init.pin_digital_input_provider(index));

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

void gpioasm_execute_instruction_sleep_match(gpioasm_engine_t *engine, bool match_all, bool *should_run_next) {
    uint32_t pin_data_length = gpioasm_get_argument_count(engine);
    uint8_t *pin_data_digital;

    engine->sleep_pin_data = engine->instruction_index;

    gpioasm_read_bytes(engine, &pin_data_digital, &pin_data_length);

    bool match_condition_fulfilled = gpioasm_filter_matches_digital_input_pins(engine, pin_data_digital, pin_data_length, match_all);

    GPIOASM_LOG_DEBUG("init condition %i\n", match_condition_fulfilled);

    // we are not continuing the execution when pins don't match
    // we wait for a gpio notification to continue
    *should_run_next = match_condition_fulfilled;

    if (!match_condition_fulfilled) {
        if (match_all) {
            engine->sleep_condition = SLEEP_MATCH_PINS_ALL;
        } else {
            engine->sleep_condition = SLEEP_MATCH_PINS_ANY;
        }
    }
}

void gpioasm_execute_instruction_sleep_match_timeout(gpioasm_engine_t *engine, bool match_all, bool *should_run_next) {
    bool match_condition_fulfilled;

    gpioasm_execute_instruction_sleep_match(engine, match_all, &match_condition_fulfilled);

    *should_run_next = match_condition_fulfilled;

    uint64_t timeout = gpioasm_read_varint(engine);

    if(engine->init.timer_handler == NULL){
        GPIOASM_LOG_ERROR("timer_handler not set in init\n");
        return;
    }

    if (match_condition_fulfilled) {
        return;
    }

    bool already_waiting = (engine->sleep_condition == SLEEP_MATCH_PINS_ALL_TIMEOUT) || (engine->sleep_condition == SLEEP_MATCH_PINS_ANY_TIMEOUT);

    if (already_waiting) {
        return;
    }

    if (match_all) {
        engine->sleep_condition = SLEEP_MATCH_PINS_ALL_TIMEOUT;
    } else {
        engine->sleep_condition = SLEEP_MATCH_PINS_ANY_TIMEOUT;
    }

    GPIOASM_LOG_DEBUG("instruction sleep timeout: %i\n", timeout);
    engine->init.timer_handler(timeout, true);
}

void gpioasm_handle_digital_input_update(gpioasm_engine_t *engine, uint32_t index, bool is_high) {
    if(engine->init.timer_handler == NULL){
        GPIOASM_LOG_ERROR("timer_handler not set in init\n");
        return;
    }

    if (engine->sleep_condition == SLEEP_NO_CONDITION) {
        return;
    }
    // rewind read head to previous sleep command
    // rewind by (pin data length)
    engine->instruction_index = engine->sleep_pin_data;

    bool execute_next_instruction;

    bool match_all = (engine->sleep_condition == SLEEP_MATCH_PINS_ALL) || (engine->sleep_condition == SLEEP_MATCH_PINS_ALL_TIMEOUT);
    bool has_timeout = (engine->sleep_condition == SLEEP_MATCH_PINS_ALL_TIMEOUT) || (engine->sleep_condition == SLEEP_MATCH_PINS_ANY_TIMEOUT);

    if (has_timeout) {
        gpioasm_execute_instruction_sleep_match_timeout(engine, match_all, &execute_next_instruction);
    } else {
        gpioasm_execute_instruction_sleep_match(engine, match_all, &execute_next_instruction);
    }

    if (execute_next_instruction) {
        if (engine->sleep_condition == SLEEP_MATCH_PINS_ALL_TIMEOUT || engine->sleep_condition == SLEEP_MATCH_PINS_ANY_TIMEOUT) {
            engine->init.timer_handler(0, false);
        }

        engine->sleep_condition = SLEEP_NO_CONDITION;
        gpioasm_step(engine);
    }
}

void gpioasm_execute_instruction_jump_unconditionally(gpioasm_engine_t *engine) {
    uint32_t target = gpioasm_read_varint(engine);
    target = MIN(target, engine->write_index);

    GPIOASM_LOG_DEBUG("instruction jump to %i\n", target);

    // disable jump counter
    engine->jump_target = 0xffffffff;

    engine->instruction_index = target;
}

void gpioasm_execute_instruction_check_bytecode_version(gpioasm_engine_t *engine, bool *should_run_next) {
    uint64_t bytecode_version = gpioasm_read_varint(engine);
    *should_run_next = (bytecode_version == 0);

    GPIOASM_LOG_DEBUG("bytecode matches: %d\n", *should_run_next);
}

void gpioasm_execute_instruction_jump_match(gpioasm_engine_t *engine, bool match_all) {
    uint64_t target = gpioasm_read_varint(engine);

    uint32_t pin_data_length = gpioasm_get_argument_count(engine);
    uint8_t *pin_data_digital;

    gpioasm_read_bytes(engine, &pin_data_digital, &pin_data_length);

    bool match_condition_fulfilled = gpioasm_filter_matches_digital_input_pins(engine, pin_data_digital, pin_data_length, match_all);

    GPIOASM_LOG_DEBUG("jump match condition fulfilled: %i\n", match_condition_fulfilled);

    if (match_condition_fulfilled) {
        // disable jump counter
        engine->jump_target = 0xffffffff;

        engine->instruction_index = target;
    }
}

void gpioasm_execute_instruction_jump_n_times(gpioasm_engine_t *engine) {
    uint32_t jump_instruction_index = engine->instruction_index - 1;
    uint32_t jump_target = gpioasm_read_varint(engine);
    uint32_t jump_count = gpioasm_read_varint(engine);

    if (jump_instruction_index != engine->jump_target) {
        // firstly encountering this jump instruction
        engine->jump_target = jump_instruction_index;
        // set counter to desired jump count
        engine->jump_counter = jump_count;
    } else {
        engine->jump_counter--;
    }

    if (engine->jump_counter == 0) {
        // jump counter at 0, not jumping any more
        // disable jump counter
        engine->jump_target = 0xffffffff;
        return;
    }

    // jump to desired address
    engine->instruction_index = jump_target;
}

uint8_t gpioasm_instruction_filter_bits(uint8_t instruction) {
    uint8_t instruction_without_last_bits = instruction & 0b11110000;
    switch (instruction_without_last_bits) {
        case INSTRUCTION_WRITE_OUTPUT_DIGITAL_PINS:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL_TIMEOUT:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY_TIMEOUT:
        case INSTRUCTION_JUMP_MATCH_PINS_ALL:
        case INSTRUCTION_JUMP_MATCH_PINS_ANY:
        case INSTRUCTION_WRITE_OUTPUT_ANALOG_CHANNEL:
            return instruction_without_last_bits;
    }

    return instruction;
}

void gpioasm_execute_instruction(gpioasm_engine_t *engine, uint8_t instruction, bool *should_run_next) {
    *should_run_next = true;

    uint8_t instruction_filtered = gpioasm_instruction_filter_bits(instruction);

    switch (instruction_filtered) {
        case INSTRUCTION_WRITE_OUTPUT_DIGITAL_PINS:
            gpioasm_execute_instruction_write_digital_outputs(engine);
            break;
        case INSTRUCTION_SLEEP_MS:
            gpioasm_execute_instruction_sleep_ms(engine);
            *should_run_next = false;
            break;
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY:
            gpioasm_execute_instruction_sleep_match(engine, instruction_filtered == INSTRUCTION_SLEEP_MATCH_INPUTS_ALL, should_run_next);
            break;
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ALL_TIMEOUT:
        case INSTRUCTION_SLEEP_MATCH_INPUTS_ANY_TIMEOUT:
            gpioasm_execute_instruction_sleep_match_timeout(engine, instruction_filtered == INSTRUCTION_SLEEP_MATCH_INPUTS_ALL_TIMEOUT, should_run_next);
            break;
        case INSTRUCTION_WRITE_OUTPUT_ANALOG_CHANNEL:
            gpioasm_execute_instruction_write_analog_output(engine);
            break;
        case INSTRUCTION_JUMP_UNCONDITIONALLY:
            gpioasm_execute_instruction_jump_unconditionally(engine);
            break;
        case INSTRUCTION_CHECK_BYTECODE_VERSION:
            gpioasm_execute_instruction_check_bytecode_version(engine, should_run_next);
            break;
        case INSTRUCTION_JUMP_MATCH_PINS_ALL:
        case INSTRUCTION_JUMP_MATCH_PINS_ANY:
            gpioasm_execute_instruction_jump_match(engine, instruction_filtered == INSTRUCTION_JUMP_MATCH_PINS_ALL);
            break;
        case INSTRCUTION_JUMP_N_TIMES:
            gpioasm_execute_instruction_jump_n_times(engine);
            break;
        case INSTRUCTION_STOP_EXECUTION:
            *should_run_next = false;
            break;
        default:
            GPIOASM_LOG_DEBUG("instruction %i unknown\n", instruction);
            *should_run_next = false;
            break;
    }
}

void gpioasm_buffer_next_packet(gpioasm_engine_t *engine) {
    bool should_run_next;
    do {
        uint8_t instruction = gpioasm_read_instruction(engine);
        gpioasm_execute_instruction(engine, instruction, &should_run_next);

        if(gpioasm_read_has_reached_end(engine)){
            GPIOASM_LOG_DEBUG("instructions end.\n");
            break;
        }
    } while (should_run_next);
}

void gpioasm_step(gpioasm_engine_t *engine) {
    gpioasm_buffer_next_packet(engine);
}

void gpioasm_handle_timer_timeout(gpioasm_engine_t *engine) {
    if (gpioasm_read_has_reached_end(engine)) {
        return;
    }
    gpioasm_step(engine);
}

void gpioasm_start(gpioasm_engine_t *engine) {
    GPIOASM_LOG_DEBUG("starting sequence\n");

    gpioasm_reset(engine);

    engine->is_running = true;

    gpioasm_step(engine);
}

void gpioasm_init(gpioasm_engine_t *engine, gpioasm_engine_init_t *gpioasm_engine_init) {
    memset(engine, 0, sizeof(gpioasm_engine_t));

    engine->jump_target = 0xffffffff;

    memcpy(&engine->init, gpioasm_engine_init, sizeof(gpioasm_engine_init_t));
}