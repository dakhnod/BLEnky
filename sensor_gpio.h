#ifndef SENSOR_GPIO_H
#define SENSOR_GPIO_H

#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
#include "boards.h"
#include "bsp.h"
#include "config/pin_configuration.h"

#define BATTERY_VOLTAGE_MAX 3000.0
#define BATTERY_VOLTAGE_MIN 1700.0

void gpio_init(void (*sensor_handler)(uint8_t));

void gpio_configure_aio_outputs();
void gpio_write_output_pin(uint32_t index, uint8_t value);
uint32_t gpio_get_output_pin_count();
uint8_t gpio_get_output_state(uint32_t index);
uint8_t *gpio_get_output_states();

#endif
