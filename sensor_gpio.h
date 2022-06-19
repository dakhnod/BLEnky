#ifndef SENSOR_GPIO_H
#define SENSOR_GPIO_H

#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
#include "boards.h"
#include "bsp.h"

#define BATTERY_VOLTAGE_MAX 3000.0
#define BATTERY_VOLTAGE_MIN 1700.0

#define INVERT_INPUT 1

// #define PIN_IN 9 // for key sensor
// #define PIN_OUT 18 // for key sensor

#define PIN_IN BUTTON_0
#define PIN_OUT LED_0


#define USE_LEDS

bool contact_get(void);
void pin_handler(nrf_drv_gpiote_pin_t, nrf_gpiote_polarity_t);
void gpio_init(void (*sensor_handler)(uint8_t));
uint8_t battery_level_get(void);
void set_led(bool on);
void enable_input(void);
void disable_input(void);

#endif
