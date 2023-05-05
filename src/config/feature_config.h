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
#define HID_D_PAD_UP_ENABLED 1
#endif
// <o> D-Pad up pin <0-32> <f.d>
#ifndef HID_D_PAD_UP_PIN
#define HID_D_PAD_UP_PIN 2
#endif
// </e>

// <e> D-Pad down enabled
#ifndef HID_D_PAD_DOWN_ENABLED
#define HID_D_PAD_DOWN_ENABLED 1
#endif
// <o> D-Pad down pin <0-32> <f.d>
#ifndef HID_D_PAD_DOWN_PIN
#define HID_D_PAD_DOWN_PIN 4
#endif
// </e>

// <e> D-Pad left enabled
#ifndef HID_D_PAD_LEFT_ENABLED
#define HID_D_PAD_LEFT_ENABLED 1
#endif
// <o> D-Pad left pin <0-32> <f.d>
#ifndef HID_D_PAD_LEFT_PIN
#define HID_D_PAD_LEFT_PIN 1
#endif
// </e>

// <e> D-Pad right enabled
#ifndef HID_D_PAD_RIGHT_ENABLED
#define HID_D_PAD_RIGHT_ENABLED 1
#endif
// <o> D-Pad right pin <0-32> <f.d>
#ifndef HID_D_PAD_RIGHT_PIN
#define HID_D_PAD_RIGHT_PIN 3
#endif
// </e>

// <e> A button enabled
#ifndef HID_BUTTON_A_ENABLED
#define HID_BUTTON_A_ENABLED 0
#endif
// <o> A button pin <0-32> <f.d>
#ifndef HID_BUTTON_A_PIN
#define HID_BUTTON_A_PIN 16
#endif
// </e>

// <e> B button enabled
#ifndef HID_BUTTON_B_ENABLED
#define HID_BUTTON_B_ENABLED 0
#endif
// <o> B button pin <0-32> <f.d>
#ifndef HID_BUTTON_B_PIN
#define HID_BUTTON_B_PIN 17
#endif
// </e>

// <e> Start button enabled
#ifndef HID_BUTTON_START_ENABLED
#define HID_BUTTON_START_ENABLED 1
#endif
// <o> Start button pin <0-32> <f.d>
#ifndef HID_BUTTON_START_PIN
#define HID_BUTTON_START_PIN 16
#endif
// </e>

// <e> Select button enabled
#ifndef HID_BUTTON_SELECT_ENABLED
#define HID_BUTTON_SELECT_ENABLED 1
#endif
// <o> Select button pin <0-32> <f.d>
#ifndef HID_BUTTON_SELECT_PIN
#define HID_BUTTON_SELECT_PIN 17
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

// </e>

// </h>

// <<< end of configuration section >>>
#endif