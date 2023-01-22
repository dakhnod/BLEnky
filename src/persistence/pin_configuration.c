#include "pin_configuration.h"
#include "nrf_log.h"
#include "storage.h"

uint32_t pin_count_output_digital = 0;
uint32_t pin_count_output_analog = 0;
uint32_t pin_count_input_digital = 0;

pin_output_analog_handler_t pin_configuration_output_analog_handler;
pin_output_digital_handler_t pin_configuration_output_digital_handler;
pin_input_digital_handler_t pin_configuration_input_digital_handler;

uint8_t is_pin_enabled(uint8_t pin_byte) {
  return (pin_byte & 0b1111) != 0b1111;
}

uint8_t is_output_pin_enabled(uint8_t pin_byte) {
  return (pin_byte & 0b1000) == 0b0000;
}

uint8_t is_output_pin_analog(uint8_t pin_byte) {
  return (pin_byte & 0b0100) == 0b0100;
}

uint8_t get_output_digital_pin_default_state(uint8_t pin_byte) {
  return (pin_byte & 0b0010) & 0b0010;
}

uint8_t is_input_pin_enabled(uint8_t pin_byte) {
  return (pin_byte & 0b1000) == 0b1000;
}

uint8_t get_input_digital_pin_pull(uint8_t pin_byte) {
  return (pin_byte & 0b0110) >> 1;
}

uint8_t get_pin_invert(uint8_t pin_byte) {
  return (pin_byte & 0b0001) & 0b0001;
}

void parse_pin_byte(uint32_t pin_index, uint8_t pin_data) {
  if (!is_pin_enabled(pin_data)) {
    return;
  }

  uint8_t invert = get_pin_invert(pin_data);

  static uint32_t pin_number = 0;
  static uint32_t pin_number_analog = 0;

  if (is_output_pin_enabled(pin_data)) {
    if(is_output_pin_analog(pin_data)){
      pin_configuration_output_analog_handler(
        pin_number_analog++,
        pin_index,
        invert
      );
    }else{
      pin_configuration_output_digital_handler(
        pin_number++,
        pin_index,
        get_output_digital_pin_default_state(pin_data),
        invert
      );
    }
  }
  else if (is_input_pin_enabled(pin_data)) {
    pin_configuration_input_digital_handler(
      pin_number++,
      pin_index,
      get_input_digital_pin_pull(pin_data),
      invert
    );
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
  if(!is_pin_enabled(pin_data)){
    return;
  }
  if (is_output_pin_enabled(pin_data)) {
    if(is_output_pin_analog(pin_data)){
      pin_count_output_analog++;
    }else{
      pin_count_output_digital++;
    }
  }else if (is_input_pin_enabled(pin_data)) {
    pin_count_input_digital++;
  }
}

uint32_t get_pin_count_output_digital() {
  return pin_count_output_digital;
}

uint32_t get_pin_count_output_analog() {
  return pin_count_output_analog;
}

uint32_t get_pin_count_input_digital() {
  return pin_count_input_digital;
}

void pin_configuration_init() {
  storage_init();

  uint8_t pin_configuration_data[16];
  ret_code_t err_code = pin_configuration_data_read(pin_configuration_data);

  if(err_code == NRF_SUCCESS){
    // count output pins
    pin_data_for_each_pin(pin_configuration_data, 16, count_up_if_enabled);
  }
}

void pin_configuration_parse(
  pin_output_digital_handler_t output_digital_handler,
  pin_output_analog_handler_t output_analog_handler,
  pin_input_digital_handler_t input_digital_handler
) {
  uint8_t pin_configuration_data[16];
  ret_code_t err_code = pin_configuration_data_read(pin_configuration_data);
  if(err_code == NRF_SUCCESS){
    pin_configuration_output_digital_handler = output_digital_handler;
    pin_configuration_output_analog_handler = output_analog_handler;
    pin_configuration_input_digital_handler = input_digital_handler;

    pin_data_for_each_pin(pin_configuration_data, 16, parse_pin_byte);
  }
}

ret_code_t pin_configuration_data_read(uint8_t *data) {
  return storage_read_pin_configuration(data);
}