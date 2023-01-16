#include "sensor_gpio.h"
#include "pin_configuration.h"
#include "app_timer.h"
#include "ble_configuration.h"
#include "app_pwm.h"

#define MAX_PIN_COUNT 32

uint32_t gpio_output_digital_pin_count = 0;
uint32_t gpio_output_analog_pin_count = 0;
uint32_t gpio_input_digital_pin_count = 0;

typedef enum {
  OUTPUT,
  INPUT
} direction_t;

typedef struct {
  union {
    gpio_config_output_digital_t output;
    gpio_config_input_digital_t input;
  } pin;
  direction_t direction;
} gpio_config_t;

gpio_config_t gpio_configs[MAX_PIN_COUNT];

gpio_input_change_handler_t gpio_input_change_handler = NULL;

app_pwm_config_t gpio_output_analog_config = APP_PWM_DEFAULT_CONFIG_2CH(20000L, APP_PWM_NOPIN, APP_PWM_NOPIN);
APP_PWM_INSTANCE(pwm0, 1);

gpio_config_t *find_gpio_config_by_index(uint32_t index, direction_t direction){
  for(uint32_t i = 0; i < MAX_PIN_COUNT; i++){
    gpio_config_t *current = gpio_configs + i;
    if(current->direction == direction){
      if(index == 0){
        return current;
      }
      index--;
    }
  }

  return NULL;
}

gpio_config_output_digital_t *find_gpio_output_by_index(uint32_t index){
  return &(find_gpio_config_by_index(index, OUTPUT)->pin.output);
}

gpio_config_input_digital_t *find_gpio_input_by_index(uint32_t index){
  return &(find_gpio_config_by_index(index, INPUT)->pin.input);
}

void gpio_write_output_digital_pin(uint32_t index, uint8_t value) {
  gpio_config_output_digital_t *config = find_gpio_output_by_index(index);
  uint32_t pin = config->pin;
  if (value ^ config->invert) {
    nrf_gpio_pin_set(pin);
  }
  else {
    nrf_gpio_pin_clear(pin);
  }
  config->state = value;
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
  return find_gpio_output_by_index(index)->state;
}

bool gpio_get_input_digital_state(uint32_t index) {
  return find_gpio_input_by_index(index)->state;
}

void gpio_encode_states(uint8_t *buffer, direction_t direction){
  uint32_t current_index = 0;
  for(uint32_t i = 0; current_index < gpio_output_digital_pin_count; i++){
    gpio_config_t *config = gpio_configs + i;
    if(config->direction != direction){
      continue;
    }
    // using output here even for inputs since state byte should be at the same location for inputs and outputs
    buffer[current_index++] = config->pin.output.state;
  }
}

void gpio_encode_output_states(uint8_t *buffer) {
  gpio_encode_states(buffer, OUTPUT);
}

void gpio_encode_input_states(uint8_t *buffer) {
    gpio_encode_states(buffer, INPUT);
}

void gpio_configure_aio_outputs_digital() {
  uint32_t current_index = 0;
  for (int i = 0; current_index < gpio_output_digital_pin_count; i++) {
    gpio_config_t *config = gpio_configs + i;
    if(config->direction != OUTPUT){
      continue;
    }
    nrf_gpio_cfg_output(config->pin.output.pin);
    gpio_write_output_digital_pin(i, config->pin.output.default_state);
    current_index++;
  };
}

void gpio_write_output_analog_pin_ticks(uint32_t index, uint16_t value){
  while (app_pwm_channel_duty_ticks_set(&pwm0, index, value) == NRF_ERROR_BUSY);
}

void gpio_write_output_analog_pin_us(uint32_t index, uint16_t us){
  gpio_write_output_analog_pin_ticks(index, us * 2);
}

void gpio_configure_aio_outputs_analog(){
  ret_code_t err_code;
  err_code = app_pwm_init(&pwm0, &gpio_output_analog_config, NULL);
  APP_ERROR_CHECK(err_code);

  app_pwm_enable(&pwm0);
}

void on_pin_changed(uint32_t index) {
  gpio_config_input_digital_t *config = find_gpio_input_by_index(index);
  NRF_LOG_DEBUG("pin %d (%d) changed to %d\n", index, config->pin, config->state);

  if (config->state == 0x01) {
    config->trigger_count++;
  }

  if (gpio_input_change_handler != NULL) {
    gpio_input_change_handler(index, config);
  }

  config->ignore_input = true;

  sensor_timer_debounce_timer_start(index);
}

void gpio_debounce_timeout_handler(uint32_t timer_index) {

  gpio_config_input_digital_t *config = find_gpio_input_by_index(timer_index);

  if (config->ignored_state == config->state) {
    config->ignore_input = false;
    return;
  }

  config->state = config->ignored_state;
  on_pin_changed(timer_index);
}

void gpio_pin_toggle_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  uint32_t pin_index = 0;
  gpio_config_input_digital_t *config = NULL;

  for (uint32_t i = 0; i < MAX_PIN_COUNT; i++) {
    gpio_config_t *cfg = gpio_configs + i;
    // ignore output configs
    if(cfg->direction != INPUT){
      continue;
    }
    config = &(cfg->pin.input);
    // check if proper pin
    if(config->pin != pin){
      continue;
    }
    break;
  }

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

  uint32_t current_index = 0;
  for (int i = 0; current_index < gpio_input_digital_pin_count; i++) {
    gpio_config_t *config = gpio_configs + i;
    if(config->direction != INPUT){
      continue;
    }
    current_index++;
    gpio_config_input_digital_t *pin_config = &(config->pin.input);
    uint32_t pin = pin_config->pin;
    uint8_t pull = pin_config->pull;

    nrf_drv_gpiote_in_config_t drv_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);

    if (pull == 0x01) {
      drv_config.pull = NRF_GPIO_PIN_PULLUP;
    }
    else if (pull == 0x02) {
      drv_config.pull = NRF_GPIO_PIN_PULLDOWN;
    }

    err_code = nrf_drv_gpiote_in_init(pin, &drv_config, gpio_pin_toggle_handler);
    APP_ERROR_CHECK(err_code);

    pin_config->state = nrf_gpio_pin_read(pin) ^ pin_config->invert;
    pin_config->ignored_state = pin_config->state;
    pin_config->trigger_count = 0;

    nrf_drv_gpiote_in_event_enable(pin, true);
  }
}

void gpio_handle_parse_output_digital(uint32_t index, uint32_t pin, uint8_t default_state, uint8_t invert) {
  gpio_config_t *config = find_gpio_config_by_index(index, OUTPUT);
  config->direction = OUTPUT;
  config->pin.output.pin = pin;
  config->pin.output.default_state = default_state;
  config->pin.output.invert = invert;
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
  gpio_config_t *config = find_gpio_config_by_index(index, INPUT);
  config->direction = INPUT;
  config->pin.input.pin = pin;
  config->pin.input.pull = pull;
  config->pin.input.invert = invert;
  config->pin.input.ignore_input = false;
}

void gpio_init(gpio_input_change_handler_t input_change_handler) {
  // ret_code_t err_code;

  ret_code_t err_code;
  err_code = nrf_drv_gpiote_init();
  APP_ERROR_CHECK(err_code);

  pin_configuration_init();

  gpio_output_digital_pin_count = get_pin_count_output_digital();
  gpio_output_analog_pin_count = get_pin_count_output_analog();
  gpio_input_digital_pin_count = get_pin_count_input_digital();

  if (gpio_input_digital_pin_count > 0) {
    sensor_timer_initialize_debounce_timers(gpio_input_digital_pin_count, gpio_debounce_timeout_handler);
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
    gpio_config_output_digital_t *config = find_gpio_output_by_index(i);
    NRF_LOG_DEBUG("pin output: %d\n", config->pin);
    NRF_LOG_DEBUG("pin default state: %d\n", config->default_state);
    NRF_LOG_DEBUG("pin invert: %d\n\n", config->invert);
  }

  for (int i = 0; i < gpio_input_digital_pin_count; i++) {
    gpio_config_input_digital_t *config = find_gpio_input_by_index(i);
    NRF_LOG_DEBUG("pin input: %d\n", config->pin);
    NRF_LOG_DEBUG("pin pull: %d\n", config->pull);
    NRF_LOG_DEBUG("pin invert: %d\n", config->invert);
  }

  gpio_input_change_handler = input_change_handler;
}