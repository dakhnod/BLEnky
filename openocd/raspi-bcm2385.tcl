set WORKAREASIZE 0

adapter driver bcm2835gpio

bcm2835gpio peripheral_base 0xFE000000

bcm2835gpio speed_coeffs 236181 60

adapter gpio swclk 11
adapter gpio swdio 25

transport select swd
