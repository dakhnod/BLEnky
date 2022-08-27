#include "sensor_gpio.h"

bool ignore_interrupt = false;

void (*sensor_handler)(uint8_t);

bool contact_get(void)
{
  return ((nrf_gpio_pin_read(PIN_IN)) ^ BUTTONS_ACTIVE_STATE);
}

void set_led(bool on)
{
#ifdef USE_LEDS
  if (on)
  {
    bsp_board_led_on(0);
  }
  else
  {
    bsp_board_led_off(0);
  }
#endif
}

void pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  if (ignore_interrupt)
  {
    return;
  }
  if (pin != PIN_IN)
  {
    return;
  }
  if (!sensor_handler)
  {
    return;
  }

  // contact = contact_get();
  uint8_t contact = ((action == NRF_GPIOTE_POLARITY_LOTOHI) ^ BUTTONS_ACTIVE_STATE);
  NRF_LOG_DEBUG("contact: %i   action: %i\n", contact, action);

  sensor_handler (contact);
  disable_input();
}

void gpio_init(void (*sensor_handler_)(uint8_t))
{
  ret_code_t err_code;

  err_code = nrf_drv_gpiote_init();
  APP_ERROR_CHECK(err_code);

#ifdef USE_LEDS
  bsp_board_leds_init();
#endif

  nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
  in_config.pull = BUTTON_PULL;
  nrf_drv_gpiote_in_init(PIN_IN, &in_config, pin_handler);
  ignore_interrupt = true;
  nrf_drv_gpiote_in_event_enable(PIN_IN, true);
  ignore_interrupt = false;

  sensor_handler = sensor_handler_;
}

void enable_input()
{
  ignore_interrupt = false;
}

void disable_input()
{
  ignore_interrupt = true;
}
