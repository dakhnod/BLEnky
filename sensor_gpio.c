#include "sensor_gpio.h"

uint8_t gpio_sensor_states[OUTPUTS_NUMBER];

#if OUTPUTS_NUMBER > 0
void gpio_write_output_pin(uint32_t index, uint8_t value){
    uint32_t pin = OUTPUTS_PINS[index];
    if(value ^ OUTPUTS_INVERT[index]){
      nrf_gpio_pin_set(pin);
    }else{
      nrf_gpio_pin_clear(pin);
    }
    gpio_sensor_states[index] = value;
}

uint32_t gpio_get_output_pin_count(){
  return OUTPUTS_NUMBER;
}

uint8_t gpio_get_output_state(uint32_t index){
  return gpio_sensor_states[index];
}

uint8_t *gpio_get_output_states(){
  return gpio_sensor_states;
}

void gpio_configure_aio_outputs(){
  for(int i = 0; i < OUTPUTS_NUMBER; i++){
    nrf_gpio_cfg_output(OUTPUTS_PINS[i]);
    gpio_write_output_pin(i, OUTPUTS_DEFAULT_STATE[i]);
  };
}
#else
void gpio_configure_aio_outputs(){};
#endif

void gpio_init(void (*sensor_handler_)(uint8_t))
{
  // ret_code_t err_code;

  gpio_configure_aio_outputs();
}