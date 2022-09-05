#include "sensor_gpio.h"
#include "pin_configuration.h"
#include "mem_manager.h"
#include "app_timer.h"
#include "ble_configuration.h"
#include "nrf_delay.h"

uint32_t gpio_output_pin_count = 0;
uint32_t gpio_input_pin_count = 0;

gpio_config_output_t *gpio_output_configs;
gpio_config_input_t *gpio_input_configs;

gpio_input_change_handler_t input_change_handler = NULL;

void gpio_write_output_pin(uint32_t index, uint8_t value) {
  gpio_config_output_t *config = gpio_output_configs + index;
  uint32_t pin = config->pin;
  if (value ^ config->invert) {
    nrf_gpio_pin_set(pin);
  }
  else {
    nrf_gpio_pin_clear(pin);
  }
  config->state = value;
}

void gpio_set_input_change_handler(gpio_input_change_handler_t handler) {
  input_change_handler = handler;
}

uint32_t gpio_get_output_pin_count() {
  return gpio_output_pin_count;
}

uint32_t gpio_get_input_pin_count() {
  return gpio_input_pin_count;
}

uint8_t gpio_get_output_state(uint32_t index) {
  return gpio_output_configs[index].state;
}

void gpio_encode_output_states(uint8_t *buffer) {
  for (int i = 0; i < gpio_output_pin_count; i++) {
    buffer[i] = gpio_output_configs[i].state;
  }
}

void gpio_encode_input_states(uint8_t *buffer) {
  for (int i = 0; i < gpio_input_pin_count; i++) {
    buffer[i] = gpio_input_configs[i].state;
  }
}

void gpio_configure_aio_outputs() {
  for (int i = 0; i < gpio_output_pin_count; i++) {
    gpio_config_output_t *config = gpio_output_configs + i;
    nrf_gpio_cfg_output(config->pin);
    gpio_write_output_pin(i, config->default_state);
  };
}

void on_pin_changed(uint32_t index) {
  gpio_config_input_t *config = gpio_input_configs + index;
  NRF_LOG_DEBUG("pin %d (%d) changed to %d\n", index, config->pin, config->state);

  if (config->state == 0x01) {
    config->trigger_count++;
  }

  if (input_change_handler != NULL) {
    input_change_handler(index, gpio_input_configs + index);
  }

  config->ignore_input = true;

  sensor_timer_debounce_timer_start(index);
}

void gpio_debounce_timeout_handler(uint32_t timer_index) {

  gpio_config_input_t *config = gpio_input_configs + timer_index;

  if (config->ignored_state == config->state) {
    config->ignore_input = false;
    return;
  }

  config->state = config->ignored_state;
  on_pin_changed(timer_index);
}

void gpio_pin_toggle_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  uint32_t pin_index = 0;
  for (uint32_t i = 1; i < gpio_input_pin_count; i++) {
    gpio_config_input_t *config = gpio_input_configs + i;
    if (config->pin == pin) {
      pin_index = i;
      break;
    }
  }

  gpio_config_input_t *config = gpio_input_configs + pin_index;
  uint8_t is_high = (action == NRF_GPIOTE_POLARITY_LOTOHI);
  is_high ^= config->invert;
  config->ignored_state = is_high;

  if (config->ignore_input) {
    return;
  }

  config->state = is_high;
  on_pin_changed(pin_index);
}

void gpio_configure_aio_inputs() {
  ret_code_t err_code;

  for (int i = 0; i < gpio_input_pin_count; i++) {
    gpio_config_input_t *pin_config = gpio_input_configs + i;
    uint32_t pin = pin_config->pin;
    uint8_t pull = pin_config->pull;

    nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);

    if (pull == 0x01) {
      config.pull = NRF_GPIO_PIN_PULLUP;
    }
    else if (pull == 0x02) {
      config.pull = NRF_GPIO_PIN_PULLDOWN;
    }

    err_code = nrf_drv_gpiote_in_init(pin, &config, gpio_pin_toggle_handler);
    APP_ERROR_CHECK(err_code);

    pin_config->state = nrf_gpio_pin_read(pin) ^ pin_config->invert;
    pin_config->ignored_state = pin_config->state;
    pin_config->trigger_count = 0;

    nrf_drv_gpiote_in_event_enable(pin, true);
  }
}

void gpio_handle_parse_output(uint32_t index, uint32_t pin, uint8_t default_state, uint8_t invert) {
  gpio_config_output_t *config = gpio_output_configs + index;
  config->pin = pin;
  config->default_state = default_state;
  config->invert = invert;
}

void gpio_handle_parse_input(uint32_t index, uint32_t pin, uint8_t pull, uint8_t invert) {
  gpio_config_input_t *config = gpio_input_configs + index;
  config->pin = pin;
  config->pull = pull;
  config->invert = invert;
  config->ignore_input = false;
}

void gpio_init() {
  // ret_code_t err_code;

  ret_code_t err_code;
  err_code = nrf_drv_gpiote_init();
  APP_ERROR_CHECK(err_code);

  err_code = nrf_mem_init();
  APP_ERROR_CHECK(err_code);

  pin_configuration_init();

  gpio_output_pin_count = get_pin_count_output();
  gpio_input_pin_count = get_pin_count_input();

  uint32_t size;
  ret_code_t result;

  if (gpio_output_pin_count > 0) {
    size = sizeof(gpio_config_output_t) * gpio_output_pin_count;
    result = nrf_mem_reserve(
      (uint8_t **)&gpio_output_configs,
      &size
    );
    APP_ERROR_CHECK(result);
  }

  if (gpio_input_pin_count > 0) {
    sensor_timer_initialize_debounce_timers(gpio_input_pin_count, gpio_debounce_timeout_handler);

    size = sizeof(gpio_config_input_t) * gpio_input_pin_count;
    result = nrf_mem_reserve(
      (uint8_t **)&gpio_input_configs,
      &size
    );
    APP_ERROR_CHECK(result);
  }

  pin_configuration_parse(
    gpio_handle_parse_output,
    gpio_handle_parse_input
  );

  if (gpio_output_pin_count > 0) {
    gpio_configure_aio_outputs();
  }
  else {
    NRF_LOG_INFO("no output pins configured\n");
  }
  if (gpio_input_pin_count > 0) {
    gpio_configure_aio_inputs();
  }
  else {
    NRF_LOG_INFO("no input pins configured\n");
  }

  for (int i = 0; i < gpio_output_pin_count; i++) {
    gpio_config_output_t *config = gpio_output_configs + i;
    NRF_LOG_INFO("pin output: %d\n", config->pin);
    NRF_LOG_INFO("pin default state: %d\n", config->default_state);
    NRF_LOG_INFO("pin invert: %d\n\n", config->invert);
  }

  for (int i = 0; i < gpio_input_pin_count; i++) {
    gpio_config_input_t *config = gpio_input_configs + i;
    NRF_LOG_INFO("pin input: %d\n", config->pin);
    NRF_LOG_INFO("pin pull: %d\n", config->pull);
    NRF_LOG_INFO("pin invert: %d\n", config->invert);
  }

}