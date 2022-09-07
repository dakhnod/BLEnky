#ifndef SENSOR_GPIO_H
#define SENSOR_GPIO_H

#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
#include "boards.h"
#include "bsp.h"
#include "pin_configuration.h"

#define BATTERY_VOLTAGE_MAX 3000.0
#define BATTERY_VOLTAGE_MIN 1700.0

#define GPIO_OUTPUT_ANALOG_PIN_LIMIT 2

typedef struct
{
  uint32_t pin;
  uint8_t default_state;
  uint8_t invert;
  uint8_t state;
} gpio_config_output_digital_t;

typedef struct
{
  uint32_t pin;
  uint8_t pull;
  uint8_t invert;
  uint8_t state;
  uint8_t ignored_state;
  uint8_t ignore_input;
  uint32_t trigger_count;
} gpio_config_input_digital_t;

typedef void (*gpio_input_change_handler_t)(uint32_t pin_index, gpio_config_input_digital_t *config);

void gpio_init();

void gpio_configure_aio_outputs();
void gpio_write_output_digital_pin(uint32_t index, uint8_t value);
uint32_t gpio_get_output_digital_pin_count();
uint32_t gpio_get_output_analog_pin_count();
uint32_t gpio_get_input_digital_pin_count();
uint8_t gpio_get_output_digital_state(uint32_t index);
void gpio_encode_output_states(uint8_t *buffer);
void gpio_encode_input_states(uint8_t *buffer);
void gpio_set_input_change_handler(gpio_input_change_handler_t handler);

#endif
