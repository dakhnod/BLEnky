# Compilation

1. Set up SDK_ROOT in the Makefile to point at an unpacked nRF5-sdk (version 12.3) folder. You can download the SDK [here](https://www.nordicsemi.com/Products/Development-software/nRF5-SDK/Download#infotabs).
2. First, you need to install `gcc-arm-none-eabi-4_9-2015q3`. Other versions of the compiler will lead to weird behaviour.
3. Then, in the root project directory where the `Makefile` is located, you can run `make default` to compile.

Here is a list of commands in the Makefile:
```
default: compile
flash: flash compiled hex to chip
reset: reboot the chip
flash_softdevice: flash the softdevice configured in the Makefile
erase: erase program and configuration data on the chip
clean: clean the build directory
```