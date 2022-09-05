# Configuration protocol

## TOC

1. [Configuring output pins](#output-pins)
2. [Configuring input pins](#input-pins)
3. [Configuring connection parameters](#connection-parameters-configuration)

## Pin configuration

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


## Connection parameters configuration

The modules preferred connection parameters can be configured by sending data to the Characteristic with UUID `9c100002-5cf1-8fa7-1549-01fdc1d171dc`.

The packet has to include the following fields (all in milliseconds except for slave_latency, little endian):

```
typedef struct {
  uint16_t min_conn_interval;
  uint16_t max_conn_interval;
  uint16_t slave_latency;
  uint16_t conn_sup_timeout;
  uint16_t advertising_interval;
} ble_configuration_connection_params_packet_t;
```

So, here's how an example packet looks like:
Min conn interval: 100ms (0x0064)
Max conn interval: 300ms (0x012C)
Slave latency: 3 (0x0003)
Supervision timeout: 2500ms (0x09C4)
Advertising interval: 1000ms (0x03E8)

And here's the assembled packet:

0x6400 2C01 0300 C409 E803

All of the Bluetooth constraints apply:

- min_conn_interval range: 7.5ms - 4000ms
- max_conn_interval range: 7.5ms - 4000ms
- min_conn_interval needs to be smaller than max_conn_interval
- Max conn_sup_timeout: 3200ms
- conn_sup_timeout needs to be greater than `(max_conn_interval * 2) * (slave_latency + 1)`
- Advertisement interval range: 20ms - 1024ms