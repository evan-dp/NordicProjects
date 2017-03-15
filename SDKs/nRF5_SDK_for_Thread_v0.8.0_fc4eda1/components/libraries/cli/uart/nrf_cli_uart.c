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
#if NRF_MODULE_ENABLED(NRF_CLI_UART)
#include "nrf_cli_uart.h"
#include "app_uart.h"
#include "boards.h"

static void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


static ret_code_t cli_uart_init(void)
{
    uint32_t err_code;

    const app_uart_comm_params_t comm_params =
      {
          RX_PIN_NUMBER,
          TX_PIN_NUMBER,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          APP_UART_FLOW_CONTROL_ENABLED,
          false,
          UART_BAUDRATE_BAUDRATE_Baud115200
      };

    APP_UART_FIFO_INIT(&comm_params,
                         NRF_CLI_UART_RX_BUF_SIZE,
                         NRF_CLI_UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    return err_code;
}

static ret_code_t cli_uart_uninit(void)
{
    return NRF_SUCCESS;
}

static ret_code_t cli_uart_read(void * p_data,
                               size_t length,
                               size_t * p_cnt)
{
    uint8_t * p_buff = p_data;
    size_t acc = 0;

    while (app_uart_get(p_buff) == NRF_SUCCESS)
    {
        ++p_buff;
        ++acc;

        if (acc == length)
        {
            break;
        }
    }

    if (p_cnt)
    {
        *p_cnt = acc;
    }

    return NRF_SUCCESS;
}

static ret_code_t cli_uart_write(const void * p_data,
                                size_t length,
                                size_t * p_cnt)
{
    size_t acc = 0;

    uint8_t const * p_buff = p_data;
    do {
        while (app_uart_put(*p_buff) != NRF_SUCCESS)
        {

        }

        ++p_buff;
        ++acc;
    } while (acc < length);

    if (p_cnt)
    {
        *p_cnt = acc;
    }

    return NRF_SUCCESS;
}

const nrf_cli_transport_t nrf_cli_uart_transport = {
        .init = cli_uart_init,
        .uninit = cli_uart_uninit,
        .read = cli_uart_read,
        .write = cli_uart_write,
};

#endif

