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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "es_stopwatch.h"
#include "sdk_macros.h"
#include "app_timer.h"
#include "es_app_config.h"

static uint32_t m_ticks_last_returned[ES_STOPWATCH_MAX_USERS];
static uint32_t m_ids_ticks_wrap[ES_STOPWATCH_MAX_USERS];
static uint8_t  m_nof_ids     = 0;
static bool     m_initialized = false;

uint32_t es_stopwatch_check(es_stopwatch_id_t id)
{
    uint32_t ticks_current = app_timer_cnt_get();
    uint32_t ticks_diff;

    if (m_ids_ticks_wrap[id] == 0)
    {
        APP_ERROR_CHECK(NRF_ERROR_INVALID_STATE);
    }

    ticks_diff = app_timer_cnt_diff_compute(ticks_current, m_ticks_last_returned[id]);

    if (ticks_diff >= m_ids_ticks_wrap[id])
    {
        m_ticks_last_returned[id] = (ticks_current / m_ids_ticks_wrap[id]) * m_ids_ticks_wrap[id];

        return ticks_diff / m_ids_ticks_wrap[id];
    }

    return 0;
}

ret_code_t es_stopwatch_create(es_stopwatch_id_t * p_sw_id, uint32_t ticks_wrap)
{
    VERIFY_PARAM_NOT_NULL(p_sw_id);

    if (m_nof_ids == ES_STOPWATCH_MAX_USERS)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (!m_initialized)
    {
        return NRF_ERROR_MODULE_NOT_INITIALZED;
    }

    *p_sw_id = m_nof_ids;

    m_ids_ticks_wrap[m_nof_ids] = ticks_wrap;

    m_nof_ids++;

    return NRF_SUCCESS;
}


void es_stopwatch_init(void)
{
    m_nof_ids = 0;
    memset(m_ticks_last_returned, 0, sizeof(m_ticks_last_returned));
    memset(m_ids_ticks_wrap, 0, sizeof(m_ids_ticks_wrap));
    m_initialized = true;
}
