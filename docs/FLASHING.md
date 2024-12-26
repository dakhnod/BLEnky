# Flashing

## Flashing the nRF51 via bootloader

This is the recommended way to flash the firmware since it allows for
updates over the air (over BLE).

### Flashing the bootloader using a Raspberry Pi

Firstly, get openocd to run. I (cannot remember why) compiled the source from https://github.com/openocd-org/openocd myself on the RasPi.
Feel free to try out https://github.com/raspberrypi/openocd aswell. Not sure what the difference is.

Then download `bootloader_with_softdevice_delay.hex` [here](https://github.com/dakhnod/nRF51-GPIO-BLE-Bridge/releases/v0.1.0)

You can use this command to flash the hex (mind the references to the nrf51.cfg, change to nrf52.cfg if needed):
```
openocd -s ${OPENOCD_INSTALL_DIR}/tcl -f openocd/raspi-bcm2385.tcl -f target/nrf51.cfg -c "program bootloader_with_softdevice_delay.hex verify reset exit"
```
with `openocd/raspi-bcm2385.tcl` using [pins](https://pinout.xyz/) 11 for `swd` and 25 for `swio`, but feel free to change those.

### Flashing the bootloader using JLink

Get the bootloader [here](https://github.com/dakhnod/nRF51-GPIO-BLE-Bridge/releases/v0.1.0)
and flash the hex via nrfjprog, or any other program capable of swd.
Here's how to do it using nrfjprog
```
nrfjprog --chiperase
nrfjprog --program path_to_bootloader.hex
nrfjprog --reset
```

### Rebooting into bootloader

If you just installed the bootloader after a wipe, the device is already in bootloader mode.
You can proceed with the installation.

If you can cut power to the device, power-cycle it.
For a few seconds, you'll be able to connect to the device named "DfuTarg".
If you fail to connect in time, power-cycle and try again.

If you can't power-cycle the device, you can restart it by performing the following:
1. Enable notifications on the characteristic `8e400001-f315-4f60-9fb8-838830daea51`
2. Write the bootloader secret to characteristic `8e400001-f315-4f60-9fb8-838830daea51`.
You can find and change the secret [here](https://github.com/dakhnod/nRF51-common/blob/master/services/dfu_service/bootloader_secret.h)
3. The device should disconnect and reboot into bootloader more. With this way of manually triggering bootloader mode, it will not boot back into the application after a few seconds.


### Flashing through bootloader

First, you need to download the application .zip file from the [latest release](https://github.com/dakhnod/nRF51-GPIO-BLE-Bridge/releases/latest). 

You should use an app like "nRF Connect" to connect to the device in bootloader mode named "DfuTarg".
There, you can tap on "DFU" and select the zip file.
When flashing is done the device will boot back into the application and you are done.

## Flashing without bootloader

To flash the hex directly, you can either compile the code yourself or download the nrf51822__xxac.hex from the [latest release](https://github.com/dakhnod/nRF51-GPIO-BLE-Bridge/releases/latest).

Then, you need to flash the softdevice.
This step needs to be done only once, not with every update.

```
nrfjprog --chiperase
nrfjprog --program path_to_softdevice.hex
```

Then, flash the application.

```
nrfjprog --chiperase
nrfjprog --program path_to_application.hex
nrfjprog --reset
```

And you should be up and running.