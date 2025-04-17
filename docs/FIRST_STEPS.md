# First steps

Right, so you got a shiny new board.
Chances are there is a freshly baked preconfigured firmware waiting just for you.
Just check the pictures if anything looks like your board and click on it.
After flashing your file feel free to re-configure BLEnky using the section at the bottom of the page.

If you don't find your board here, try looking through the [latest release](https://github.com/dakhnod/BLEnky/releases/latest) for a package that sounds like your board.
If your board shows up as a drive and says something about UF2, follow [these instructions](#uf2-based-boards).
If it just shows up as a Serial port, it likely has the nordic bootloader installed, go with [this](#nordic-dfu-based-boards).
If it doesn't show up at all, follow [this](#generic-board--without-bootloader).

[<img height="100" src="https://files.seeedstudio.com/wiki/XIAO-BLE/nrf52840_front.jpg">](#xiao-ble)
[<img height="100" src="https://docs-be.nordicsemi.com/bundle/ncs-latest/page/zephyr/_images/others_promicro_nrf52840.webp?_LANG=enus">](#pro-micro--nice-nano)
[<img height="100" src="https://camo.githubusercontent.com/b0ed7718a2938ed9660fe2611832c7d0cc850ea262c49a9e873610f651241ffe/68747470733a2f2f6769746875622d70726f64756374696f6e2d757365722d61737365742d3632313064662e73332e616d617a6f6e6177732e636f6d2f3835323534372f3239353031303232332d66363464356466372d633465642d343862382d613961382d6162663534653031343739312e6a7067">](#pro-micro)
[<img height="100" src="https://www.nordicsemi.com/-/media/Images/Products/DevKits/nRF52-Series/nRF52840-Dongle/nRF52840-Dongle-rev2-prod-page.png?h=750&iar=0&mw=350&w=350&hash=994DC726E9F3DBB12056C07F5DC0A802">](#nrf52840-dongle)

## XIAO BLE

On these boards, you need to update the bootloader first.
Thus install `adafruit-nrfutil` using pip.
Then, get THIS FILE and run `adafruit-nrfutil dfu serial -p BOARD_PORT -pkg BOOTLOADER_FILE.zip`.
After, make sure that the .txt on the board shows Softdevice version 6.1.1.
After, you just need to copy the right file onto the board, which appears as a flash drive.
The firmware file to search for contains `xiao_ble.uf2`.
Firmware files can be found [in the releases](https://github.com/dakhnod/BLEnky/releases/latest).

## Pro micro / Nice nano

These boards come pre-flashed with a uf2 bootloader, so you just need to copy the right file onto the board, which appears as a flash drive.
The firmware file to search for contains `others_promicro_nrf52840.uf2`.
Firmware files can be found [in the releases](https://github.com/dakhnod/BLEnky/releases/latest).

## nRF52840 Dongle

These boards come pre-flashed with the nordic bootloader.
To flash these, you need to plugin the dongle and install nrfutil.

Then get this file and run
```
nrfutil dfu serial -p DONGLE_PORT -pkg FIRMWARE_FILE.zip
```
The file to look for contains `nordic_nrf52840dongle.zip`.
Firmware files can be found [in the releases](https://github.com/dakhnod/BLEnky/releases/latest).

## UF2 based boards

For these boards, find the right `.uf2` firmware in [in the latest release](https://github.com/dakhnod/BLEnky/releases/latest) and copy it onto the drive that should appear once you connect the board.
If you can't find anything suiting, get the generic build and configure it through the website.

Just make sure, using the text file on the drive, that the Softdevice version is 6.1.1.

## Nordic DFU based boards

For these boards, find the right firmware in [in the latest release](https://github.com/dakhnod/BLEnky/releases/latest).
To flash these, you need to plugin the dongle and install nrfutil.
If you can't find anything suiting, get the generic build and configure it through the website.

Then get this file and run
```
nrfutil dfu serial -p DONGLE_PORT -pkg FIRMWARE_FILE.zip
```

## Generic board / without bootloader

If your board does not appear to have a bootloader at all, get the hex file for your board, or the generic hex file.
Then follow [these instruction](FLASHING.md).