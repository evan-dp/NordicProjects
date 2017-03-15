/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**
 * @ingroup twi_master_with_twis_slave_example
 * @defgroup twi_master_with_twis_slave_example_config Example code configuration
 *
 * Configuration for the code presenting TWIS and TWI functionality.
 * @{
 */

#ifndef TWI_MASTER_WITH_TWIS_SLAVE_CONFIG_H__
#define TWI_MASTER_WITH_TWIS_SLAVE_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EEPROM_SIM_SIZE                   320  //!< Simulated EEPROM size.

/* Maximum number of bytes writable to this slave emulator in one sequential access including 
   address of the slave memory
 */
#define EEPROM_SIM_SEQ_WRITE_MAX_BYTES    200

/* Maximum number of bytes writable to this slave emulator in one sequential access */
#define EEPROM_SIM_SEQ_READ_MAX_BYTES     200     //<! Number of data bytes transfer in single request

#define EEPROM_SIM_ADDR                   0x50    //!< Simulated EEPROM TWI slave address.


#define EEPROM_SIM_SCL_S         31   //!< Slave SCL pin.
#define EEPROM_SIM_SDA_S         30   //!< Slave SDA pin.

#define EEPROM_SIM_TWIS_INST     1    //!< TWIS interface used by EEPROM simulator.

/* Flash start address to load the RAM with at startup */
#define EEPROM_SIM_FLASH_ADDRESS  0x8000

/* Slave memory addressing byte length */
#define EEPROM_SIM_ADDRESS_LEN_BYTES    2

/* if EEPROM_SIM_ADDRESS_LEN_BYTES == 2, below will configure which byte is sent first by master */
/**
 * @enum address_byte_endian
 * @brief Endianness of the address byte that is received from master.
 */
typedef enum
{
    /*lint -save -e30*/
    BIG_ENDIAN = 0,   /**< MSB is sent first by master for address. */
    LITTLE_ENDIAN,    /**< LSB is sent first by master for address. */
} address_byte_endian;

#define TWI_ADDRESS_CONFIG    LITTLE_ENDIAN

/* Master Configuration */
#define MASTER_TWI_INST          0    //!< TWI interface used as a master accessing EEPROM memory.
#define UART_TX_BUF_SIZE         1024 //!< UART TX buffer size.
#define UART_RX_BUF_SIZE         32   //!< UART RX buffer size.
#define TWI_SCL_M                3    //!< Master SCL pin.
#define TWI_SDA_M                4    //!< Master SDA pin.
#define IN_LINE_PRINT_CNT        16   //!< Number of data bytes printed in a single line.


/** @} */

#ifdef __cplusplus
}
#endif

#endif // TWI_MASTER_WITH_TWIS_SLAVE_CONFIG_H__
