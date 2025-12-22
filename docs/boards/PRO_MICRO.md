# Pro micro / Nice nano

## Flashing

These boards come pre-flashed with a uf2 bootloader, so you just need to copy the right file onto the board, which appears as a flash drive.
The firmware file to search for contains `others_promicro_nrf52840.uf2`.
Firmware files can be found [in the releases](https://github.com/dakhnod/BLEnky/releases/latest).

Once the firmware is copied/flashed, the flash drive will disappear and you should be able to head to https://ble.nullco.de and connect to your board. You just saved yourself years of development!

To get back into the bootloader, a quick doble reset should suffice.

## Low power

In order to get the lowest power ingest (microamperes), supply around 3V to the pin marked in this picture:

![PXL_20251222_225933250](https://github.com/user-attachments/assets/f777dd82-d1de-4d68-a1eb-dd396289568c)

Then, configure pin 0.13 as output and default low, to disable the LDO.
I managed to get around 10uA of power consumption this way.
As far as I have discovered so far, no other pin is connected to the 3V input of the nRF.
As such, all other power pins consume more power.

You can also use the B+/RAW pins to supply power, but that result in around 30uA of power consumption.

Please open an Issue if I am wrong.

Looks like providing 3V voltage to the B+/RAW pin also causes very low power consumption.
I measured below 0.2mA on B+/RAW, so that may work out aswell.
Setting 0.13 to low reduces to around 30uA, but you should provide around 3V I believe.

With a Lithium battery connected to B+/RAW you should leave the LDO on (0.13 at high), which will lead to around 200uA power consumption.
With a 1000mA battery this gives you around 200 days of battery life.

If you measure power consumption around 200uA, try [this](https://github.com/joric/nrfmicro/wiki/Alternatives/dd5782fb56855cc7e24e884f1e423d664da34db1) under "Workarounds".
