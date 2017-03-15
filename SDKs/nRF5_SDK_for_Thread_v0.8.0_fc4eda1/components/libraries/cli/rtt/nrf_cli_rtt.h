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

#ifndef NRF_CLI_RTT_H__
#define NRF_CLI_RTT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_cli.h"


/**@file
 *
 * @defgroup nrf_cli_rtt RTT command line interface transport layer.
 * @ingroup nrf_cli
 *
 * @{
 *
 */

/**
 * @brief Command line interface transport.
 * */
extern const nrf_cli_transport_t nrf_cli_rtt_transport;


/** @} */


#ifdef __cplusplus
}
#endif

#endif /* NRF_CLI_RTT_H__ */
