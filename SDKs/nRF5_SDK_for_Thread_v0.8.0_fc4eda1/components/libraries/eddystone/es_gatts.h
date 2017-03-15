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

#ifndef ES_GATTS_H__
#define ES_GATTS_H__

#include <stdint.h>
#include "nrf_ble_escs.h"

/**
 * @file
 * @defgroup eddystone_gatts GATTS
 * @brief Functions for handling GATTS write and read requests.
 * @ingroup eddystone
 * @{
 */

ret_code_t es_gatts_init(nrf_ble_escs_t * p_ble_escs);

/**@brief Function for handling all write requests from the Central.
 *
 * @param[in]   p_escs      Pointer to the Eddystone Configuration Service.
 * @param[in]   uuid        The UUID of the characteristic that is being written to.
 * @param[in]   val_handle  Value handle field of the characteristic handle of the characteristic that is being written to.
 * @param[in]   p_data      Pointer to the data to be written.
 * @param[in]   length      Length of the data to be written.
 *
 */
void es_gatts_handle_write(nrf_ble_escs_t * p_escs,
                                uint16_t    uuid,
                                uint16_t    val_handle,
                                uint8_t   * p_data,
                                uint16_t    length);


/**@brief Function for handling all read requests from the Central.
 *
 * @param[in]   p_escs      Pointer to the Eddystone Configuration Service.
 * @param[in]   uuid        The UUID of the characteristic that is being read from.
 * @param[in]   val_handle  Value handle field of the characteristic handle of the characteristic that is being read from.
 *
 */
void es_gatts_handle_read(nrf_ble_escs_t * p_escs, uint16_t uuid, uint16_t val_handle);

/**@brief Function for sending an RW-authorization reply.
 *
 * @param[in]   p_escs      Pointer to the Eddystone Configuration Service.
 * @param[in]   p_reply     Pointer to the reply to send.
 *
 * @retval NRF_SUCCESS              If the reply was successfully issued to the SoftDevice.
 * @retval NRF_ERROR_NULL           If either of the pointers @p p_escs or @p p_reply is NULL.
 * @retval NRF_ERROR_INVALID_STATE  If the connection handle of @p p_escs is invalid.
 * @return                          Otherwise, an error code from sd_ble_gatts_rw_authorize_reply() is returned.
 */
ret_code_t es_gatts_send_reply(nrf_ble_escs_t * p_escs, ble_gatts_rw_authorize_reply_params_t * p_reply);

/**@brief Function for sending an RW-authorization reply with status 'Operation not permitted'.
 *
 * @param[in]   p_escs      Pointer to the Eddystone Configuration Service.
 * @param[in]   op_is_read  Flag that specifies if the operation being responded to is a 'read' operation.
                            If false, a 'write' operation is assumed.
 *
 * @retval NRF_ERROR_NULL   If @p p_escs is NULL.
 * @return                  Otherwise, the error code from es_gatts_send_reply() is returned.
 */
ret_code_t es_gatts_send_op_not_permitted(nrf_ble_escs_t * p_escs, bool op_is_read);

/**
 * @}
 */

#endif // ES_GATTS_H__
