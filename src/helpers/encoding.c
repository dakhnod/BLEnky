#include "encoding.h"

#include "sdk_common.h"

uint8_t encoding_get_pin_bits(const uint8_t *pin_data, uint32_t pin_data_length, uint32_t pin_index){
        uint32_t bit_index_full = pin_index * 2;

        uint32_t byte_index = bit_index_full / 8;
        uint8_t bit_index = bit_index_full % 8;

        uint8_t current_byte = pin_data[pin_data_length - byte_index - 1];

        uint8_t output_bits = (current_byte >> bit_index) & 0b00000011;

        return output_bits;
}

uint32_t encoding_get_byte_count_from_pins(uint32_t pin_count)
{
    return CEIL_DIV(pin_count, 4);
}