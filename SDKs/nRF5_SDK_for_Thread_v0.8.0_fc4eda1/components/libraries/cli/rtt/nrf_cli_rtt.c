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



#include "sdk_common.h"
#if NRF_MODULE_ENABLED(NRF_CLI_RTT)
#include <SEGGER_RTT_Conf.h>
#include <SEGGER_RTT.h>
#include "nrf_cli_rtt.h"



static ret_code_t cli_rtt_init(void)
{
    SEGGER_RTT_Init();
    return NRF_SUCCESS;
}

static ret_code_t cli_rtt_uninit(void)
{
    return NRF_SUCCESS;
}

static ret_code_t cli_rtt_read(void * p_data,
                               size_t length,
                               size_t * p_cnt)
{
    size_t rcnt = SEGGER_RTT_Read(NRF_CLI_RTT_TERMINAL_ID, p_data, length);
    if (p_cnt)
    {
        *p_cnt = rcnt;
    }

    return NRF_SUCCESS;
}

static ret_code_t cli_rtt_write(const void * p_data,
                                size_t length,
                                size_t * p_cnt)
{
    size_t wcnt;
    size_t acc = 0;
    do {
        wcnt = SEGGER_RTT_Write(NRF_CLI_RTT_TERMINAL_ID, p_data, length);

        acc += wcnt;
        length -= wcnt;
    } while (length);

    if (p_cnt)
    {
        *p_cnt = acc;
    }

    return NRF_SUCCESS;
}

const nrf_cli_transport_t nrf_cli_rtt_transport = {
        .init = cli_rtt_init,
        .uninit = cli_rtt_uninit,
        .read = cli_rtt_read,
        .write = cli_rtt_write,
};

#endif
