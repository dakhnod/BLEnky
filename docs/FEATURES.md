# Features

The source includes a few features, that can be turned on or off during compilation.
The feature configuration can be configured by using the [CMSIS configuration wizard](https://sourceforge.net/projects/cmsisconfig/) like this: `java -jar CMSIS_Configuration_Wizard.jar src/config/feature_config.h`.

To preserve memory, the project should be configured as minimal as possible.

## GPIO configuration

### GPIO_INPUT_COUNT_MAX

This option sets the amount of memory allocated for input pins.

### GPIO_OUTPUT_COUNT_MAX

This option sets the amount of memory allocated for output pins.

### GPIO_DEBOUNCE_TIMEOUT_MS

Repeated inputs on one input will be ignored if they occur within the debounce timeout.

### Battery profile

This feature adds a service to monitor the charging state and voltage of the battery

### Binary sensor profile

Adds a binary sensor service that includes only the first input.
The format and protocol follows the binary sensor profile.

### Automation IO

Adds an Automation IO profile that includes all inputs and outputs.
The format and protocol follows the Automation IO profile.

### gpioASM

This feature adds the [gpioASM](https://github.com/dakhnod/gpioASM) service to upload compiled gpioASM.
Also, the gpioASM engine/runtime is enabled so that the code can be executed.

### Cycling speed / cadence

Adds a `cycling speed and cadence` service that reads out the input with index 0.
The service periodically reports the revolution count and last revolution event time.

### HID

Experimental feature

### Sleep mode

This feature, if enabled, puts the chip to sleep mode.

In sleep mode, the chip cannot be connected to.

Any GPIO input wakes up the chip again.

#### Configuration

##### Sleep mode
- Light sleep: Clock keeps ticking, ram is retained. Needs a bit of power
- Deep sleep: Clock off, RAM wiped. Chip will be rebooted on wakeup. Needs very little power

##### Sleep timeout
Once there is no GPIO input for this amount of minutes, the chip goes to sleep mode according to the configuration above.

Technically, the timeout will most likely not be met exactly.

If set to e.g. 10 minutes, the chip will go to sleep in 10 to 11 minutes.