# BLE GPIO Multitool

## Configuration protocol

Input and output pins can be configured without reprogramming the device by sending a configuration packet to `9c100001-5cf1-8fa7-1549-01fdc1d171dc`.
This packet is constructed of four bits per pin, aka two pins per byte.

The leftmost bit per pin decides whether the pin is an output or an input:
```
0xxx: output
1xxx: input
```

The rightmost bit defines the inversion of the pin.
When that bit is set, the output/input of the pin will be opposite of the voltage level reading.
```
xxx0: not inverted
xxx1: inverted
```

### Output pins
The third byte defines the default pin state.
```
0x0x: LOW after startup
0x1x: HIGH after startup
```
The inversion bit inverts the real output on the pin.

Here are a few examples:
```
0000: pin configured as output, default LOW, not inverted
0010: pin configured as output, default HIGH, not inverted
0001: pin configured as output, default LOW, inverted (real output will be HIGH by default due to inversion)
0011: pin configured as output, default HIGH, inverted (real output will be LOW by default)
```

### Input pins
Bit 2 and 3 define the input pullup/down.
```
100x: no pullup/pulldown
101x: pulldown enabled
110x: pullup enabled
```

Here the inversion bit also applies and inverts the reading.

Here a few examples:
```
1000: input, no pull, not inverted
1010: input, pulldown, not inverted
1100: input, pullup, not inverted
1001: input, no pull, inverted
1011: input, pulldown, inverted
1101: input, pullup, inverted
```

### Disabled pins
Setting the bits to `1111` disables that pin.

## Packet structure
The pin bits are read and applied to pins starting from 0.
Sending just `0000` would enable pin 0 as output.
Since only bytes can be sent, That affect can be achieved by sending `11110000`.

Lets say we want to use pin 2 as an inverted output and pin 6 as input. Our packet would look like this:

```
1111 # disabling pin 0
1111 # disabling pin 1
0001 # pin 2 as output with inversion
1111 # disabling pin 3
1111 # disabling pin 4
1111 # disabling pin 5
1000 # enabling pin 6 as input
1111 # disabling pin 7, need bits for padding
```
or
`0b11111111000111111111111110001111`
or
`0x4280287119`.

After sending the payload the packet will be padded with `0xff` to reach 16 bytes and stored in the flash.
The microcontroller will then reboot and apply the new configuration.
The configuration persists reboots, needs to be done only once.

Also, the configuration characteristic can be read to disconver the configuration already present on the device.