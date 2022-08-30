#include "sensor_gpio.h"
#include "pin_configuration.h"
#include "mem_manager.h"

uint32_t *gpio_output_pins;
uint32_t gpio_output_pin_count = 0;
uint8_t *gpio_output_default_states;
uint8_t *gpio_output_pin_invert;
uint8_t *gpio_output_pin_states;

uint32_t *gpio_input_pins;
uint32_t gpio_input_pin_count = 0;
uint8_t *gpio_input_pulls;
uint8_t *gpio_input_states;
uint8_t *gpio_input_invert;

void gpio_write_output_pin(uint32_t index, uint8_t value) {
  uint32_t pin = gpio_output_pins[index];
  if (value ^ gpio_output_pin_invert[index]) {
    nrf_gpio_pin_set(pin);
  }
  else {
    nrf_gpio_pin_clear(pin);
  }
  gpio_output_pin_states[index] = value;
}

uint32_t gpio_get_output_pin_count() {
  return gpio_output_pin_count;
}

uint32_t gpio_get_input_pin_count() {
  return gpio_input_pin_count;
}

uint8_t gpio_get_output_state(uint32_t index) {
  return gpio_output_pin_states[index];
}

uint8_t *gpio_get_output_states() {
  return gpio_output_pin_states;
}

void gpio_configure_aio_outputs() {
  for (int i = 0; i < gpio_output_pin_count; i++) {
    nrf_gpio_cfg_output(gpio_output_pins[i]);
    gpio_write_output_pin(i, gpio_output_default_states[i]);
  };
}

void gpio_pin_configuration_data_read(uint8_t *data) {
  pin_configuration_data_read(data);
}

void parse_configuration_data() {
  uint8_t configuration_data[16];
  gpio_pin_configuration_data_read(configuration_data);
}

void gpio_init(void (*sensor_handler_)(uint8_t)) {
  // ret_code_t err_code;

  ret_code_t err_code;

  err_code = nrf_mem_init();
  APP_ERROR_CHECK(err_code);

  pin_configuration_init();

  gpio_output_pin_count = get_pin_count_output();
  gpio_input_pin_count = get_pin_count_input();

  NRF_LOG_DEBUG("output pin count: %d\n", gpio_output_pin_count);

  uint32_t size;
  ret_code_t result;

  if (gpio_output_pin_count > 0) {
    size = gpio_output_pin_count * 4;
    result = nrf_mem_reserve(
      (uint8_t **)&gpio_output_pins,
      &size
    );
    APP_ERROR_CHECK(result);

    size = gpio_output_pin_count;
    result = nrf_mem_reserve(
      &gpio_output_default_states,
      &size
    );
    APP_ERROR_CHECK(result);

    size = gpio_output_pin_count;
    result = nrf_mem_reserve(
      &gpio_output_pin_invert,
      &size
    );
    APP_ERROR_CHECK(result);

    size = gpio_output_pin_count;
    result = nrf_mem_reserve(
      &gpio_output_pin_states,
      &size
    );
    APP_ERROR_CHECK(result);

    gpio_configure_aio_outputs();
  }

  if (gpio_input_pin_count > 0) {
    size = gpio_input_pin_count * 4;
    result = nrf_mem_reserve(
      (uint8_t **)&gpio_input_pins,
      &size
    );
    APP_ERROR_CHECK(result);

    size = gpio_input_pin_count;
    result = nrf_mem_reserve(
      (uint8_t **)&gpio_input_pulls,
      &size
    );
    APP_ERROR_CHECK(result);

    size = gpio_input_pin_count;
    result = nrf_mem_reserve(
      (uint8_t **)&gpio_input_states,
      &size
    );
    APP_ERROR_CHECK(result);

    size = gpio_input_pin_count;
    result = nrf_mem_reserve(
      (uint8_t **)&gpio_input_invert,
      &size
    );
    APP_ERROR_CHECK(result);
  }



  pin_configuration_parse(
    gpio_output_pins,
    gpio_output_default_states,
    gpio_output_pin_invert,
    gpio_input_pins,
    gpio_input_pulls,
    gpio_input_invert
  );

  for (int i = 0; i < gpio_output_pin_count; i++) {
    NRF_LOG_INFO("pin output: %d\n", gpio_output_pins[i]);
    NRF_LOG_INFO("pin default state: %d\n", gpio_output_default_states[i]);
    NRF_LOG_INFO("pin invert: %d\n\n", gpio_output_pin_invert[i]);
  }

  for (int i = 0; i < gpio_input_pin_count; i++) {
    NRF_LOG_INFO("pin input: %d\n", gpio_input_pins[i]);
    NRF_LOG_INFO("pin pull: %d\n", gpio_input_pulls[i]);
    NRF_LOG_INFO("pin invert: %d\n", gpio_input_invert[i]);
  }

}