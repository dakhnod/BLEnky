# Nordic DFU based boards

For these boards, find the right firmware in [in the latest release](https://github.com/dakhnod/BLEnky/releases/latest).
To flash these, you need to plugin the dongle and install nrfutil.
If you can't find anything suiting, get the generic build and configure it through the website.

Then get this file and run
```
nrfutil dfu serial -p DONGLE_PORT -pkg FIRMWARE_FILE.zip
```

Once the firmware is copied/flashed, the flash drive will disappear and you should be able to head to https://ble.nullco.de and connect to your board. You just saved yourself years of development!

To get back into the bootloader, a quick doble reset should suffice.