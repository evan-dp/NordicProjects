/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x1f000, LENGTH = 0xD5000
  RAM (rwx) :  ORIGIN = 0x20005000, LENGTH = 0x3B000

  /** Location of bootloader setting in flash. */
  BOOTLOADER_SETTINGS (rw) : ORIGIN = 0x000FF000, LENGTH = 0x1000

  /** Location of OpenThread settings in flash. */
  OPENTHREAD_SETTINGS (rw) : ORIGIN = 0x000F4000, LENGTH = 0x4000
}

SECTIONS
{
  /* Place the bootloader settings page in flash. */
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
    KEEP(*(.fs_data))
    PROVIDE(__stop_pwr_mgmt_data = .);
  } > FLASH

  __flash_data_end = (ORIGIN(OPENTHREAD_SETTINGS) + LENGTH(OPENTHREAD_SETTINGS));
  __flash_data_start = ORIGIN(OPENTHREAD_SETTINGS);

} INSERT AFTER .data;

INCLUDE "nrf5x_common.ld"
