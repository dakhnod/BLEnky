# gpioASM Service

the gpioASM Service exposes an endpoint for uploading [gpioASM](https://github.com/dakhnod/gpioASM/blob/main/docs/GPIO_ASM.md) executables.

The easiest way to do this is via the "gpioASM upload" segment on the [website](https://ble.nullco.de/#gpio-asm-upload).

Just upload your gpioASM source and it will be compiled, loaded onto the chip and executed.

The service has the UUID `b1190000-2a74-d5a2-784f-c1cdb3862ab0`.

## gpioASM Upload characteristic

the upload characteristic has the UUID `b1190001-2a74-d5a2-784f-c1cdb3862ab0`.

The gpioASM executable should be split up into packets sized max. `MTU - 1`.

Every packet is prefixed by a sequence number.

The sequence number has the leftmost bit set if any packets follow.
An unset leftmost bit indicates the terminating packet and starts gpioASM execution on the chip.
Here is an example of sequence numbers: 

`[0b10000000, 0b10000001, 0b10000010, 0b00000011]`