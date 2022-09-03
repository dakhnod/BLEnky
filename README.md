# BLE GPIO Multitool

This project provides a firmware for cheap, nRF51-based IO-modules to control LEDs, relays, motors (and more) and read from buttons, sensors (etc.) via Bluetooth low energy.
An IO module can have inputs and outputs that can be read from and written to.

With this firmware you can basically use every available pin on the chip as an input or an output.
Well, until a buffer overruns or the RAM runs out or something...

This firmware allows for flashing an nRF51 once and control and configuration via a [website](https://ble.nullco.de),
without needing to reflash the chip.

Furthermore, it gives you the oppurtunity to not only write bits to output pins, but upload whole sequences (also known as animations)
that can repeat and will be played by the chip, even when nothing is connected to it, as seen in the following gif.

![](https://user-images.githubusercontent.com/26143255/188250995-35686f41-e14b-4790-976d-508945753b45.gif)

## Table of contents

1. [Configuration](docs/CONFIGURATION.md)
2. [Pin control](docs/AUTOMATION_IO.md)
3. [Sequences/Animations](docs/SEQUENCES.md)
