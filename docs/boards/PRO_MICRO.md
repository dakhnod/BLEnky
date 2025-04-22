# Pro micro / Nice nano

## Flashing

These boards come pre-flashed with a uf2 bootloader, so you just need to copy the right file onto the board, which appears as a flash drive.
The firmware file to search for contains `others_promicro_nrf52840.uf2`.
Firmware files can be found [in the releases](https://github.com/dakhnod/BLEnky/releases/latest).

Once the firmware is copied/flashed, the flash drive will disappear and you should be able to head to https://ble.nullco.de and connect to your board. You just saved yourself years of development!

To get back into the bootloader, a quick doble reset should suffice.

## Low power

In order to get the lowest power ingest (microamperes), supply around 3V to the pin marked in this picture:

![Image](https://github.com/user-attachments/assets/85d85e85-a2dd-4b9f-a323-fdbf0cab0c24)

As far as I have discovered so far, no other pin is connected to the 3V input of the nRF.
As such, all other power pins consume more power.

Please open an Issue if I am wrong.