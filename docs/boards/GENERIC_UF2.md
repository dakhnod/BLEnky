# UF2 based boards

For these boards, find the right `.uf2` firmware in [in the latest release](https://github.com/dakhnod/BLEnky/releases/latest) and copy it onto the drive that should appear once you connect the board.
If you can't find anything suiting, get the generic build and configure it through the website.

Just make sure, using the text file on the drive, that the Softdevice version is 6.1.1.

Once the firmware is copied/flashed, the flash drive will disappear and you should be able to head to https://ble.nullco.de and connect to your board. You just saved yourself years of development!

To get back into the bootloader, a quick doble reset should suffice.