#include "storage.h"

void pin_configuration_data_read(uint8_t *data);
void pin_configuration_init();
void pin_configuration_parse(
  uint32_t *output_pins,
  uint8_t *output_pin_default_states,
  uint8_t *output_pin_invert,
  uint32_t *input_pins,
  uint8_t *input_pin_pulls,
  uint8_t *input_pin_invert
);
uint32_t get_pin_count_output();
uint32_t get_pin_count_input();