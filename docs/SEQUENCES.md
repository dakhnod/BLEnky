## Output sequences

This firmware extends the Autiomation IO Service with a few features, for instance the ability to upload output sequences instead of only momentary states.

A sequence consists of series of pin states and delays between them.
That allows you to blink leds, play funny patterns, periodically toggle Relays etc. without even being connected to the chip.

If output pins are configured, the Automation IO service exposed a characteristic with the UUID `9c102a56-5cf1-8fa7-1549-01fdc1d171dc`, the digital output characteristic.

### Output digital sequence protocol

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

The whole sequence data is split up into packets, the sequence byte is inserted and the packets are sent to the sequence characteristic. Examples will follow.

Let's now take a look at the data:

The first varint indicates how often the sequence should be repeated.
Here are the different cases for the repitition int:
```
0: sequence is repeated indefinetly, until interrupted
n: sequence is repeated n times, until interrupted
```

After that follow packets of pin states and delays.

The pin states are encoded just like when written [directly](https://github.com/dakhnod/NRF51-binary-sensor-firmware/tree/automation-io-service#configuration-protocol). Here's a quick refresher:
```
0b1101000000110101
states: [don't change, HIGH, LOW, LOW, LOW, don't change, HIGH, HIGH]
```
Delays are encoded as varints.
To not overcomplicate things, you'll have to believe me that 1000 in decimal is `0b1110100000000111` encoded as varint.

Now, let's assume we have two output pins configured. We want to set them to [HIGH, LOW] and sleep for 1000ms after that. Here's the corresponding packet:
```
0b01001111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
```

Great, we've controlled pins once and slept after.
Now let's say we want to make the two pins blink, here's the data:

```
0b01001111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
```
This writes the pins twice. We're missing the repeat varint. Let's add it in:
```
0b00000000 // repeat indefinetly

0b01001111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
```
Whoa, we want the blinking to end after 10 blinks, though:
```
0b00001010 // 10 repititions, varint-encoded

0b01001111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
```

Since this fits in 19 bytes (the maximum amount of bytes per write),
we can can wrap the whole sequence into one packet:
```
0b00000000 // sequence number 0, leftmost bit set to 0 since this packet is the last one

0b00001010 // 10 repititions, varint-encoded

0b01001111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
```
Now we can send this packet (0b000A4FE8071fE807) straight to the sequence characteristic and the chip might start playing it.

Lets look at a bigger payload:
```
0b00001010 // 10 repititions, varint-encoded

0b01001111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b01011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b01001111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b01011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
```
We will habe to split this payload up into two packets:

#1:
```
0b10000000 // sequence number 0, first bit indicating that packets will follow

0b00001010 // 10 repititions, varint-encoded

0b01001111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b01011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b01001111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
```
#2:
```
0b00000001 // sequence number 1, first indicated last packet

0b01011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)

0b00011111 // pin bytes
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
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


### Outout combined sequence protocol

If any output pins are configured, the IO service exposes a characteristic with the UUID `9c100056-5cf1-8fa7-1549-01fdc1d171dc`, the so-called combined output sequence cahracteristic.

If only digital outputs are configured, the characteristic accepts the same data and behaves exactly as the digital output sequence characteristic.

If any analog output pins are configured, each sequence packet has to also include values for the analog outputs.
This gives you the opportunity of controlling digital and analog output pins with the same sequence.

The digital pins are encoded like with the regular output sequence.
Analog pins are encoded as little-endian uint16 values, just like then writing to the [analog characteristics](AUTOMATION_IO_SERVICE.md#output-analog-pins) directly.

Setting the value to 0xffff leaves the pin untouched, aka preserves its set pwm duty cycle.

So, here is an example of a packet with analog values:

```
0b01001111 // pin bytes
0b11011100 // first analog value (1500) first byte
0b00000101 // first analog value (1500) second byte
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
```

Here is an example with multiple analogs:


```
0b01001111 // pin bytes
0b11010000 // first analog value (2000) first byte
0b00000111 // first analog value (2000) second byte
0b11111111 // second analog value (0xffff) first byte
0b11111111 // second analog value (0xffff) second byte
0b11101000 // delay varint byte 0 (sleep)
0b00000111 // delay varint byte 1 (sleep)
```