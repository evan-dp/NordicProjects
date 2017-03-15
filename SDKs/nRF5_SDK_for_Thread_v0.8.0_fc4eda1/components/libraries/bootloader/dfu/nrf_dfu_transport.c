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

#include "nrf_dfu_transport.h"
#include "nrf_log.h"


#define DFU_TRANS_SECTION_ITEM_GET(i)       NRF_SECTION_ITEM_GET(dfu_trans, nrf_dfu_transport_t, (i))
#define DFU_TRANS_SECTION_ITEM_COUNT        NRF_SECTION_ITEM_COUNT(dfu_trans, nrf_dfu_transport_t)

//lint -esym(526, dfu_transBase) -esym(526, dfu_transLimit)
NRF_SECTION_DEF(dfu_trans, const nrf_dfu_transport_t);

uint32_t nrf_dfu_transports_init(void)
{
    uint32_t const num_transports = DFU_TRANS_SECTION_ITEM_COUNT;
    uint32_t ret_val = NRF_SUCCESS;

    NRF_LOG_DEBUG("In nrf_dfu_transports_init\r\n");

    NRF_LOG_DEBUG("num transports: %d\r\n", num_transports);

    for (uint32_t i = 0; i < num_transports; i++)
    {
        nrf_dfu_transport_t * const trans = DFU_TRANS_SECTION_ITEM_GET(i);
        ret_val = trans->init_func();
        if (ret_val != NRF_SUCCESS)
        {
            break;
        }
    }

    NRF_LOG_DEBUG("After nrf_dfu_transports_init\r\n");

    return ret_val;
}


uint32_t nrf_dfu_transports_close(void)
{
    uint32_t const num_transports = DFU_TRANS_SECTION_ITEM_COUNT;
    uint32_t ret_val = NRF_SUCCESS;

    NRF_LOG_DEBUG("In nrf_dfu_transports_close\r\n");

    NRF_LOG_DEBUG("num transports: %d\r\n", num_transports);

    for (uint32_t i = 0; i < num_transports; i++)
    {
        nrf_dfu_transport_t * const trans = DFU_TRANS_SECTION_ITEM_GET(i);
        ret_val = trans->close_func();
        if (ret_val != NRF_SUCCESS)
        {
            break;
        }
    }

    NRF_LOG_DEBUG("After nrf_dfu_transports_init\r\n");

    return ret_val;
}
