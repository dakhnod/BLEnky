# XIAO BLE

On these boards, you need to update the bootloader first.
Thus install `adafruit-nrfutil` using pip.
Then, get [this file](https://github.com/user-attachments/files/19794746/xiao_nrf52840_ble_bootloader-0.9.2-10-g4329614-dirty_s140_6.1.1.zip) and run `adafruit-nrfutil dfu serial -p BOARD_PORT -pkg xiao_nrf52840_ble_bootloader-0.9.2-10-g4329614-dirty_s140_6.1.1.zip`.
After, make sure that the .txt on the board shows Softdevice version 6.1.1.
After, you just need to copy the right file onto the board, which appears as a flash drive.
The firmware file to search for contains `xiao_ble.uf2`.
Firmware files can be found [in the releases](https://github.com/dakhnod/BLEnky/releases/latest).

Once the firmware is copied/flashed, the flash drive will disappear and you should be able to head to https://ble.nullco.de and connect to your board. You just saved yourself years of development!

To get back into the bootloader, a quick doble reset should suffice.