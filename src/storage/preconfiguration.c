#include <stdint.h>
#include <stdbool.h>
#include <feature_config.h>
#include <nrf_log.h>

/*
This is awful.
I am sorry for whoever needs to read and understand this.

If it's you Daniel, f you. You are responsible for this.
*/

#define GPIO_CONFIGURATION_CHECK(pin_index, data) \
do{ \
    uint8_t bits = 0b1111; \
    if(GPIO_CONFIGURATION_PIN_##pin_index##_MODE != GPIO_CONFIGURATION_PIN_MODE_DISABLED){ \
        bits = ((GPIO_CONFIGURATION_PIN_##pin_index##_MODE == GPIO_CONFIGURATION_PIN_MODE_INPUT) << 3) \
             | ((GPIO_CONFIGURATION_PIN_##pin_index##_INVERT)); \
        \
        if(GPIO_CONFIGURATION_PIN_##pin_index##_MODE == GPIO_CONFIGURATION_PIN_MODE_INPUT) { \
            bits |= (GPIO_CONFIGURATION_PIN_##pin_index##_PULL << 1); \
        }else{ \
            bits |= (GPIO_CONFIGURATION_PIN_##pin_index##_DEFAULT_OUTPUT << 1); \
        } \
        \
        uint8_t *pin_bits = data + ((HARDWARE_PIN_COUNT - pin_index - 1) / 2); \
        if((pin_index & 1) == 0) { \
            (*pin_bits) &= 0b11110000; \
            (*pin_bits) |= bits; \
        }else{ \
            (*pin_bits) &= 0b00001111; \
            (*pin_bits) |= (bits << 4); \
        } \
    } \
}while (false)

void preconfiguration_load(uint8_t *data) {
    GPIO_CONFIGURATION_CHECK(0, data);
    GPIO_CONFIGURATION_CHECK(1, data);
    GPIO_CONFIGURATION_CHECK(2, data);
    GPIO_CONFIGURATION_CHECK(3, data);
    GPIO_CONFIGURATION_CHECK(4, data);
    GPIO_CONFIGURATION_CHECK(5, data);
    GPIO_CONFIGURATION_CHECK(6, data);
    GPIO_CONFIGURATION_CHECK(7, data);
    GPIO_CONFIGURATION_CHECK(8, data);
    GPIO_CONFIGURATION_CHECK(9, data);
    GPIO_CONFIGURATION_CHECK(10, data);
    GPIO_CONFIGURATION_CHECK(11, data);
    GPIO_CONFIGURATION_CHECK(12, data);
    GPIO_CONFIGURATION_CHECK(13, data);
    GPIO_CONFIGURATION_CHECK(14, data);
    GPIO_CONFIGURATION_CHECK(15, data);
    GPIO_CONFIGURATION_CHECK(16, data);
    GPIO_CONFIGURATION_CHECK(17, data);
    GPIO_CONFIGURATION_CHECK(18, data);
    GPIO_CONFIGURATION_CHECK(19, data);
    GPIO_CONFIGURATION_CHECK(20, data);
    GPIO_CONFIGURATION_CHECK(21, data);
    GPIO_CONFIGURATION_CHECK(22, data);
    GPIO_CONFIGURATION_CHECK(23, data);
    GPIO_CONFIGURATION_CHECK(24, data);
    GPIO_CONFIGURATION_CHECK(25, data);
    GPIO_CONFIGURATION_CHECK(26, data);
    GPIO_CONFIGURATION_CHECK(27, data);
    GPIO_CONFIGURATION_CHECK(28, data);
    GPIO_CONFIGURATION_CHECK(29, data);
    GPIO_CONFIGURATION_CHECK(30, data);
    GPIO_CONFIGURATION_CHECK(31, data);
    #if HARDWARE_PIN_COUNT == 64
    GPIO_CONFIGURATION_CHECK(32, data);
    GPIO_CONFIGURATION_CHECK(33, data);
    GPIO_CONFIGURATION_CHECK(34, data);
    GPIO_CONFIGURATION_CHECK(35, data);
    GPIO_CONFIGURATION_CHECK(36, data);
    GPIO_CONFIGURATION_CHECK(37, data);
    GPIO_CONFIGURATION_CHECK(38, data);
    GPIO_CONFIGURATION_CHECK(39, data);
    GPIO_CONFIGURATION_CHECK(40, data);
    GPIO_CONFIGURATION_CHECK(41, data);
    GPIO_CONFIGURATION_CHECK(42, data);
    GPIO_CONFIGURATION_CHECK(43, data);
    GPIO_CONFIGURATION_CHECK(44, data);
    GPIO_CONFIGURATION_CHECK(45, data);
    GPIO_CONFIGURATION_CHECK(46, data);
    GPIO_CONFIGURATION_CHECK(47, data);
    GPIO_CONFIGURATION_CHECK(48, data);
    GPIO_CONFIGURATION_CHECK(49, data);
    GPIO_CONFIGURATION_CHECK(50, data);
    GPIO_CONFIGURATION_CHECK(51, data);
    GPIO_CONFIGURATION_CHECK(52, data);
    GPIO_CONFIGURATION_CHECK(53, data);
    GPIO_CONFIGURATION_CHECK(54, data);
    GPIO_CONFIGURATION_CHECK(55, data);
    GPIO_CONFIGURATION_CHECK(56, data);
    GPIO_CONFIGURATION_CHECK(57, data);
    GPIO_CONFIGURATION_CHECK(58, data);
    GPIO_CONFIGURATION_CHECK(59, data);
    GPIO_CONFIGURATION_CHECK(60, data);
    GPIO_CONFIGURATION_CHECK(61, data);
    GPIO_CONFIGURATION_CHECK(62, data);
    GPIO_CONFIGURATION_CHECK(63, data);
    #endif
}