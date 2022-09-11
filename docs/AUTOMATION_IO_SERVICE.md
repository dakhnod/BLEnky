## TOC

1. [Interfacing with digital output pins](#output-digital-pins)
2. [Interfacing with analog output pins](#output-analog-pins)
3. [Interfacing with input pins](#input-pins)


## Pin interface protocol

The firmware exposes endpoints for reading from input pins and writing to output pins,
if any are configured. To configure pins, read about the [configuration protocol](CONFIGURATION.md).

When controlling pins, each bit is represented by two bits.
This part of the protocol mostly adheres to the [Bluetooth Automation IO Service](https://www.bluetooth.com/de/specifications/specs/automation-io-service-1-0/).

If inputs and outputs are configured, there are two characteristics with the UUID `00002a56-0000-1000-8000-00805f9b34fb` exposed.
The one allowing to be written to is the output-endpoint, the other one the input.
Each characteristic exposes a Descriptor with the UUID `00002909-0000-1000-8000-00805f9b34fb`, exposing the amount of configured pins.

### Output digital pins

When a pin is configured as an output, here is what the bit combinations mean:
```
0b00: output LOW
0b01: output HIGH
0b10: output high-impedance (not supported by this firmware)
0b11: ignore pin
```
All of the pins to be written are concatenated into the minimum amount of bytes needed and send to the characteristic `00002a56-0000-1000-8000-00805f9b34fb` (the one allowing writes).
The bytes are padded with 0b11.
Here are a few examples of one and multiple pins:
```
[LOW]
0b00111111

[HIGH]
0b01111111

[LOW, LOW]
0b00001111

[HIGH, HIGH]
0b01011111

[HIGH, LOW, LOW, HIGH]
0b01000001

[HIGH, LOW, LOW, HIGH, LOW]
0b01000001 00111111
```

If you want to set only one pin instead of all of them, you can send 0b11 as the other pin states.
Sending `0b11110011` for instance will only touch the third output and set it to LOW.
Sending `0b01110000` will set the first pin to HIGH and the third and fourth pin to LOW,
while leaving the second pin output as it is.

### Output analog pins

For every pin that is configured as an analog output, the IO Service will expose a characteristic with the UUID
`00002a58-0000-1000-8000-00805f9b34fb`.
This characteristic accepts a two-byte little-endian unsigned integer representing the output PWM duty cycle.
The PWM pulse width ranges from 0us to 20000us.
Due to a PWM frequency of 50HZ an analog output can be used to control a standard servo by writing values
from 1000us to 2000us, even outside of that if your servo supports that.
Writing `0xDC05` for instance would set the output to a duty cycle of 1500us, setting a servo to center position.
The value 0xffff will be ignored.

It should noted that having any analog pins configured raises the boards current consumption to a minimum of 1mA.

### Input pins

If configured, the characteristic with the UUID `00002a56-0000-1000-8000-00805f9b34fb` that is not not writable will expose information about input pins.
Every pin is represented by two bits, just like the outputs:
```
0b00: pin reads LOW
0b01: pin reads HIGH
0b10: can't remember, not supported in this firmware
0b11: pin not available, most likely padding
```
Bytes from the characteristic are read from left to right,
bits within the bytes are also read from left to right.

Here are a few examples:
```
0b00111111: one input pin, reading LOW
0b01111111: one input pin, reading HIGH
0b01010101: four input pins, all reading HIGH
0b01001111: two input pins, first HIGH, second LOW
0b00000100 01011111: six input pins, reading LOW, LOW, HIGH, LOW, HIGH, HIGH
```
The input-characteristic can be read or subscribed to using notifications.
When notifications are enabled, each change on one of the input pins will trigger a notification containing all input pin states via the above described format.