/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
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

/** @file
 * @defgroup nrf_mtx nRF Mutex
 * @{
 * @ingroup app_common
 * @brief Mutex used for protecting resources.
 *
 * This module provides a mutex that can be used to ensure only one context may enter a critical
 * section holding the lock.
 */
#ifndef NRF_MTX_H__ 
#define NRF_MTX_H__ 

#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"
#include "nrf_atomic.h"
#include "nrf_assert.h"

#define NRF_MTX_LOCKED      1
#define NRF_MTX_UNLOCKED    0

/**
 * @brief Mutex data type.
 *
 * All fields in this struct are internal, and should never be modified outside of the nrf_mtx_*
 * functions.
 */
typedef nrf_atomic_u32_t nrf_mtx_t;

/**
 * @brief Initialize mutex.
 *
 * This function _must_ be called before nrf_mtx_trylock() and nrf_mtx_unlock() functions.
 *
 * @param[out] p_mtx The mutex to be initialized.
 */
__STATIC_INLINE void nrf_mtx_init(nrf_mtx_t * p_mtx);

/**
 * @brief Try to lock a mutex.
 *
 * If the mutex is already held by another context, this function will return immediately.
 *
 * @param[in, out] p_mtx The mutex to be locked.
 * @return true if lock was acquired, false if not
 */
__STATIC_INLINE bool nrf_mtx_trylock(nrf_mtx_t * p_mtx);

/**
 * @brief Unlock a mutex. 
 *
 * This function _must_ only be called when holding the lock. Unlocking a mutex which you do not 
 * hold will give undefined behavior.
 *
 * @param[in, out] p_mtx The mutex to be unlocked.
 */
__STATIC_INLINE void nrf_mtx_unlock(nrf_mtx_t * p_mtx);

#ifndef SUPPRESS_INLINE_IMPLEMENTATION

__STATIC_INLINE void nrf_mtx_init(nrf_mtx_t * p_mtx)
{
    ASSERT(p_mtx  != NULL);

    *p_mtx = NRF_MTX_UNLOCKED;
    __DMB();
}

__STATIC_INLINE bool nrf_mtx_trylock(nrf_mtx_t * p_mtx)
{
    ASSERT(p_mtx  != NULL);

    uint32_t old_val = nrf_atomic_u32_store_fetch(p_mtx, NRF_MTX_LOCKED);

    return (old_val == NRF_MTX_UNLOCKED);
}

__STATIC_INLINE void nrf_mtx_unlock(nrf_mtx_t * p_mtx)
{
    ASSERT(p_mtx  != NULL);
    ASSERT(*p_mtx == NRF_MTX_LOCKED);

    *p_mtx = NRF_MTX_UNLOCKED;
    __DMB();
}

#endif //SUPPRESS_INLINE_IMPLEMENTATION

#endif // NRF_MTX_H__
/** @} */
