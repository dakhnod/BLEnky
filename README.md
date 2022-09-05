# BLE GPIO Bridge

This project provides a firmware for cheap, nRF51-based IO-modules to control LEDs, relays, motors (and more) and read from buttons, sensors (etc.) via Bluetooth low energy.
An IO module can have inputs and outputs that can be read from and written to.

To summarize, this is basically like Tasmota for the nRF51, that can be powered from a coin cell for a long time.

With this firmware you can basically use every available pin on the chip as an input or an output.
Well, until a buffer overruns or the RAM runs out or something...

This firmware allows for flashing an nRF51 once and control and configuration via a [website](https://ble.nullco.de),
without needing to reflash the chip.

Furthermore, it gives you the oppurtunity to not only write bits to output pins, but upload whole sequences (also known as animations)
that can repeat and will be played by the chip, even when nothing is connected to it, as seen in the following gif.

![Running sequence](https://user-images.githubusercontent.com/26143255/188250995-35686f41-e14b-4790-976d-508945753b45.gif)

## Table of contents

1. [Configuration](docs/CONFIGURATION.md)
2. [Pin inputs/outputs (Automation IO)](docs/AUTOMATION_IO_SERVICE.md)
3. [Pin inputs (Binary Sensor Service)](docs/BINARY_SENSOR_SERVICE.md)
4. [Sequences/Animations](docs/SEQUENCES.md)

## Roadmap

Here are a few things I have planned for the future

- [ ] Add analog (PWM) outputs for servo control
- [ ] Add analog inputs to read out voltage(s)
- [ ] Add debounce settings to allow for quicker button presses
- [ ] Add selection of common boards to website
