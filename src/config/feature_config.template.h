#ifndef FEATURE_CONFIG_H
#define FEATURE_CONFIG_H

#define APP_TIMER_PRESCALER 0

#ifdef BLENKY_BSP_FILE
#include BLENKY_BSP_FILE
#endif

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
#ifndef GPIO_CONFIGURATION_PIN_0_MODE
#define GPIO_CONFIGURATION_PIN_0_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_0_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_0_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_0_PULL
#define GPIO_CONFIGURATION_PIN_0_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_0_INVERT
#define GPIO_CONFIGURATION_PIN_0_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 1 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_1_MODE
#define GPIO_CONFIGURATION_PIN_1_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_1_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_1_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_1_PULL
#define GPIO_CONFIGURATION_PIN_1_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_1_INVERT
#define GPIO_CONFIGURATION_PIN_1_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 2 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_2_MODE
#define GPIO_CONFIGURATION_PIN_2_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_2_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_2_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_2_PULL
#define GPIO_CONFIGURATION_PIN_2_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_2_INVERT
#define GPIO_CONFIGURATION_PIN_2_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 3 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_3_MODE
#define GPIO_CONFIGURATION_PIN_3_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_3_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_3_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_3_PULL
#define GPIO_CONFIGURATION_PIN_3_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_3_INVERT
#define GPIO_CONFIGURATION_PIN_3_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 4 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_4_MODE
#define GPIO_CONFIGURATION_PIN_4_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_4_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_4_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_4_PULL
#define GPIO_CONFIGURATION_PIN_4_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_4_INVERT
#define GPIO_CONFIGURATION_PIN_4_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 5 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_5_MODE
#define GPIO_CONFIGURATION_PIN_5_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_5_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_5_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_5_PULL
#define GPIO_CONFIGURATION_PIN_5_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_5_INVERT
#define GPIO_CONFIGURATION_PIN_5_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 6 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_6_MODE
#define GPIO_CONFIGURATION_PIN_6_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_6_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_6_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_6_PULL
#define GPIO_CONFIGURATION_PIN_6_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_6_INVERT
#define GPIO_CONFIGURATION_PIN_6_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 7 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_7_MODE
#define GPIO_CONFIGURATION_PIN_7_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_7_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_7_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_7_PULL
#define GPIO_CONFIGURATION_PIN_7_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_7_INVERT
#define GPIO_CONFIGURATION_PIN_7_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 8 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_8_MODE
#define GPIO_CONFIGURATION_PIN_8_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_8_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_8_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_8_PULL
#define GPIO_CONFIGURATION_PIN_8_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_8_INVERT
#define GPIO_CONFIGURATION_PIN_8_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 9 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_9_MODE
#define GPIO_CONFIGURATION_PIN_9_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_9_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_9_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_9_PULL
#define GPIO_CONFIGURATION_PIN_9_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_9_INVERT
#define GPIO_CONFIGURATION_PIN_9_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 10 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_10_MODE
#define GPIO_CONFIGURATION_PIN_10_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_10_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_10_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_10_PULL
#define GPIO_CONFIGURATION_PIN_10_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_10_INVERT
#define GPIO_CONFIGURATION_PIN_10_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 11 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_11_MODE
#define GPIO_CONFIGURATION_PIN_11_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_11_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_11_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_11_PULL
#define GPIO_CONFIGURATION_PIN_11_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_11_INVERT
#define GPIO_CONFIGURATION_PIN_11_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 12 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_12_MODE
#define GPIO_CONFIGURATION_PIN_12_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_12_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_12_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_12_PULL
#define GPIO_CONFIGURATION_PIN_12_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_12_INVERT
#define GPIO_CONFIGURATION_PIN_12_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 13 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_13_MODE
#define GPIO_CONFIGURATION_PIN_13_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_13_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_13_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_13_PULL
#define GPIO_CONFIGURATION_PIN_13_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_13_INVERT
#define GPIO_CONFIGURATION_PIN_13_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 14 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_14_MODE
#define GPIO_CONFIGURATION_PIN_14_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_14_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_14_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_14_PULL
#define GPIO_CONFIGURATION_PIN_14_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_14_INVERT
#define GPIO_CONFIGURATION_PIN_14_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 15 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_15_MODE
#define GPIO_CONFIGURATION_PIN_15_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_15_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_15_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_15_PULL
#define GPIO_CONFIGURATION_PIN_15_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_15_INVERT
#define GPIO_CONFIGURATION_PIN_15_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 16 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_16_MODE
#define GPIO_CONFIGURATION_PIN_16_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_16_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_16_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_16_PULL
#define GPIO_CONFIGURATION_PIN_16_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_16_INVERT
#define GPIO_CONFIGURATION_PIN_16_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 17 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_17_MODE
#define GPIO_CONFIGURATION_PIN_17_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_17_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_17_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_17_PULL
#define GPIO_CONFIGURATION_PIN_17_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_17_INVERT
#define GPIO_CONFIGURATION_PIN_17_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 18 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_18_MODE
#define GPIO_CONFIGURATION_PIN_18_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_18_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_18_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_18_PULL
#define GPIO_CONFIGURATION_PIN_18_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_18_INVERT
#define GPIO_CONFIGURATION_PIN_18_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 19 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_19_MODE
#define GPIO_CONFIGURATION_PIN_19_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_19_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_19_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_19_PULL
#define GPIO_CONFIGURATION_PIN_19_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_19_INVERT
#define GPIO_CONFIGURATION_PIN_19_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 20 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_20_MODE
#define GPIO_CONFIGURATION_PIN_20_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_20_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_20_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_20_PULL
#define GPIO_CONFIGURATION_PIN_20_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_20_INVERT
#define GPIO_CONFIGURATION_PIN_20_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 21 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_21_MODE
#define GPIO_CONFIGURATION_PIN_21_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_21_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_21_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_21_PULL
#define GPIO_CONFIGURATION_PIN_21_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_21_INVERT
#define GPIO_CONFIGURATION_PIN_21_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 22 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_22_MODE
#define GPIO_CONFIGURATION_PIN_22_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_22_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_22_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_22_PULL
#define GPIO_CONFIGURATION_PIN_22_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_22_INVERT
#define GPIO_CONFIGURATION_PIN_22_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 23 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_23_MODE
#define GPIO_CONFIGURATION_PIN_23_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_23_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_23_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_23_PULL
#define GPIO_CONFIGURATION_PIN_23_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_23_INVERT
#define GPIO_CONFIGURATION_PIN_23_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 24 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_24_MODE
#define GPIO_CONFIGURATION_PIN_24_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_24_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_24_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_24_PULL
#define GPIO_CONFIGURATION_PIN_24_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_24_INVERT
#define GPIO_CONFIGURATION_PIN_24_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 25 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_25_MODE
#define GPIO_CONFIGURATION_PIN_25_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_25_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_25_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_25_PULL
#define GPIO_CONFIGURATION_PIN_25_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_25_INVERT
#define GPIO_CONFIGURATION_PIN_25_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 26 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_26_MODE
#define GPIO_CONFIGURATION_PIN_26_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_26_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_26_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_26_PULL
#define GPIO_CONFIGURATION_PIN_26_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_26_INVERT
#define GPIO_CONFIGURATION_PIN_26_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 27 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_27_MODE
#define GPIO_CONFIGURATION_PIN_27_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_27_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_27_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_27_PULL
#define GPIO_CONFIGURATION_PIN_27_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_27_INVERT
#define GPIO_CONFIGURATION_PIN_27_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 28 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_28_MODE
#define GPIO_CONFIGURATION_PIN_28_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_28_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_28_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_28_PULL
#define GPIO_CONFIGURATION_PIN_28_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_28_INVERT
#define GPIO_CONFIGURATION_PIN_28_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 29 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_29_MODE
#define GPIO_CONFIGURATION_PIN_29_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_29_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_29_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_29_PULL
#define GPIO_CONFIGURATION_PIN_29_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_29_INVERT
#define GPIO_CONFIGURATION_PIN_29_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 30 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_30_MODE
#define GPIO_CONFIGURATION_PIN_30_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_30_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_30_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_30_PULL
#define GPIO_CONFIGURATION_PIN_30_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_30_INVERT
#define GPIO_CONFIGURATION_PIN_30_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 31 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_31_MODE
#define GPIO_CONFIGURATION_PIN_31_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_31_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_31_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_31_PULL
#define GPIO_CONFIGURATION_PIN_31_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_31_INVERT
#define GPIO_CONFIGURATION_PIN_31_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 32 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_32_MODE
#define GPIO_CONFIGURATION_PIN_32_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_32_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_32_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_32_PULL
#define GPIO_CONFIGURATION_PIN_32_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_32_INVERT
#define GPIO_CONFIGURATION_PIN_32_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 33 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_33_MODE
#define GPIO_CONFIGURATION_PIN_33_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_33_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_33_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_33_PULL
#define GPIO_CONFIGURATION_PIN_33_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_33_INVERT
#define GPIO_CONFIGURATION_PIN_33_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 34 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_34_MODE
#define GPIO_CONFIGURATION_PIN_34_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_34_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_34_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_34_PULL
#define GPIO_CONFIGURATION_PIN_34_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_34_INVERT
#define GPIO_CONFIGURATION_PIN_34_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 35 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_35_MODE
#define GPIO_CONFIGURATION_PIN_35_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_35_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_35_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_35_PULL
#define GPIO_CONFIGURATION_PIN_35_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_35_INVERT
#define GPIO_CONFIGURATION_PIN_35_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 36 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_36_MODE
#define GPIO_CONFIGURATION_PIN_36_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_36_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_36_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_36_PULL
#define GPIO_CONFIGURATION_PIN_36_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_36_INVERT
#define GPIO_CONFIGURATION_PIN_36_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 37 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_37_MODE
#define GPIO_CONFIGURATION_PIN_37_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_37_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_37_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_37_PULL
#define GPIO_CONFIGURATION_PIN_37_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_37_INVERT
#define GPIO_CONFIGURATION_PIN_37_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 38 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_38_MODE
#define GPIO_CONFIGURATION_PIN_38_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_38_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_38_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_38_PULL
#define GPIO_CONFIGURATION_PIN_38_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_38_INVERT
#define GPIO_CONFIGURATION_PIN_38_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 39 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_39_MODE
#define GPIO_CONFIGURATION_PIN_39_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_39_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_39_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_39_PULL
#define GPIO_CONFIGURATION_PIN_39_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_39_INVERT
#define GPIO_CONFIGURATION_PIN_39_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 40 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_40_MODE
#define GPIO_CONFIGURATION_PIN_40_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_40_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_40_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_40_PULL
#define GPIO_CONFIGURATION_PIN_40_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_40_INVERT
#define GPIO_CONFIGURATION_PIN_40_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 41 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_41_MODE
#define GPIO_CONFIGURATION_PIN_41_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_41_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_41_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_41_PULL
#define GPIO_CONFIGURATION_PIN_41_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_41_INVERT
#define GPIO_CONFIGURATION_PIN_41_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 42 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_42_MODE
#define GPIO_CONFIGURATION_PIN_42_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_42_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_42_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_42_PULL
#define GPIO_CONFIGURATION_PIN_42_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_42_INVERT
#define GPIO_CONFIGURATION_PIN_42_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 43 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_43_MODE
#define GPIO_CONFIGURATION_PIN_43_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_43_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_43_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_43_PULL
#define GPIO_CONFIGURATION_PIN_43_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_43_INVERT
#define GPIO_CONFIGURATION_PIN_43_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 44 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_44_MODE
#define GPIO_CONFIGURATION_PIN_44_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_44_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_44_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_44_PULL
#define GPIO_CONFIGURATION_PIN_44_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_44_INVERT
#define GPIO_CONFIGURATION_PIN_44_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 45 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_45_MODE
#define GPIO_CONFIGURATION_PIN_45_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_45_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_45_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_45_PULL
#define GPIO_CONFIGURATION_PIN_45_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_45_INVERT
#define GPIO_CONFIGURATION_PIN_45_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 46 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_46_MODE
#define GPIO_CONFIGURATION_PIN_46_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_46_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_46_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_46_PULL
#define GPIO_CONFIGURATION_PIN_46_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_46_INVERT
#define GPIO_CONFIGURATION_PIN_46_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 47 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_47_MODE
#define GPIO_CONFIGURATION_PIN_47_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_47_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_47_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_47_PULL
#define GPIO_CONFIGURATION_PIN_47_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_47_INVERT
#define GPIO_CONFIGURATION_PIN_47_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 48 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_48_MODE
#define GPIO_CONFIGURATION_PIN_48_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_48_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_48_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_48_PULL
#define GPIO_CONFIGURATION_PIN_48_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_48_INVERT
#define GPIO_CONFIGURATION_PIN_48_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 49 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_49_MODE
#define GPIO_CONFIGURATION_PIN_49_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_49_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_49_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_49_PULL
#define GPIO_CONFIGURATION_PIN_49_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_49_INVERT
#define GPIO_CONFIGURATION_PIN_49_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 50 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_50_MODE
#define GPIO_CONFIGURATION_PIN_50_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_50_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_50_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_50_PULL
#define GPIO_CONFIGURATION_PIN_50_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_50_INVERT
#define GPIO_CONFIGURATION_PIN_50_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 51 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_51_MODE
#define GPIO_CONFIGURATION_PIN_51_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_51_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_51_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_51_PULL
#define GPIO_CONFIGURATION_PIN_51_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_51_INVERT
#define GPIO_CONFIGURATION_PIN_51_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 52 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_52_MODE
#define GPIO_CONFIGURATION_PIN_52_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_52_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_52_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_52_PULL
#define GPIO_CONFIGURATION_PIN_52_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_52_INVERT
#define GPIO_CONFIGURATION_PIN_52_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 53 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_53_MODE
#define GPIO_CONFIGURATION_PIN_53_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_53_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_53_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_53_PULL
#define GPIO_CONFIGURATION_PIN_53_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_53_INVERT
#define GPIO_CONFIGURATION_PIN_53_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 54 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_54_MODE
#define GPIO_CONFIGURATION_PIN_54_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_54_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_54_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_54_PULL
#define GPIO_CONFIGURATION_PIN_54_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_54_INVERT
#define GPIO_CONFIGURATION_PIN_54_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 55 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_55_MODE
#define GPIO_CONFIGURATION_PIN_55_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_55_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_55_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_55_PULL
#define GPIO_CONFIGURATION_PIN_55_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_55_INVERT
#define GPIO_CONFIGURATION_PIN_55_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 56 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_56_MODE
#define GPIO_CONFIGURATION_PIN_56_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_56_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_56_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_56_PULL
#define GPIO_CONFIGURATION_PIN_56_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_56_INVERT
#define GPIO_CONFIGURATION_PIN_56_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 57 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_57_MODE
#define GPIO_CONFIGURATION_PIN_57_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_57_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_57_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_57_PULL
#define GPIO_CONFIGURATION_PIN_57_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_57_INVERT
#define GPIO_CONFIGURATION_PIN_57_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 58 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_58_MODE
#define GPIO_CONFIGURATION_PIN_58_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_58_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_58_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_58_PULL
#define GPIO_CONFIGURATION_PIN_58_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_58_INVERT
#define GPIO_CONFIGURATION_PIN_58_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 59 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_59_MODE
#define GPIO_CONFIGURATION_PIN_59_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_59_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_59_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_59_PULL
#define GPIO_CONFIGURATION_PIN_59_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_59_INVERT
#define GPIO_CONFIGURATION_PIN_59_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 60 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_60_MODE
#define GPIO_CONFIGURATION_PIN_60_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_60_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_60_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_60_PULL
#define GPIO_CONFIGURATION_PIN_60_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_60_INVERT
#define GPIO_CONFIGURATION_PIN_60_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 61 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_61_MODE
#define GPIO_CONFIGURATION_PIN_61_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_61_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_61_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_61_PULL
#define GPIO_CONFIGURATION_PIN_61_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_61_INVERT
#define GPIO_CONFIGURATION_PIN_61_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 62 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_62_MODE
#define GPIO_CONFIGURATION_PIN_62_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_62_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_62_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_62_PULL
#define GPIO_CONFIGURATION_PIN_62_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_62_INVERT
#define GPIO_CONFIGURATION_PIN_62_INVERT 0
#endif
// </e>

// </h>



// <h>Pin 63 configuration

// <o> Mode
// <0=> Disabled
// <1=> Input
// <2=> Output
#ifndef GPIO_CONFIGURATION_PIN_63_MODE
#define GPIO_CONFIGURATION_PIN_63_MODE 0
#endif

// <o> Default output (Only applies for output mode)
// <i> Only applies for output mode
// <0=> Low
// <1=> High
#ifndef GPIO_CONFIGURATION_PIN_63_DEFAULT_OUTPUT
#define GPIO_CONFIGURATION_PIN_63_DEFAULT_OUTPUT 0
#endif

// <o> Pull mode (Only applies for input mode)
// <i> Only applies for input mode
// <0=> Disabled
// <1=> Pullup
// <2=> Pulldown
#ifndef GPIO_CONFIGURATION_PIN_63_PULL
#define GPIO_CONFIGURATION_PIN_63_PULL 0
#endif

// <e>Invert input / output
#ifndef GPIO_CONFIGURATION_PIN_63_INVERT
#define GPIO_CONFIGURATION_PIN_63_INVERT 0
#endif
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


// <e>Custom advertisement data enabled
#ifndef FEATURE_CUSTOM_ADVERTISEMENT_DATA_ENABLED
#define FEATURE_CUSTOM_ADVERTISEMENT_DATA_ENABLED 0
#endif

#if FEATURE_CUSTOM_ADVERTISEMENT_DATA_ENABLED == 1

// <o>Custom data advertisement interval (ms) <500-5000>
#ifndef ADVERTISEMENT_INTERVAL_CUSTOM_DATA
#define ADVERTISEMENT_INTERVAL_CUSTOM_DATA 5000
#endif

// <o>Custom data advertisement duration (s) <0-1440>
// <i>Set to 0 for infinite
#ifndef ADVERTISEMENT_TIMEOUT_CUSTOM_DATA
#define ADVERTISEMENT_TIMEOUT_CUSTOM_DATA 0
#endif

// <o>Custom advertisement data
#ifndef ADVERTISEMENT_CUSTOM_DATA
#define ADVERTISEMENT_CUSTOM_DATA 0xhex, 0xstring, 0xhere
#endif

// <e>Connectable in custom advertisement mode
#ifndef CUSTOM_ADVERTISEMENT_CONNECTABLE
#define CUSTOM_ADVERTISEMENT_CONNECTABLE 0
#endif
// </e>

#endif
// </e>

// <s.13>Default device name
#ifndef DEVICE_NAME
#define DEVICE_NAME "BLEnky"
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

// <o> Binary sensor pin <0-31> <f.d>
#ifndef BINARY_SENSOR_PIN
#define BINARY_SENSOR_PIN 0
#endif
// </e>

// <e>Automation IO profile enabled
#ifndef FEATURE_AUTOMATION_IO_ENABLED
#define FEATURE_AUTOMATION_IO_ENABLED 1
#endif

// <e>Use a different UUID (2A57) for AIO output characteristics
#ifndef AUTOMATION_IO_DIFFERENT_UUIDS
#define AUTOMATION_IO_DIFFERENT_UUIDS 1
#endif
// </e>
// </e>

// <e>gpioASM profile and engine enabled
#ifndef FEATURE_GPIO_ASM_ENABLED
#define FEATURE_GPIO_ASM_ENABLED 1
#endif
// </e>

// <e>Cycling speed/cadence profile and engine enabled
#ifndef FEATURE_CYCLING_SPEED_CADENCE_ENABLED
#define FEATURE_CYCLING_SPEED_CADENCE_ENABLED 0
#endif

// <o> Wheel sensor pin <0-31> <f.d>
#ifndef CSC_WHEEL_SENSOR_PIN
#define CSC_WHEEL_SENSOR_PIN 0
#endif
// </e>

// <e>HID service enabled (experimental)
#ifndef FEATURE_HID_ENABLED
#define FEATURE_HID_ENABLED 0
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
// <o> D-Pad up pin <0-31> <f.d>
#ifndef HID_D_PAD_UP_PIN
#define HID_D_PAD_UP_PIN 0
#endif
// </e>

// <e> D-Pad down enabled
#ifndef HID_D_PAD_DOWN_ENABLED
#define HID_D_PAD_DOWN_ENABLED 0
#endif
// <o> D-Pad down pin <0-31> <f.d>
#ifndef HID_D_PAD_DOWN_PIN
#define HID_D_PAD_DOWN_PIN 0
#endif
// </e>

// <e> D-Pad left enabled
#ifndef HID_D_PAD_LEFT_ENABLED
#define HID_D_PAD_LEFT_ENABLED 0
#endif
// <o> D-Pad left pin <0-31> <f.d>
#ifndef HID_D_PAD_LEFT_PIN
#define HID_D_PAD_LEFT_PIN 0
#endif
// </e>

// <e> D-Pad right enabled
#ifndef HID_D_PAD_RIGHT_ENABLED
#define HID_D_PAD_RIGHT_ENABLED 0
#endif
// <o> D-Pad right pin <0-31> <f.d>
#ifndef HID_D_PAD_RIGHT_PIN
#define HID_D_PAD_RIGHT_PIN 0
#endif
// </e>



// <e> A button enabled
#ifndef HID_BUTTON_A_ENABLED
#define HID_BUTTON_A_ENABLED 0
#endif
// <o> A button pin <0-31> <f.d>
#ifndef HID_BUTTON_A_PIN
#define HID_BUTTON_A_PIN 0
#endif
// </e>

// <e> B button enabled
#ifndef HID_BUTTON_B_ENABLED
#define HID_BUTTON_B_ENABLED 0
#endif
// <o> B button pin <0-31> <f.d>
#ifndef HID_BUTTON_B_PIN
#define HID_BUTTON_B_PIN 0
#endif
// </e>

// <e> Start button enabled
#ifndef HID_BUTTON_START_ENABLED
#define HID_BUTTON_START_ENABLED 0
#endif
// <o> Start button pin <0-31> <f.d>
#ifndef HID_BUTTON_START_PIN
#define HID_BUTTON_START_PIN 0
#endif
// </e>

// <e> Select button enabled
#ifndef HID_BUTTON_SELECT_ENABLED
#define HID_BUTTON_SELECT_ENABLED 0
#endif
// <o> Select button pin <0-31> <f.d>
#ifndef HID_BUTTON_SELECT_PIN
#define HID_BUTTON_SELECT_PIN 0
#endif
// </e>


// </e>

// </e>

// <e>Sleep mode enabled
#ifndef FEATURE_SLEEP_MODE_ENABLED
#define FEATURE_SLEEP_MODE_ENABLED 0
#endif

#if FEATURE_ENABLED(SLEEP_MODE)
//<o>Sleep mode
//<0=> Light sleep (RAM retention)
//<1=> Deep sleep (no RAM retention)
#ifndef SLEEP_MODE
#define SLEEP_MODE 0x1
#endif

//<o>Sleep timeout in minutes <0-180>
#ifndef SLEEP_TIMEOUT_MINUTES
#define SLEEP_TIMEOUT_MINUTES 5
#endif
#endif

//<e>Go to light sleep after graceful disconnect
#ifndef SLEEP_AFTER_DISCONNECT
#define SLEEP_AFTER_DISCONNECT 0
#endif
//</e>

// </e>

#ifndef VIRTUAL_INPUT_PIN_COUNT
#define VIRTUAL_INPUT_PIN_COUNT 0
#endif

// </h>

// <<< end of configuration section >>>
#endif
