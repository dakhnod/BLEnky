#include "pin_configuration.h"
#include "nrf_log.h"

uint32_t pin_count_output;
uint32_t pin_count_input;

uint32_t current_output_pin_index;
uint32_t current_input_pin_index;

pin_output_handler_t pin_configuration_output_handler;
pin_input_handler_t pin_configuration_input_handler;

uint8_t is_pin_enabled(uint8_t pin_byte) {
  return (pin_byte & 0b1111) != 0b1111;
}

uint8_t is_output_pin_enabled(uint8_t pin_byte) {
  return (pin_byte & 0b1000) == 0b0000;
}

uint8_t is_input_pin_enabled(uint8_t pin_byte) {
  return (pin_byte & 0b1000) == 0b1000;
}

uint8_t get_pin_default_state(uint8_t pin_byte) {
  return (pin_byte >> 1) & 0b1;
}

uint8_t get_pin_invert(uint8_t pin_byte) {
  return (pin_byte >> 0) & 0b1;
}

uint8_t get_pin_pull(uint8_t pin_byte) {
  return (pin_byte >> 1) & 0b11;
}

void parse_pin_byte(uint32_t pin_index, uint8_t pin_data) {
  if (!is_pin_enabled(pin_data)) {
    return;
  }

  if (is_output_pin_enabled(pin_data)) {
    pin_configuration_output_handler(
      current_output_pin_index,
      pin_index,
      get_pin_default_state(pin_data),
      get_pin_invert(pin_data)
    );

    current_output_pin_index++;
  }
  else if (is_input_pin_enabled(pin_data)) {
    pin_configuration_input_handler(
      current_input_pin_index,
      pin_index,
      get_pin_pull(pin_data),
      get_pin_invert(pin_data)
    );

    current_input_pin_index++;
  }
}

void pin_data_for_each_pin(uint8_t *pin_data, uint32_t length, void (*handle_pin_data)(uint32_t pin_index, uint8_t pin_data)) {
  for (int i = 0; i < length; i++) {
    uint8_t data = pin_data[length - i - 1];
    handle_pin_data((i * 2 + 0), (data >> 0) & 0b1111);
    handle_pin_data((i * 2 + 1), (data >> 4) & 0b1111);
  }
}

void count_up_if_enabled(uint32_t pin_index, uint8_t pin_data) {
  if (is_pin_enabled(pin_data)) {
    if (is_output_pin_enabled(pin_data)) {
      pin_count_output++;
    }
    if (is_input_pin_enabled(pin_data)) {
      pin_count_input++;
    }
  }
}

uint32_t get_pin_count_output() {
  return pin_count_output;
}

uint32_t get_pin_count_input() {
  return pin_count_input;
}

void pin_configuration_init() {
  storage_init();

  uint8_t pin_configuration_data[16];
  pin_configuration_data_read(pin_configuration_data);

  // count output pins
  pin_data_for_each_pin(pin_configuration_data, 16, count_up_if_enabled);
}

void pin_configuration_parse(
  pin_output_handler_t output_handler,
  pin_input_handler_t input_handler
) {
  uint8_t pin_configuration_data[16];
  pin_configuration_data_read(pin_configuration_data);

  current_output_pin_index = 0;
  current_input_pin_index = 0;

  pin_configuration_output_handler = output_handler;
  pin_configuration_input_handler = input_handler;

  pin_data_for_each_pin(pin_configuration_data, 16, parse_pin_byte);
}

void pin_configuration_data_read(uint8_t *data) {
  storage_read(data, 16);
}