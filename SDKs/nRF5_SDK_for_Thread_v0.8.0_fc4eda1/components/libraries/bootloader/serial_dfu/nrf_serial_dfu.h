/* Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
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

/**@file
 *
 * @defgroup nrf_serial_dfu Serial DFU transport layer
 * @{
 * @ingroup  sdk_nrf_bootloader
 * @brief    Device Firmware Update (DFU) transport layer using UART.
 *
 * @details  The transport layer can be used for performing firmware updates over UART. 
 *           The implementation uses SLIP to encode packets.
 */

#ifndef NRF_SERIAL_DFU_H__
#define NRF_SERIAL_DFU_H__

#include <stdint.h>
#include "nrf_drv_uart.h"
#include "slip.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Serial DFU opcodes.
 */
typedef enum
{
    SERIAL_DFU_OP_CODE_CREATE_OBJECT               = 0x01,                                 /**< Value of the opcode field for a 'Create object' request. */
    SERIAL_DFU_OP_CODE_SET_RECEIPT_NOTIF           = 0x02,                                 /**< Value of the opcode field for a 'Set Packet Receipt Notification' request. */
    SERIAL_DFU_OP_CODE_CALCULATE_CRC               = 0x03,                                 /**< Value of the opcode field for a 'Calculating checksum' request. */
    SERIAL_DFU_OP_CODE_EXECUTE_OBJECT              = 0x04,                                 /**< Value of the opcode field for an 'Initialize DFU parameters' request. */
    SERIAL_DFU_OP_CODE_SELECT_OBJECT               = 0x06,                                 /**< Value of the opcode field for a 'Select object' request. */
    SERIAL_DFU_OP_CODE_GET_SERIAL_MTU              = 0x07,                                 /**< Value of the opcode field for a 'Get Serial MTU' request. */
    SERIAL_DFU_OP_CODE_WRITE_OBJECT                = 0x08,                                 /**< Value of the opcode indicating a write to the current object. */
    SERIAL_DFU_OP_CODE_RESPONSE                    = 0x60                                  /**< Value of the opcode field for a response.*/
} serial_dfu_op_code_t;

/**@brief   DFU transport layer state.
 *
 * @details This structure contains status information related to the transport layer.
 */
typedef struct
{
    nrf_drv_uart_t      uart_instance;            /**< Structure holding the state of the UART driver. */
    slip_t              slip;
    
    uint8_t             uart_buffer;
    uint8_t             recv_buffer[256 + 1];

    uint16_t            pkt_notif_target;
    uint16_t            pkt_notif_target_count;
} serial_dfu_t;


/**@brief      Function for initializing the transport layer.
 *
 * @retval     NRF_SUCCESS If the transport layer was successfully initialized. Otherwise, an error code is returned.
 */
uint32_t serial_dfu_transport_init(void);


/**@brief      Function for closing down the transport layer.
 *
 * @retval     NRF_SUCCESS If the transport layer was correctly closed down.
 */
uint32_t serial_dfu_transport_close(void);

#ifdef __cplusplus
}
#endif

#endif // NRF_SERIAL_DFU_H__

/** @} */
