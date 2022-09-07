#include "sequence.h"
#include "sensor_timer.h"

#define BUFFER_SIZE 128

uint8_t sequence_buffer[BUFFER_SIZE]; // 127 packets, each 19 bytes
uint32_t sequence_buffer_write_index = 0;
uint8_t sequence_current_write_seq_num = 0;

uint32_t sequence_buffer_read_index = 0;
uint32_t sequence_buffer_start_index;

uint8_t repeat_indefinetly;
uint64_t sequence_repeat_count;

uint32_t sequence_pin_digital_data_length;
uint32_t sequence_pin_analog_data_length;
uint8_t sequence_contains_analog = false;

pin_data_handler_t sequence_pin_data_handler;
sequence_progress_update_handler_t sequence_progress_update_handler;

uint8_t sequence_is_running_ = false;
uint32_t sequence_packet_index = 0;

typedef struct {
    uint8_t *pin_digital_data;
    uint32_t pin_digital_data_length;

    uint16_t *pin_analog_data;
    uint32_t pin_analog_data_length;

    uint64_t delay;
} sequence_packet_t;

void handle_sequence_update() {
    sequence_progress_update_handler(sequence_is_running_, sequence_packet_index, sequence_repeat_count);
}

void sequence_stop(uint8_t should_notify) {
    timer_sequence_stop();
    sequence_is_running_ = false;
    if (should_notify) {
        handle_sequence_update();
    }
}

void sequence_reset() {
    sequence_buffer_read_index = sequence_buffer_start_index;
    sequence_packet_index = 0;
}

SEQUENCE_PACKET_PUSH_RESULT sequence_push_packet(uint8_t *data, uint32_t length) {
    uint8_t sequence_number = data[0] & 0b01111111;

    NRF_LOG_DEBUG("packet seq %d\n", sequence_number);

    if (sequence_number == 0) {
        sequence_stop(false);
        sequence_reset();
        sequence_current_write_seq_num = 0;
        sequence_buffer_write_index = 0;
    }
    else if (sequence_number != (sequence_current_write_seq_num + 1)) {
        return PUSH_MISSED_PACKET;
    }
    sequence_current_write_seq_num = sequence_number;

    uint32_t payload_length = length - 1;
    uint8_t *payload = data + 1;

    if ((sequence_buffer_write_index + payload_length) >= BUFFER_SIZE) {
        return PUSH_OVERFLOW;
    }

    memcpy(sequence_buffer + sequence_buffer_write_index, payload, payload_length);

    sequence_buffer_write_index += payload_length;

    if ((data[0] & 0b10000000) == 0b00000000) {
        for (int i = 0; i < sequence_buffer_write_index; i++) {
        }
        return PUSH_FINAL_PACKET;
    }

    return PUSH_SUCCESS;
}

uint8_t sequence_is_running() {
    return sequence_is_running_;
}

uint32_t sequence_get_packet_index() {
    return sequence_packet_index;
}

uint32_t sequence_get_repeat_count() {
    return sequence_repeat_count;
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

void sequence_read_bytes(uint8_t *buffer, uint32_t *length) {
    uint32_t bytes_available = sequence_buffer_write_index - sequence_buffer_read_index;
    *length = MIN(bytes_available, *length);

    for (uint32_t i = 0; i < *length; i++) {
        buffer[i] = sequence_buffer[sequence_buffer_read_index];
        sequence_buffer_read_index++;
    }
}

uint8_t sequence_read_has_reached_end() {
    return sequence_buffer_read_index >= sequence_buffer_write_index;
}

void sequence_execute_packet(sequence_packet_t *packet) {
    if (sequence_pin_data_handler != NULL) {
        sequence_pin_data_handler(
            packet->pin_digital_data, 
            packet->pin_digital_data_length,
            packet->pin_analog_data, 
            packet->pin_analog_data_length
        );
    }
    timer_sequence_start(packet->delay);
}

void sequence_buffer_next_packet() {
    uint32_t pin_data_length = sequence_pin_digital_data_length;
    // the buffer copying could perhaps be skipped
    uint8_t pin_data_digital[pin_data_length];

    sequence_read_bytes(pin_data_digital, &pin_data_length);

    uint16_t analog_data[sequence_pin_analog_data_length * 2];
    uint32_t analog_data_length = 0;

    if(sequence_contains_analog){
        analog_data_length = sequence_pin_analog_data_length;
        sequence_read_bytes((uint8_t*) analog_data, &analog_data_length);
    }

    uint64_t delay = sequence_read_varint();

    sequence_packet_t next_packet = {
        .pin_digital_data = pin_data_digital,
        .pin_digital_data_length = pin_data_length,
        .pin_analog_data = analog_data,
        .pin_analog_data_length = analog_data_length,
        .delay = delay
    };

    sequence_execute_packet(&next_packet);
}

void sequence_step() {
    sequence_buffer_next_packet();

    handle_sequence_update();
    sequence_packet_index++;
}

void sequence_timer_timeout_handler() {
    if (sequence_read_has_reached_end()) {
        if (!repeat_indefinetly) {
            sequence_repeat_count--;
            if (sequence_repeat_count == 0) {
                sequence_is_running_ = false;
                handle_sequence_update();
                return;
            }
        }
        sequence_reset();
    }
    sequence_step();
}

void sequence_start(uint8_t contains_analog) {
    sequence_reset();
    sequence_buffer_read_index = 0;
    sequence_repeat_count = sequence_read_varint();
    repeat_indefinetly = (sequence_repeat_count == 0);
    sequence_contains_analog = contains_analog;

    sequence_buffer_start_index = sequence_buffer_read_index;

    sequence_is_running_ = true;

    sequence_step();
}

void sequence_init(
    uint32_t pin_data_digital_length,
    uint32_t pin_data_analog_length,
    pin_data_handler_t pin_data_handler,
    sequence_progress_update_handler_t progress_update_handler
) {
    sequence_pin_digital_data_length = pin_data_digital_length;
    sequence_pin_analog_data_length = pin_data_analog_length;
    timer_sequence_set_timeout_handler(sequence_timer_timeout_handler);
    sequence_pin_data_handler = pin_data_handler;
    sequence_progress_update_handler = progress_update_handler;
}