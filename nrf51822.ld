/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  /* flash length is calculated using the flash origin of the bootloader_secure */
  FLASH (rx) : ORIGIN = 0x0001b000, LENGTH = 0x00035C00 - 0x0001b000
  RAM (rwx) :  ORIGIN = 0x20001ff8, LENGTH = 0x2008
  BOOTLOADER_SETTINGS (rw) : ORIGIN = 0x0003FC00, LENGTH = 0x0400
}

SECTIONS
{
  /* Ensures the bootloader settings are placed at the last flash page. */
  .bootloaderSettings(NOLOAD) :
  {

  } > BOOTLOADER_SETTINGS
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > RAM
  .pwr_mgmt_data :
  {
    PROVIDE(__start_pwr_mgmt_data = .);
    KEEP(*(.pwr_mgmt_data))
    PROVIDE(__stop_pwr_mgmt_data = .);
  } > RAM
} INSERT AFTER .data;

INCLUDE "nrf5x_common.ld"
