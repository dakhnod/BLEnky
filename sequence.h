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

uint64_t varint_decode_next_varint(uint8_t **data);
SEQUENCE_PACKET_PUSH_RESULT sequence_push_packet(uint8_t *data, uint32_t length);
void sequence_start();