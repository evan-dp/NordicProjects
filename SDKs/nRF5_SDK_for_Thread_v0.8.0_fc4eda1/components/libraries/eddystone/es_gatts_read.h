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

#ifndef ES_GATTS_READ_H__
#define ES_GATTS_READ_H__

#include <stdint.h>
#include "nrf_ble_escs.h"

/**
 * @file
 * @defgroup eddystone_gatts_read GATTS read
 * @brief Functions for handling GATTS read requests.
 * @ingroup eddystone_gatts
 * @{
 */

ret_code_t es_gatts_read_send_not_permitted(nrf_ble_escs_t * p_escs);

ret_code_t es_gatts_read_handle_unlocked_read(nrf_ble_escs_t * p_escs,
                                              uint16_t         uuid,
                                              uint16_t         val_handle,
                                              uint8_t          active_slot,
                                              uint8_t          lock_state);

ret_code_t es_gatts_read_handle_unlock(nrf_ble_escs_t * p_escs);

ret_code_t es_gatts_read_handle_locked_read(nrf_ble_escs_t * p_escs,
                                      uint16_t         uuid,
                                      uint8_t          lock_state);

/**
 * @}
 */

#endif // ES_GATTS_READ_H__
