/* Copyright (c) 2017 Nordic Semiconductor. All Rights Reserved.
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

/**
 * @defgroup nrf_strerror Error code converter
 * @ingroup app_common
 *
 * @brief Module for converting error code into a printable string.
 * @{
 */
#ifndef NRF_STRERROR_H__
#define NRF_STRERROR_H__

#include "sdk_errors.h"

/**
 * @brief Function for getting a printable error string.
 *
 * @param code Error code to convert.
 *
 * @note This function cannot fail.
 *       For the function that may fail with error translation, see @ref nrf_strerror_find.
 *
 * @return Pointer to the printable string.
 *         If the string is not found,
 *         it returns a simple string that says that the error is unknown.
 */
char const * nrf_strerror_get(ret_code_t code);

/**
 * @brief Function for finding a printable error string.
 *
 * This function gets the error string in the same way as @ref nrf_strerror_get,
 * but if the string is not found, it returns NULL.
 *
 * @param code  Error code to convert.
 * @return 		Pointer to the printable string.
 *         		If the string is not found, NULL is returned.
 */
char const * nrf_strerror_find(ret_code_t code);

/** @} */

#endif /* NRF_STRERROR_H__ */
