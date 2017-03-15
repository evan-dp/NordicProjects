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

#include "es_gatts.h"
#include "es_gatts_read.h"
#include "es_gatts_write.h"
#include "es_slot.h"

static nrf_ble_escs_lock_state_read_t m_lock_state;
static uint8_t                        m_active_slot;

/**@brief Function checking if beacon is unlocked.
 *
 * @param[in] p_escs Pointer to Eddystone Configuration Service.
 *
 * @retval true  If beacon is unlocked.
 * @retval false If beacon is locked.
 */
static bool is_beacon_unlocked(const nrf_ble_escs_t * p_escs)
{
    return m_lock_state != NRF_BLE_ESCS_LOCK_STATE_LOCKED;
}


ret_code_t es_gatts_send_reply(nrf_ble_escs_t *                        p_escs,
                               ble_gatts_rw_authorize_reply_params_t * p_reply)
{
    VERIFY_PARAM_NOT_NULL(p_escs);
    VERIFY_PARAM_NOT_NULL(p_reply);

    if (p_escs->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        return sd_ble_gatts_rw_authorize_reply(p_escs->conn_handle, p_reply);
    }

    return NRF_ERROR_INVALID_STATE;
}


ret_code_t es_gatts_send_op_not_permitted(nrf_ble_escs_t * p_escs, bool read)
{
    ble_gatts_rw_authorize_reply_params_t reply = {0};

    VERIFY_PARAM_NOT_NULL(p_escs);

    if (read)
    {
        reply.type                    = BLE_GATTS_AUTHORIZE_TYPE_READ;
        reply.params.read.gatt_status = BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED;
    }

    else
    {
        reply.type                     = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
        reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }

    return es_gatts_send_reply(p_escs, &reply);
}



void es_gatts_handle_write(nrf_ble_escs_t * p_escs,
                                uint16_t    uuid,
                                uint16_t    val_handle,
                                uint8_t   * p_data,
                                uint16_t    length)
{
    ret_code_t err_code;

    if (is_beacon_unlocked(p_escs))
    {
        if (uuid == BLE_UUID_ESCS_UNLOCK_CHAR)
        {
            err_code = es_gatts_send_op_not_permitted(p_escs, false);
            APP_ERROR_CHECK(err_code);
        }

        else
        {
            err_code = es_gatts_write_handle_unlocked_write(
                p_escs, uuid, val_handle, p_data, length, m_active_slot);
            APP_ERROR_CHECK(err_code);
        }
    }

    else
    {
        if (uuid == BLE_UUID_ESCS_UNLOCK_CHAR)
        {
            err_code = es_gatts_write_handle_unlock(p_escs, p_data, length, val_handle);
            APP_ERROR_CHECK(err_code);
        }

        else
        {
            err_code = es_gatts_send_op_not_permitted(p_escs, false);
            APP_ERROR_CHECK(err_code);
        }
    }
}


void es_gatts_handle_read(nrf_ble_escs_t * p_escs, uint16_t uuid, uint16_t val_handle)
{
    ret_code_t err_code;

    if (is_beacon_unlocked(p_escs))
    {
        if (uuid == BLE_UUID_ESCS_UNLOCK_CHAR)
        {
            err_code = es_gatts_send_op_not_permitted(p_escs, true);
            APP_ERROR_CHECK(err_code);
        }

        else
        {
            err_code = es_gatts_read_handle_unlocked_read(p_escs, uuid, val_handle, m_active_slot, m_lock_state);
            APP_ERROR_CHECK(err_code);
        }
    }

    else // Beacon is locked.
    {
        if (uuid == BLE_UUID_ESCS_UNLOCK_CHAR)
        {
            err_code = es_gatts_read_handle_unlock(p_escs);
            APP_ERROR_CHECK(err_code);
        }

        else
        {
            err_code = es_gatts_read_handle_locked_read(p_escs, uuid, m_lock_state);
            APP_ERROR_CHECK(err_code);
        }
    }
}


ret_code_t es_gatts_init(nrf_ble_escs_t * p_ble_escs)
{
    VERIFY_PARAM_NOT_NULL(p_ble_escs);

    m_active_slot = 0;
    m_lock_state  = NRF_BLE_ESCS_LOCK_STATE_LOCKED;

    p_ble_escs->p_active_slot = &m_active_slot;
    p_ble_escs->p_lock_state  = &m_lock_state;

    return NRF_SUCCESS;
}
