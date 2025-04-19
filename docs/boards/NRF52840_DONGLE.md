# nRF52840 Dongle

These boards come pre-flashed with the nordic bootloader.
To flash these, you need to plugin the dongle and install nrfutil.

Then get this file and run
```
nrfutil dfu serial -p DONGLE_PORT -pkg FIRMWARE_FILE.zip
```
The file to look for contains `nordic_nrf52840dongle.zip`.
Firmware files can be found [in the releases](https://github.com/dakhnod/BLEnky/releases/latest).

Once the firmware is copied/flashed, the flash drive will disappear and you should be able to head to https://ble.nullco.de and connect to your board. You just saved yourself years of development!

To get back into the bootloader, a quick doble reset should suffice.