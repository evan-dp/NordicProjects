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

#ifndef ES_GATTS_WRITE_H__
#define ES_GATTS_WRITE_H__

#include <stdint.h>
#include "nrf_ble_escs.h"

/**
 * @file
 * @defgroup eddystone_gatts_write GATTS write
 * @brief Functions for handling GATTS write requests.
 * @ingroup eddystone_gatts
 * @{
 */

ret_code_t es_gatts_write_handle_unlocked_write(nrf_ble_escs_t * p_escs,
                                                uint16_t         uuid,
                                                uint16_t         val_handle,
                                                uint8_t *        p_data,
                                                uint16_t         length,
                                                uint8_t          active_slot);

ret_code_t es_gatts_write_handle_unlock(nrf_ble_escs_t * p_escs,
                                        uint8_t *        p_data,
                                        uint16_t         length,
                                        uint16_t         val_handle);

/**
 * @}
 */

#endif // ES_GATTS_WRITE_H__
