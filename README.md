# BLE GPIO Multitool

## Pin interface protocol

The firmware exposes endpoints for reading from input pins and writing to output pins,
if any are configured. To configure pins, read about the configuration protocol.

When controlling pins, each bit is represented by two bits.
This part of the protocol mostly adheres to the [Bluetooth Automation IO Service](https://www.bluetooth.com/de/specifications/specs/automation-io-service-1-0/).

If inputs and outputs are configured, there are two characteristics with the UUID `00002a56-0000-1000-8000-00805f9b34fb` exposed.
The one allowing to be written to is the output-endpoint, the other one the input.
Each characteristic exposes a Descriptor with the UUID `00002909-0000-1000-8000-00805f9b34fb`, exposing the amount of configured pins.

### Output pins

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

## Output sequences

This firmware extends the Autiomation IO Service with a few features, for instance the ability to upload output sequences instead of only momentary states.

A sequence consists of series of pin states and delays between them.
That allows you to blink leds, play funny patterns, periodically toggle Relays etc. without even being connected to the chip.

If output pins are configured, the Automation IO service exposed a characteristic with the UUID `9c102a56-5cf1-8fa7-1549-01fdc1d171dc`, the digital output characteristic.

### Output sequence protocol

First, let us establish the concept of `varint`s.
Varints are used in Google's Protobuf amongst other things and are described pretty well [here](https://developers.google.com/protocol-buffers/docs/encoding#varints).

In short, varints can encode integers with variable sized arrays. The higher the number, the bigger the array.

Each byte of a varint has the leftmost bit set to 1, excluding the last byte.
The first bit acts as an other-bytes-follow bit.

When uploading a sequence, the data can be sent with multiple packets, allowing for longer sequences.

The first byte in each packet sent to the output characteristic is the sequence number.
The sequence number starts with 0 and is counted up by one with every write.
When one number is skipped, the whole sequence is rejected.
Also, the leftmost bits in the sequence byte indicates that packets will follow.
The last write has to have the leftmost sequence-bit set to 0.
Here are the sequence numbers for 4 writes:
`0b10000000, 0b10000001, 0b10000010, 0b00000011`.

The whole sequence data is split up into packets, the sequence byte is inserted and the packets are sent to the sequence characteristic.  Examples will follow.

Let's now take a look at the data:

The first varint indicates how often the sequence should be repeated.
Here are the different cases for the repitition int:
```
0: sequence is repeated indefinetly, until interrupted
n: sequence is repeated n times, until interrupted
```

After that follow packets of pin states and delays.

The pin states are encoded just like when written directly. Here's a quick refresher:
```
0b1101000000110101
states: [don't change, HIGH, LOW, LOW, LOW, don't change, HIGH, HIGH]
```
Delays are encoded as varints.
To not overcomplicate things, you'll have to believe me that 1000 in decimal is `0b1110100000000111` encoded as varint.

Now, let's assume we have two output pins configured. We want to set them to [HIGH, LOW] and sleep for 1000ms after that. Here's the corresponding packet:
```
0b01001111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)
```

Great, we've controlled pins once and slept after.
Now let's say we want to make the two pins blink, here's the data:

```
0b01001111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)
```
This writes the pins twice. We're missing the repeat varint. Let's add it in:
```
0b00000000 // repeat indefinetly

0b01001111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)
```
Whoa, we want the blinking to end after 10 blinks, though:
```
0b00001010 // 10 repititions, varint-encoded

0b01001111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)
```

Since this fits in 19 bytes (the maximum amount of bytes per write),
we can can wrap the whole sequence into one packet:
```
0b00000000 // sequence number 0, leftmost bit set to 0 since this packet is the last one

0b00001010 // 10 repititions, varint-encoded

0b01001111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)
```
Now we can send this packet (0b000A4FE8071fE807) straight to the sequence characteristic and the chip might start playing it.

Lets look at a bigger payload:
```
0b00001010 // 10 repititions, varint-encoded

0b01001111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b01011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b01001111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b01011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)
```
We will habe to split this payload up into two packets:

#1:
```
0b10000000 // sequence number 0, first bit indicating that packets will follow

0b00001010 // 10 repititions, varint-encoded

0b01001111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b01011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b01001111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)
```
#2:
```
0b00000001 // sequence number 1, first indicated last packet

0b01011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // varint byte 0 (sleep)
0b00000111 // varint byte 1 (sleep)
```

The sequence characteristic can be read and subscribed to to obtain information about the playing sequence.

Reading the characteristic will yeald 9 bytes:
```
0b00000001 // sequence is playing (0b00000000 means sequence not playing)

0b00000110 // four bytes indicating the current packet index
0b00000000 // starting from 0 with every new sequence start
0b00000000 // when a new bit state from the sequence is applied,
0b00000000 // this number grows by 1. Ignore this four-byte number if sequence is not playing.

0b00000000 // four bytes indicating the amount of remaining repititions
0b00000000 // when this four-byte number is 0, the sequence is played indefinetly 
0b00000000 // ignore is sequence is not playing.
0b00000000
```
Subscribing to the sequence characteristic will send updates from the sequence playback via notifications with the above described 9-byte format.

## Configuration protocol

Input and output pins can be configured without reprogramming the device by sending a configuration packet to the characteristic with the UUID  `9c100001-5cf1-8fa7-1549-01fdc1d171dc`.
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

Here the inversion bit also applies and inverts the reading.

Bit 2 and 3 define the input pullup/down.
```
100x: no pullup/pulldown
101x: pullup enabled
110x: pulldown enabled
```

Here a few examples:
```
1000: input, no pull, not inverted
1010: input, pullup, not inverted
1100: input, pulldown, not inverted
1001: input, no pull, inverted
1011: input, pullup, inverted
1101: input, pulldown, inverted
```

### Disabled pins
Setting the bits to `1111` disables that pin.

## Packet structure
The pin bits are read and applied to pins starting from pin 0.
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
```
 pin 7                               pin 0
0b1111 1000 1111 1111 1111 0001 1111 1111
```
or
`0xF8FFF1FF`.

After sending the payload the packet will be padded with `0xff` to reach 16 bytes and stored in the flash.
The microcontroller will then reboot and apply the new configuration.
The configuration persists reboots, needs to be done only once.

Also, the configuration characteristic can be read to discover the configuration already present on the device.
