#include "sequence.h"

#define BUFFER_SIZE 512

uint8_t sequence_buffer[BUFFER_SIZE]; // 127 packets, each 19 bytes
uint32_t sequence_buffer_index = 0;
uint8_t sequence_current_seq_num = 0;

uint8_t *sequence_current_address = 0;

SEQUENCE_PACKET_PUSH_RESULT sequence_push_packet(uint8_t *data, uint32_t length) {
    uint8_t sequence_number = data[0] & 0b01111111;

    if (sequence_number == 0) {
        NRF_LOG_DEBUG("resetting buffer index\n");
        sequence_current_seq_num = 0;
        sequence_buffer_index = 0;
    }
    else if (sequence_number != (sequence_current_seq_num + 1)) {
        return PUSH_MISSED_PACKET;
    }
    sequence_current_seq_num = sequence_number;

    uint32_t payload_length = length - 1;
    uint8_t *payload = data + 1;

    if ((sequence_buffer_index + payload_length) >= BUFFER_SIZE) {
        return PUSH_OVERFLOW;
    }

    NRF_LOG_DEBUG("received %d bytes\n", payload_length);

    memcpy(sequence_buffer + sequence_buffer_index, payload, payload_length);

    sequence_buffer_index += payload_length;

    if (data[0] & 0b10000000) {
        for (int i = 0; i < sequence_buffer_index; i++) {
        }
        return PUSH_FINAL_PACKET;
    }

    return PUSH_SUCCESS;
}

void sequence_start() {
    sequence_current_address = sequence_buffer;
    uint32_t current_buffer_index = sequence_current_address - sequence_buffer;
    while (current_buffer_index < sequence_buffer_index) {
        uint32_t next_int = varint_decode_next_varint(&sequence_current_address);
        NRF_LOG_DEBUG("next int: %d\n", next_int);
    }
    NRF_LOG_DEBUG("done decoding\n");
}

uint32_t varint_decode_next_varint(uint8_t **data) {
    uint32_t current = 0;
    for (uint32_t i = 0; i < 4; i++) {
        uint8_t byte = **data;
        (*data)++;
        current |= ((byte & 0b01111111) << (i * 7));

        if (!(byte & 0b10000000)) {
            return current;
        }
    }
    return 0;
}