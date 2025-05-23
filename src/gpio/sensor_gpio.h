#ifndef SENSOR_GPIO_H
#define SENSOR_GPIO_H

#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
#include "pin_configuration.h"

#define BATTERY_VOLTAGE_MAX 3000.0
#define BATTERY_VOLTAGE_MIN 1700.0

#define GPIO_OUTPUT_ANALOG_PIN_LIMIT 2

typedef struct
{
  uint32_t pin;
  uint8_t state;
  uint8_t default_state;
  uint8_t invert;
} gpio_config_output_digital_t;

typedef struct
{
  uint32_t pin;
  uint8_t state;
  uint8_t pull;
  uint8_t invert;
  uint8_t ignored_state;
  uint8_t ignore_input;
  uint8_t changed;
  uint8_t virtual;
  uint32_t trigger_count;
} gpio_config_input_digital_t;

typedef void (*gpio_input_change_handler_t)(int highest_changed_index);

void gpio_init(gpio_input_change_handler_t input_change_handler);
void gpio_configure_aio_outputs();
void gpio_write_output_digital_pin(uint32_t index, uint8_t new_state);
void gpio_write_output_analog_pin_ticks(uint32_t index, uint16_t value);
void gpio_write_output_analog_pin_us(uint32_t index, uint16_t us);
uint32_t gpio_get_output_digital_pin_count();
uint32_t gpio_get_output_analog_pin_count();
uint32_t gpio_get_input_digital_pin_count();
uint8_t gpio_get_output_digital_state(uint32_t index);
bool gpio_get_input_digital_state(uint32_t index);
void gpio_encode_output_states(uint8_t *buffer);
void gpio_encode_input_states(uint8_t *buffer);
gpio_config_input_digital_t *gpio_find_input_by_index(uint32_t index);

#endif
