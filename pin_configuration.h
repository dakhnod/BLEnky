#include "storage.h"


typedef void (*pin_output_handler_t)(uint32_t index, uint32_t pin, uint8_t default_state, uint8_t invert);
typedef void (*pin_input_handler_t)(uint32_t index, uint32_t pin, uint8_t pull, uint8_t invert);

void pin_configuration_data_read(uint8_t *data);
void pin_configuration_init();
void pin_configuration_parse(
  pin_output_handler_t output_handler,
  pin_input_handler_t input_handler
);
uint32_t get_pin_count_output();
uint32_t get_pin_count_input();