#ifndef FEATURE_CONFIG_H
#define FEATURE_CONFIG_H

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

// <h>Features configuration

// <e>Battery status profile enabled
#ifndef FEATURE_BATTERY_PROFILE_ENABLED
#define FEATURE_BATTERY_PROFILE_ENABLED 1
#endif
// </e>

// <e>Binary sensor profile enabled
#ifndef FEATURE_BINARY_SENSOR_ENABLED
#define FEATURE_BINARY_SENSOR_ENABLED 1
#endif
// </e>

// <e>Automation IO profile enabled
#ifndef FEATURE_AUTOMATION_IO_ENABLED
#define FEATURE_AUTOMATION_IO_ENABLED 1
#endif
// </e>

// <e>gpioASM profile and engine enabled
#ifndef FEATURE_GPIO_ASM_ENABLED
#define FEATURE_GPIO_ASM_ENABLED 1
#endif
// </e>

// <e>Cycling speed/cadence profile and engine enabled
#ifndef FEATURE_CYCLING_SPEED_CADENCE_ENABLED
#define FEATURE_CYCLING_SPEED_CADENCE_ENABLED 1
#endif
// </e>

// <e>HID service enabled (experimental)
#ifndef FEATURE_HID_ENABLED
#define FEATURE_HID_ENABLED 1
#endif
// </e>

// <e>Sleep mode enabled
#ifndef FEATURE_SLEEP_MODE_ENABLED
#define FEATURE_SLEEP_MODE_ENABLED 1
#endif

//<o> Sleep mode
//<0=> Light sleep (RAM retention)
//<1=> Deep sleep (no RAM retention)
#define SLEEP_MODE 0x00
//</o>

//<o>Sleep timeout in minutes <1-8>
#define SLEEP_TIMEOUT_MINUTES 5
//</o>

//</e>

// </h>

// <<< end of configuration section >>>
#endif