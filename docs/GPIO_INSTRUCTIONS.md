# GPIO instructions (gpioASM)

The firmware allows for uploading and running simple instructions to write and read pins and implement simple logic flows.
The system can be used to upload simple sketches to `power a motor until an endstop is reached`, `toggle an LED with a button press`, `turn a stepper motor until a hall sensor is triggered` and many more.

Here are the available instructions:

| command | arguments | meaning |
| ------- | --------- | ------- |
| `0b00000000` | [digital bits](#digital-bits) | [write digital outputs](#write-digital-output) |
| `0b00010000` | uint16_t | [write first analog channel](#write-analog-outputs) |
| `0b00010001` | uint16_t | [write second analog channel](#write-analog-outputs) |
| `0b00010010` | uint16_t | [write third analog channel](#write-analog-outputs) |
| `0b00010011` | uint16_t | [write fourth analog channel](#write-analog-outputs) |
| `0b00100000` | [varint](#varint) | [sleep for n milliseconds](#sleep-for-n-milliseconds) |
| `0b00100001` | [digital bits](#digital-bits) | [sleep until all inputs match](#sleep-until-all-inputs-match) |
| `0b00100010` | [digital bits](#digital-bits) | [sleep until any inputs match](#sleep-until-any-inputs-match) |
| `0b01000000` | [varint](#varint) | [jump to code location](#jump-to-code-location) |
| `0b01000001` | [varint](#varint), [digital bits](#digital-bits) | [jump to location if all pins match](#jump-to-location-if-all-pins-match) |
| `0b01000010` | [varint](#varint), [digital bits](#digital-bits) | [jump to location if any pins match](#jump-to-location-if-any-pins-match) |
| `0b01000010` | [varint](#varint) | [perform jump max. n times](#perform-jump-max-n-times)
| `0b10000000` | | [stop script execution](#stop-script-execution)

## Datatypes

### digital bits

Digital bits represent logical states of digital GPIO pins.
Every digital pin can be in one of four states:

| state bits | state description |
| --- | --- |
| `0b00` | pin is LOW |
| `0b01` | pin is HIGH |
| `0b10` | pin is high-impedance |
| `0b11` | pin is not available or ignored |

Pin states are represented via bytes.
Every byte can house up to four pin states, unconfigured pin bits are set to `0b11`.
For example, five pins, states [HIGH, LOW, LOW, LOW, HIGH] are represented by two bytes, and look like this:

`0b01000000 01111111`

The amount of bytes to send depends on the amount of configured digital output pins.
If the chip boots with 6 digital output pins configured, it always expects a digital payload sized 2 bytes (`CEIL(pin_count / 4)`).

### varint

Varints are encoded integers that grow bytewise in size with higher integer values.
Best described [here](https://developers.google.com/protocol-buffers/docs/encoding).

## Commands


### Write digital output

This instructions reads in the following bytes and writes the encoded pin states to the configured digital outputs. The amount of expected bytes depends on the digital output pin count, as described [here](#digital-bits).

Example:

`0b00000000 11000111`

Writes LOW to the second output pin and HIGH to the third.
The first output pin remains untouched since the bits are set to `0b11`.

### Write analog output(s)

This instruction reads in the next two bytes and parses them as an unsigned 16-bit little-endian encoded integer.
It uses this value as microseconds to output as an anlog channels duty cycle.

Example:

`0b00010000 11011100 00000101`

outputs a duty cycle of 1500us on analog channel 0.

`0b00010010 11101000 00000011`

outputs a duty cycle of 1000us on analog channel 2.

### sleep for n milliseconds

This instruction reads in a [varint](#varint) and uses its value in milliseconds to sleep.
Script execution continues after the sleep is over.

Example:

`0b00100000 01100100`

delays script execution for 100ms.


### sleep until all inputs match

This instruction takes in [digital bits](#digital-bits) and waits until all the input pins match the given states.
It can be used to wait for endstops, reed switches, button presses etc.

Example:

`0b00100001 11110001`

Waits for pins #3 to become LOW and pin #4 to become HIGH.
Pins #1 and #2 are ignored since their bits are set to `0b11`.

### sleep until any inputs match

same as [sleep until all inputs match](#sleep-until-all-inputs-match), but one single matching input is enough to terminate the sleep.

Example:

`0b00100010 11110001`

sleep terminates when either pin #3 is LOW __or__ pin #4 is high.
Pins #1 and #2 are ignored since their bits are set to `0b11`.

### jump to code location

This instruction reads in a [varint](#varint) and sets the internal instruction pointer to that value, continuing code execution from there.

Example:

`0b01000000 00000000`

jumpts to byte index 0, restarting script execution from the first instruction.

### jump to location if all pins match

This instruction reads in a byte index like the [simple jump](#jump-to-code-location), but also reads in [bit states](#digital-bits).
It only performs the jump if the configured input pin states match the requested pin states.

Example:

`0b01000001 00000000 11110100`

only jumpts to byte index 0, when pin #3 reads HIGH and pin #4 reads LOW.
Pins #1 und #2 are ignored since their bits are set to `0b11`.

### jump to location if any pins match

This instruction works just like [jump if all match](#jump-to-location-if-all-pins-match), but one single amtchis sufficient to trigger the jump. Not all pins have to match.

Example:

`0b01000010 00000000 11110100`

only jumpts to byte index 0, when pin #3 reads HIGH __or__ pin #4 reads LOW.
Pins #1 und #2 are ignored since their bits are set to `0b11`.

### perform jump max. n times

This instruction reads in a [varint](#varint) jump target and a [varint](#varint) `n`, and repeats the jump only `n` number of times.

The runtime knows what jump to count for as long as no other jump instruction is executed.
As soon as any other jump instruction is executed, the runtime forgets what jump it was supposed to count for.

Example:

`0b01000010 00000000 01100100`

jumps back to byte index 0 100 times, unless the loop is jumped out of.

### stop script execution

this instruction requires no arguments and simply stops the script execution.
No further instruction is read in after this is executed.