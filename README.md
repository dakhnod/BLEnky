# BLEnky

[![Firmware test badge](https://github.com/dakhnod/blenky/actions/workflows/firmware-test.yml/badge.svg 'Firmware test badge')](https://github.com/dakhnod/BLEnky/actions/workflows/firmware-test.yml)


![Running sequence](https://user-images.githubusercontent.com/26143255/189000402-cf582116-7096-429b-8a44-aa2442ba5524.gif)

Want to make a device smart and super low power withing minutes? Here you go!

You can see this Project in action [here](https://youtu.be/jnUlXBZHBno), [here](https://youtu.be/GZ5C588gBdo) and [here](https://youtu.be/10ko1Ppw78A).
An exhaustive list of Projects can be found [here](https://daniel.nullco.de/) under `Projects/Software/Automation/BLEnky`.

Firmware for cheap, nRF51 or nRF52-based IO-modules to control LEDs, relays, servos, motors (and more) and read from buttons, sensors (etc.), build Cycling sensors and HID controlelrs via Bluetooth low energy.
An IO module can have inputs and outputs that can be read from and written to.

All of the features can be [enabled/disabled](docs/FEATURES.md) to save ram and flash storage.

To summarize, this is basically like Tasmota for the nRF51, that can be powered from a coin cell for a long time.

With this firmware you can use every available pin on the chip as an input or an output.
Well, until a buffer overruns or the RAM runs out or something...

This firmware allows for flashing an nRF51/nRF52 once and control and configuration via a [website](https://ble.nullco.de),
without needing to reflash the chip.

Furthermore, it gives you the oppurtunity to not only write and read bits to/from pins, but also to upload little [gpioASM](https://github.com/dakhnod/gpioASM) programms that run offline on the chip.

Learn about my other projects [here](https://daniel.nullco.de).

## Table of contents

1. [Configuration](docs/CONFIGURATION.md)
2. [Feature configuration](docs/FEATURES.md)
3. [Pin inputs/outputs (Automation IO)](docs/AUTOMATION_IO_SERVICE.md)
4. [Pin inputs (Binary Sensor Service)](docs/BINARY_SENSOR_SERVICE.md)
5. [gpioASM](docs/GPIO_ASM_SERVICE.md)
6. [Compilation](docs/COMPILATION.md)
7. [Programming the chip](docs/FLASHING.md)
8. [First steps](docs/FIRST_STEPS.md)

## Roadmap

Here are a few things I have planned for the future

- [x] Add analog (PWM) outputs for servo control
- [ ] Add analog inputs to read out voltage(s)
- [x] Add debounce settings to allow for quicker button presses
- [x] Add selection of common boards to website
