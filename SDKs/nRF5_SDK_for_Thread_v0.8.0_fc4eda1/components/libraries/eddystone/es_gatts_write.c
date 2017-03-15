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

#include "es_gatts_write.h"
#include "es_adv.h"
#include "es_flash.h"
#include "es_gatts.h"
#include "es_security.h"


static ret_code_t send_write_reply(nrf_ble_escs_t * p_escs, ble_gatts_rw_authorize_reply_params_t * p_reply)
{
    VERIFY_PARAM_NOT_NULL(p_escs);
    VERIFY_PARAM_NOT_NULL(p_reply);

    p_reply->type                = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
    p_reply->params.write.update = 1;
    p_reply->params.write.offset = 0;

    return es_gatts_send_reply(p_escs, p_reply);
}


/**@brief Function checking if length of event is correct, given the frame data.
 *
 * @param[in] p_data Written ADV Slot data.
 * @param[in] length Written length.
 *
 * @retval true If length is valid.
 * @retval false If length is not valid.
 */
static bool length_is_valid(uint8_t * p_data, uint8_t length)
{
    if (length == 0 || (length == 1 && p_data[0] == 0))
    {
        return true;
    }

    else
    {
        switch ((es_frame_type_t)p_data[0])
        {
            case ES_FRAME_TYPE_UID:
                return length == ESCS_UID_WRITE_LENGTH;

            case ES_FRAME_TYPE_URL:
                return ((length >= ESCS_URL_MIN_WRITE_LENGTH) && (length <= ESCS_URL_WRITE_LENGTH));

            case ES_FRAME_TYPE_TLM:
                return (length == ESCS_TLM_WRITE_LENGTH);

            case ES_FRAME_TYPE_EID:
                return ((length == ESCS_EID_WRITE_ECDH_LENGTH) ||
                        (length == ESCS_EID_WRITE_IDK_LENGTH));

            default:
                return false;
        }
    }
}


ret_code_t es_gatts_write_handle_unlocked_write(nrf_ble_escs_t * p_escs,
                                                uint16_t         uuid,
                                                uint16_t         val_handle,
                                                uint8_t *        p_data,
                                                uint16_t         length,
                                                uint8_t          active_slot)
{
    VERIFY_PARAM_NOT_NULL(p_escs);
    VERIFY_PARAM_NOT_NULL(p_data);

    ret_code_t                            err_code;
    ble_gatts_rw_authorize_reply_params_t reply      = {0};
    bool                                  long_write = false;

    reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;

    switch (uuid)
    {
        case BLE_UUID_ESCS_ACTIVE_SLOT_CHAR:
            if (*p_data > APP_MAX_ADV_SLOTS - 1)
            {
                // Invalid Attribute Length: for an attempt to write illegal values.
                // The beacon will list the total number of available slots in the
                // max_supported_total_slots field in the Capabilities characteristic.
                reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_INVALID_ATT_VAL_LENGTH;
                length                         = 0;
            }
            break;

        case BLE_UUID_ESCS_ADV_INTERVAL_CHAR:
            es_adv_interval_set(BYTES_SWAP_16BIT(*(nrf_ble_escs_adv_interval_t *)p_data));
            break;

        case BLE_UUID_ESCS_RADIO_TX_PWR_CHAR:
            es_slot_radio_tx_pwr_set(active_slot, *(nrf_ble_escs_radio_tx_pwr_t *)(p_data));
            break;

        case BLE_UUID_ESCS_ADV_TX_PWR_CHAR:
            // Update slot info so that ADV data will only be read from what is written by client.
            es_slot_set_adv_custom_tx_power(active_slot, *(nrf_ble_escs_radio_tx_pwr_t *)(p_data));
            break;

        case BLE_UUID_ESCS_LOCK_STATE_CHAR:
            if (length == 1 && (*p_data == NRF_BLE_ESCS_LOCK_BYTE_LOCK ||
                                *p_data == NRF_BLE_ESCS_LOCK_BYTE_DISABLE_AUTO_RELOCK))
            {
                // Do nothing special, allow the write.
            }
            else if (length == ESCS_LOCK_STATE_NEW_LOCK_CODE_WRITE_LENGTH &&
                     *p_data == NRF_BLE_ESCS_LOCK_BYTE_LOCK)
            {
                // 0x00 + key[16] : transition to lock state and update the lock code.
                err_code = es_security_lock_code_update((p_data) + 1);
                RETURN_IF_ERROR(err_code);

                // Only write the lock byte (0x00) to the characteristic, so set length to 1.
                length = 1;
            }
            else
            {
                // Any invalid values locks the characteristic by default.
                *p_data = NRF_BLE_ESCS_LOCK_BYTE_LOCK;
                length  = 1;
            }
            break;

        case BLE_UUID_ESCS_RW_ADV_SLOT_CHAR:
            if (length > 20)
            {
                long_write = true;
            }
            reply.params.write.gatt_status = length_is_valid(p_data, length)
                                                 ? BLE_GATT_STATUS_SUCCESS
                                                 : BLE_GATT_STATUS_ATTERR_INVALID_ATT_VAL_LENGTH;

            if (reply.params.write.gatt_status == BLE_GATT_STATUS_SUCCESS)
            {
                es_slot_on_write(active_slot, length, p_data);
                es_adv_interval_set(es_adv_interval_get()); // Ensure that valid advertisement interval is used.
            }
            break;

        case BLE_UUID_ESCS_FACTORY_RESET_CHAR:
            if (*p_data == 0x0B)
            {
                err_code = es_flash_factory_reset();
                RETURN_IF_ERROR(err_code);
            }
            break;

        case BLE_UUID_ESCS_REMAIN_CONNECTABLE_CHAR:
#if APP_IS_REMAIN_CONNECTABLE_SUPPORTED == ESCS_FUNCT_REMAIN_CONNECTABLE_SUPPORTED_Yes
            if (*p_data != 0)
            {
                es_adv_remain_connectable_set(true);
            }

            else
            {
                es_adv_remain_connectable_set(false);
            }
#endif
            break;

        default:
            break;
    }
    reply.params.write.len    = length;
    reply.params.write.p_data = p_data;

    if (!long_write)
    {
        return send_write_reply(p_escs, &reply);
    }

    else
    {
        return NRF_SUCCESS;
    }
}


ret_code_t es_gatts_write_handle_unlock(nrf_ble_escs_t * p_escs,
                                        uint8_t *        p_data,
                                        uint16_t         length,
                                        uint16_t         val_handle)
{
    VERIFY_PARAM_NOT_NULL(p_escs);
    VERIFY_PARAM_NOT_NULL(p_data);
    
    ret_code_t                            err_code;
    ble_gatts_rw_authorize_reply_params_t reply = {0};
    ble_gatts_value_t                     value = {.len = length, .offset = 0, .p_value = p_data};

    if (length == ESCS_AES_KEY_SIZE)
    {
        err_code = sd_ble_gatts_value_set(p_escs->conn_handle, val_handle, &value);

        if (err_code == NRF_SUCCESS)
        {
            reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
            es_security_unlock_verify((value.p_value));
        }

        else
        {
            reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
        }
    }

    reply.params.write.len    = length;
    reply.params.write.p_data = (const uint8_t *)value.p_value;

    return send_write_reply(p_escs, &reply);
}
