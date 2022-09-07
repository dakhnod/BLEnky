#include "sensor_gpio.h"
#include "pin_configuration.h"
#include "mem_manager.h"
#include "app_timer.h"
#include "ble_configuration.h"
#include "nrf_delay.h"
#include "app_pwm.h"

uint32_t gpio_output_digital_pin_count = 0;
uint32_t gpio_output_analog_pin_count = 0;
uint32_t gpio_input_digital_pin_count = 0;

gpio_config_output_digital_t *gpio_output_configs;
gpio_config_input_digital_t *gpio_input_configs;

gpio_input_change_handler_t input_change_handler = NULL;

app_pwm_config_t gpio_output_analog_config = APP_PWM_DEFAULT_CONFIG_2CH(20000L, APP_PWM_NOPIN, APP_PWM_NOPIN);
APP_PWM_INSTANCE(pwm0, 1);

void gpio_write_output_digital_pin(uint32_t index, uint8_t value) {
  gpio_config_output_digital_t *config = gpio_output_configs + index;
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

uint32_t gpio_get_output_digital_pin_count() {
  return gpio_output_digital_pin_count;
}

uint32_t gpio_get_output_analog_pin_count() {
  return gpio_output_analog_pin_count;
}

uint32_t gpio_get_input_digital_pin_count() {
  return gpio_input_digital_pin_count;
}

uint8_t gpio_get_output_digital_state(uint32_t index) {
  return gpio_output_configs[index].state;
}

void gpio_encode_output_states(uint8_t *buffer) {
  for (int i = 0; i < gpio_output_digital_pin_count; i++) {
    buffer[i] = gpio_output_configs[i].state;
  }
}

void gpio_encode_input_states(uint8_t *buffer) {
  for (int i = 0; i < gpio_input_digital_pin_count; i++) {
    buffer[i] = gpio_input_configs[i].state;
  }
}

void gpio_configure_aio_outputs_digital() {
  for (int i = 0; i < gpio_output_digital_pin_count; i++) {
    gpio_config_output_digital_t *config = gpio_output_configs + i;
    nrf_gpio_cfg_output(config->pin);
    gpio_write_output_digital_pin(i, config->default_state);
  };
}

void gpio_write_output_analog_pin_ticks(uint32_t index, uint16_t value){
  while (app_pwm_channel_duty_ticks_set(&pwm0, 0, value) == NRF_ERROR_BUSY);
}

void gpio_write_output_analog_pin_ms(uint32_t index, uint16_t ms){
  gpio_write_output_analog_pin_ticks(index, ms * 2);
}

void gpio_configure_aio_outputs_analog(){
  ret_code_t err_code;
  err_code = app_pwm_init(&pwm0, &gpio_output_analog_config, NULL);
  APP_ERROR_CHECK(err_code);

  app_pwm_enable(&pwm0);
}

void on_pin_changed(uint32_t index) {
  gpio_config_input_digital_t *config = gpio_input_configs + index;
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

  gpio_config_input_digital_t *config = gpio_input_configs + timer_index;

  if (config->ignored_state == config->state) {
    config->ignore_input = false;
    return;
  }

  config->state = config->ignored_state;
  on_pin_changed(timer_index);
}

void gpio_pin_toggle_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  uint32_t pin_index = 0;
  for (uint32_t i = 1; i < gpio_input_digital_pin_count; i++) {
    gpio_config_input_digital_t *config = gpio_input_configs + i;
    if (config->pin == pin) {
      pin_index = i;
      break;
    }
  }

  gpio_config_input_digital_t *config = gpio_input_configs + pin_index;
  uint8_t is_high = (action == NRF_GPIOTE_POLARITY_LOTOHI);
  is_high ^= config->invert;
  config->ignored_state = is_high;

  if (config->ignore_input) {
    return;
  }

  config->state = is_high;
  on_pin_changed(pin_index);
}

void gpio_configure_aio_inputs_digital() {
  ret_code_t err_code;

  for (int i = 0; i < gpio_input_digital_pin_count; i++) {
    gpio_config_input_digital_t *pin_config = gpio_input_configs + i;
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

void gpio_handle_parse_output_digital(uint32_t index, uint32_t pin, uint8_t default_state, uint8_t invert) {
  gpio_config_output_digital_t *config = gpio_output_configs + index;
  config->pin = pin;
  config->default_state = default_state;
  config->invert = invert;
}

void gpio_handle_parse_output_analog(uint32_t index, uint32_t pin, uint8_t invert) {
  static uint32_t analog_pin_index = 0;
  if(index >= GPIO_OUTPUT_ANALOG_PIN_LIMIT){
    return;
  }
  gpio_output_analog_config.pins[index] = pin;
  gpio_output_analog_config.pin_polarity[index] = invert ? APP_PWM_POLARITY_ACTIVE_LOW : APP_PWM_POLARITY_ACTIVE_HIGH;
  analog_pin_index++;
}

void gpio_handle_parse_input_digital(uint32_t index, uint32_t pin, uint8_t pull, uint8_t invert) {
  gpio_config_input_digital_t *config = gpio_input_configs + index;
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

  gpio_output_digital_pin_count = gpio_get_output_digital_pin_count();
  gpio_output_analog_pin_count = gpio_get_output_analog_pin_count();
  gpio_input_digital_pin_count = gpio_get_input_digital_pin_count();

  uint32_t size;
  ret_code_t result;

  if (gpio_output_digital_pin_count > 0) {
    size = sizeof(gpio_config_output_digital_t) * gpio_output_digital_pin_count;
    result = nrf_mem_reserve(
      (uint8_t **)&gpio_output_configs,
      &size
    );
    APP_ERROR_CHECK(result);
  }

  if (gpio_input_digital_pin_count > 0) {
    sensor_timer_initialize_debounce_timers(gpio_input_digital_pin_count, gpio_debounce_timeout_handler);

    size = sizeof(gpio_config_input_digital_t) * gpio_input_digital_pin_count;
    result = nrf_mem_reserve(
      (uint8_t **)&gpio_input_configs,
      &size
    );
    APP_ERROR_CHECK(result);
  }

  pin_configuration_parse(
    gpio_handle_parse_output_digital,
    gpio_handle_parse_output_analog,
    gpio_handle_parse_input_digital
  );

  if (gpio_output_digital_pin_count > 0) {
    gpio_configure_aio_outputs_digital();
  }else{
    NRF_LOG_INFO("no digital output pins configured\n");
  }

  if(gpio_output_analog_pin_count > 0){
    gpio_configure_aio_outputs_analog();
  }else{
    NRF_LOG_INFO("no analog output pins configured\n");
  }

  if (gpio_input_digital_pin_count > 0) {
    gpio_configure_aio_inputs_digital();
  }else {
    NRF_LOG_INFO("no digital input pins configured\n");
  }

  for (int i = 0; i < gpio_output_digital_pin_count; i++) {
    gpio_config_output_digital_t *config = gpio_output_configs + i;
    NRF_LOG_INFO("pin output: %d\n", config->pin);
    NRF_LOG_INFO("pin default state: %d\n", config->default_state);
    NRF_LOG_INFO("pin invert: %d\n\n", config->invert);
  }

  for (int i = 0; i < gpio_input_digital_pin_count; i++) {
    gpio_config_input_digital_t *config = gpio_input_configs + i;
    NRF_LOG_INFO("pin input: %d\n", config->pin);
    NRF_LOG_INFO("pin pull: %d\n", config->pull);
    NRF_LOG_INFO("pin invert: %d\n", config->invert);
  }

}