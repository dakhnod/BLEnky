#ifndef FEATURE_CONFIG_H
#define FEATURE_CONFIG_H

#define APP_TIMER_PRESCALER 0

#define FEATURE_ENABLED(feature) FEATURE_##feature##_ENABLED == 1

// <<< Use Configuration Wizard in Context Menu >>>

// <h>GPIO configuration

// <o>Max gpio input count <0-32> <f.d>
#ifndef GPIO_INPUT_COUNT_MAX
#define GPIO_INPUT_COUNT_MAX 8
#endif

// <o>Max gpio output count <0-32> <f.d>
#ifndef GPIO_OUTPUT_COUNT_MAX
#define GPIO_OUTPUT_COUNT_MAX 8
#endif

// <o>Input debounce timeout in ms <0-1000> <f.d>
#ifndef GPIO_DEBOUNCE_TIMEOUT_MS
#define GPIO_DEBOUNCE_TIMEOUT_MS 100
#endif

// <h>Pin configurations

#define GPIO_CONFIGURATION_PIN_MODE_DISABLED 0
#define GPIO_CONFIGURATION_PIN_MODE_INPUT 1
#define GPIO_CONFIGURATION_PIN_MODE_OUTPUT 2



// <h>Pin 0 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_0_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_0_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_0_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_0_INVERT 0
// </e>

// </h>



// <h>Pin 1 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_1_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_1_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_1_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_1_INVERT 0
// </e>

// </h>



// <h>Pin 2 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_2_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_2_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_2_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_2_INVERT 0
// </e>

// </h>



// <h>Pin 3 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_3_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_3_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_3_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_3_INVERT 0
// </e>

// </h>



// <h>Pin 4 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_4_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_4_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_4_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_4_INVERT 0
// </e>

// </h>



// <h>Pin 5 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_5_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_5_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_5_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_5_INVERT 0
// </e>

// </h>



// <h>Pin 6 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_6_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_6_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_6_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_6_INVERT 0
// </e>

// </h>



// <h>Pin 7 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_7_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_7_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_7_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_7_INVERT 0
// </e>

// </h>



// <h>Pin 8 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_8_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_8_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_8_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_8_INVERT 0
// </e>

// </h>



// <h>Pin 9 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_9_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_9_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_9_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_9_INVERT 0
// </e>

// </h>



// <h>Pin 10 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_10_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_10_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_10_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_10_INVERT 0
// </e>

// </h>



// <h>Pin 11 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_11_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_11_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_11_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_11_INVERT 0
// </e>

// </h>



// <h>Pin 12 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_12_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_12_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_12_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_12_INVERT 0
// </e>

// </h>



// <h>Pin 13 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_13_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_13_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_13_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_13_INVERT 0
// </e>

// </h>



// <h>Pin 14 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_14_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_14_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_14_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_14_INVERT 0
// </e>

// </h>



// <h>Pin 15 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_15_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_15_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_15_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_15_INVERT 0
// </e>

// </h>



// <h>Pin 16 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_16_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_16_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_16_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_16_INVERT 0
// </e>

// </h>



// <h>Pin 17 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_17_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_17_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_17_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_17_INVERT 0
// </e>

// </h>



// <h>Pin 18 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_18_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_18_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_18_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_18_INVERT 0
// </e>

// </h>



// <h>Pin 19 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_19_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_19_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_19_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_19_INVERT 0
// </e>

// </h>



// <h>Pin 20 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_20_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_20_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_20_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_20_INVERT 0
// </e>

// </h>



// <h>Pin 21 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_21_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_21_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_21_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_21_INVERT 0
// </e>

// </h>



// <h>Pin 22 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_22_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_22_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_22_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_22_INVERT 0
// </e>

// </h>



// <h>Pin 23 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_23_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_23_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_23_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_23_INVERT 0
// </e>

// </h>



// <h>Pin 24 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_24_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_24_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_24_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_24_INVERT 0
// </e>

// </h>



// <h>Pin 25 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_25_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_25_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_25_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_25_INVERT 0
// </e>

// </h>



// <h>Pin 26 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_26_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_26_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_26_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_26_INVERT 0
// </e>

// </h>



// <h>Pin 27 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_27_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_27_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_27_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_27_INVERT 0
// </e>

// </h>



// <h>Pin 28 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_28_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_28_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_28_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_28_INVERT 0
// </e>

// </h>



// <h>Pin 29 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_29_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_29_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_29_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_29_INVERT 0
// </e>

// </h>



// <h>Pin 30 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_30_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_30_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_30_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_30_INVERT 0
// </e>

// </h>



// <h>Pin 31 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#define GPIO_CONFIGURATION_PIN_31_MODE GPIO_CONFIGURATION_PIN_MODE_DISABLED

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#define GPIO_CONFIGURATION_PIN_31_DEFAULT_OUTPUT 0

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#define GPIO_CONFIGURATION_PIN_31_PULL 0

// <e>Invert input / output
#define GPIO_CONFIGURATION_PIN_31_INVERT 0
// </e>

// </h>



// </h>

// </h>

// <h>BLE Configuration

// <o>Fast advertisement interval (ms) <10-1000>
#ifndef ADVERTISEMENT_INTERVAL_FAST
#define ADVERTISEMENT_INTERVAL_FAST 100
#endif

// <o>Fast advertisement duration (s) <0-60>
#ifndef ADVERTISEMENT_TIMEOUT_FAST
#define ADVERTISEMENT_TIMEOUT_FAST 10
#endif

// <o>Slow advertisement interval (ms) <500-5000>
// <i>Set to 0 for infinite
#ifndef ADVERTISEMENT_INTERVAL_SLOW
#define ADVERTISEMENT_INTERVAL_SLOW 1000
#endif

// <o>Slow advertisement duration (s) <0-1440>
#ifndef ADVERTISEMENT_TIMEOUT_SLOW
#define ADVERTISEMENT_TIMEOUT_SLOW 0
#endif

// <s.13>Default device name
#ifndef DEVICE_NAME
#define DEVICE_NAME "BLE400"
#endif 

// <o>Minimal connection interval (ms) <8-4000>
#ifndef BLE_MIN_CONN_INTERVAL
#define BLE_MIN_CONN_INTERVAL 10
#endif

// <o>Maximal connection interval (ms) <8-4000>
#ifndef BLE_MAX_CONN_INTERVAL
#define BLE_MAX_CONN_INTERVAL 20
#endif

// <o>Slave latency (connection event count) <0-100>
#ifndef BLE_SLAVE_LATENCY
#define BLE_SLAVE_LATENCY 100
#endif

// <o>Connection supervision timeout (ms) <100-32000>
#ifndef BLE_CONN_SUP_TIMEOUT
#define BLE_CONN_SUP_TIMEOUT 5000
#endif

// <e>BLE bonding enabled
#ifndef FEATURE_BLE_BONDING_ENABLED
#define FEATURE_BLE_BONDING_ENABLED 1
#endif

#if FEATURE_ENABLED(BLE_BONDING)
// <e>Static passkey enabled
#ifndef STATIC_PASSKEY_ENABLED
#define STATIC_PASSKEY_ENABLED 0
#endif
#if STATIC_PASSKEY_ENABLED == 1
// <s.6>Static passkey (6 digits)
#ifndef BLE_BONDIG_PASSKEY
#define BLE_BONDIG_PASSKEY "123456"
#endif
#endif
// </e>
#endif

// </e>

// </h>

// <h>Features configuration

// <e>Battery status profile enabled
#ifndef FEATURE_BATTERY_PROFILE_ENABLED
#define FEATURE_BATTERY_PROFILE_ENABLED 1
#endif
// </e>

// <e>Binary sensor profile enabled
#ifndef FEATURE_BINARY_SENSOR_ENABLED
#define FEATURE_BINARY_SENSOR_ENABLED 0
#endif

// <o> Binary sensor pin <0-32> <f.d>
#ifndef BINARY_SENSOR_PIN
#define BINARY_SENSOR_PIN 0
#endif
// </e>

// <e>Automation IO profile enabled
#ifndef FEATURE_AUTOMATION_IO_ENABLED
#define FEATURE_AUTOMATION_IO_ENABLED 1
#endif
// </e>

// <e>gpioASM profile and engine enabled
#ifndef FEATURE_GPIO_ASM_ENABLED
#define FEATURE_GPIO_ASM_ENABLED 0
#endif
// </e>

// <e>Cycling speed/cadence profile and engine enabled
#ifndef FEATURE_CYCLING_SPEED_CADENCE_ENABLED
#define FEATURE_CYCLING_SPEED_CADENCE_ENABLED 0
#endif

// <o> Wheel sensor pin <0-32> <f.d>
#ifndef CSC_WHEEL_SENSOR_PIN
#define CSC_WHEEL_SENSOR_PIN 0
#endif
// </e>

// <e>HID service enabled (experimental)
#ifndef FEATURE_HID_ENABLED
#define FEATURE_HID_ENABLED 1
#endif

#if FEATURE_ENABLED(HID)
#if ! FEATURE_ENABLED(BLE_BONDING)
#error BLE bonding needs to be enabled for HID to work
#endif
#endif

// <e> Gamepad enabled
#ifndef HID_GAMEPAD_ENABLED
#define HID_GAMEPAD_ENABLED 1
#endif


// <e> D-Pad up enabled
#ifndef HID_D_PAD_UP_ENABLED
#define HID_D_PAD_UP_ENABLED 0
#endif
// <o> D-Pad up pin <0-32> <f.d>
#ifndef HID_D_PAD_UP_PIN
#define HID_D_PAD_UP_PIN 0
#endif
// </e>

// <e> D-Pad down enabled
#ifndef HID_D_PAD_DOWN_ENABLED
#define HID_D_PAD_DOWN_ENABLED 0
#endif
// <o> D-Pad down pin <0-32> <f.d>
#ifndef HID_D_PAD_DOWN_PIN
#define HID_D_PAD_DOWN_PIN 0
#endif
// </e>

// <e> D-Pad left enabled
#ifndef HID_D_PAD_LEFT_ENABLED
#define HID_D_PAD_LEFT_ENABLED 0
#endif
// <o> D-Pad left pin <0-32> <f.d>
#ifndef HID_D_PAD_LEFT_PIN
#define HID_D_PAD_LEFT_PIN 0
#endif
// </e>

// <e> D-Pad right enabled
#ifndef HID_D_PAD_RIGHT_ENABLED
#define HID_D_PAD_RIGHT_ENABLED 0
#endif
// <o> D-Pad right pin <0-32> <f.d>
#ifndef HID_D_PAD_RIGHT_PIN
#define HID_D_PAD_RIGHT_PIN 0
#endif
// </e>



// <e> A button enabled
#ifndef HID_BUTTON_A_ENABLED
#define HID_BUTTON_A_ENABLED 0
#endif
// <o> A button pin <0-32> <f.d>
#ifndef HID_BUTTON_A_PIN
#define HID_BUTTON_A_PIN 0
#endif
// </e>

// <e> B button enabled
#ifndef HID_BUTTON_B_ENABLED
#define HID_BUTTON_B_ENABLED 0
#endif
// <o> B button pin <0-32> <f.d>
#ifndef HID_BUTTON_B_PIN
#define HID_BUTTON_B_PIN 0
#endif
// </e>

// <e> Start button enabled
#ifndef HID_BUTTON_START_ENABLED
#define HID_BUTTON_START_ENABLED 0
#endif
// <o> Start button pin <0-32> <f.d>
#ifndef HID_BUTTON_START_PIN
#define HID_BUTTON_START_PIN 0
#endif
// </e>

// <e> Select button enabled
#ifndef HID_BUTTON_SELECT_ENABLED
#define HID_BUTTON_SELECT_ENABLED 0
#endif
// <o> Select button pin <0-32> <f.d>
#ifndef HID_BUTTON_SELECT_PIN
#define HID_BUTTON_SELECT_PIN 0
#endif
// </e>


// </e>

// </e>

// <e>Sleep mode enabled
#ifndef FEATURE_SLEEP_MODE_ENABLED
#define FEATURE_SLEEP_MODE_ENABLED 1
#endif

#if FEATURE_ENABLED(SLEEP_MODE)
//<o>Sleep mode
//<0=> Light sleep (RAM retention)
//<1=> Deep sleep (no RAM retention)
#define SLEEP_MODE 0x1

//<o>Sleep timeout in minutes <0-180>
#define SLEEP_TIMEOUT_MINUTES 5
#endif

//<e>Go to light sleep after graceful disconnect
#ifndef SLEEP_AFTER_DISCONNECT
#define SLEEP_AFTER_DISCONNECT 1
#endif
//</e>

// </e>

// </h>

// <<< end of configuration section >>>
#endif